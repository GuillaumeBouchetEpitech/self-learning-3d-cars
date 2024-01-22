
import Logger from "./Logger";
import {
  scriptLoadingUtility,
  getWebGl2Context,
} from "./helpers/index";
import {
  isWebWorkerSupported,
  isWasmSupported,
  isWebGL2Supported,
  isMultithreadingSupported,
} from "./environment/index";

export class Application {

  private _canvas: HTMLCanvasElement;
  private _webglCtx?: WebGL2RenderingContext;

  private _width: number = 800;
  private _height: number = 600;

  private _isInitialized: boolean = false;
  private _isAborted: boolean = false;
  private _wasmApplicationStartFunc: (width: number, height: number, totalGenomes: number, totalCores: number) => void;
  private _wasmApplicationUpdateFunc: (deltaTime: number) => void;
  private _wasmApplicationRenderFunc: () => void;
  private _onProgress: (precent: number) => void;
  private _onError: (text: string) => void;

  private _module: any;

  constructor(
    canvas: HTMLCanvasElement,
    onProgress: (precent: number) => void,
    onError: (text: string) => void,
  ) {
    this._canvas = canvas;
    this._onProgress = onProgress;
    this._onError = onError;
  }

  async initialize(
    width: number,
    height: number,
    totalGenomes: number,
    totalCores: number,
    inLogger: Logger,
  ): Promise<void> {

    this._width = width;
    this._height = height;

    //
    //
    // WebWorker support

    if (!isWebWorkerSupported()) {
      throw new Error("missing WebWorker feature (unsupported)");
    }
    inLogger.log("[JS][check] WebWorker feature => supported");

    //
    //
    // WebAssembly support

    if (!isWasmSupported()) {
      throw new Error("missing WebAssembly feature (unsupported)");
    }

    inLogger.log("[JS][check] WebAssembly feature => supported");

    //
    //
    // multithreading support

    const multithreadingSupported = isMultithreadingSupported();
    if (multithreadingSupported) {
      inLogger.log("[JS][check] multithreading => supported");
    } else {
      inLogger.log("[JS][check] multithreading => NOT supported");
      inLogger.log("[JS][check] => falling back to the webworker version");
    }

    //
    //
    // WebGL2 support

    if (!isWebGL2Supported()) {
      throw new Error("missing WebGL2 feature (unsupported)");
    }
    inLogger.log("[JS][check] WebGL2 feature => supported");

    //
    //
    // WebGL2 initialisation

    const onContextCreationError = (): void => {
      this._isAborted = true;
      if (this._onError)
        this._onError(`Could not create a WebGL2 context.`);
    };
    this._canvas.addEventListener("webglcontextcreationerror", onContextCreationError, false);

    const onWebGlContextLost = (): void => {
      this._isAborted = true;
      if (this._onError)
        this._onError("WebGL2 context lost. You will need to reload the page.");
    };
    this._canvas.addEventListener("webglcontextlost", onWebGlContextLost, false);

    // this prevent the contextual menu to appear on a right click
    const onContextMenu = (event: MouseEvent): void => {
      event.preventDefault();
    };
    this._canvas.addEventListener("contextmenu", onContextMenu, false);

    this._webglCtx = getWebGl2Context(this._canvas);
    inLogger.log("[JS] WebGL2 context initialized");

    //
    //
    // multithreading support

    await this._setupWasmApplication(
      multithreadingSupported,
      totalGenomes,
      totalCores,
      inLogger
    );
  }

  private async _fetchWasmScript(
    wasmFolder: string,
    inLogger: Logger,
  ): Promise<void> {

    inLogger.log("[JS][wasm] fetching");
    const fetchStartTime = Date.now();

    // this will globally expose the function selfDriving3dCars()
    await scriptLoadingUtility(`./${wasmFolder}/index.js`);

    const fetchStopTime = Date.now();
    const fetchElapsedTime = ((fetchStopTime - fetchStartTime) / 1000).toFixed(3);
    inLogger.log(`[JS][wasm] fetched ${fetchElapsedTime}sec`);
  }

  private async _loadWasmApplication(
    wasmFolder: string,
    inLogger: Logger,
  ): Promise<void> {

    inLogger.log("[JS] loading");
    const loadStartTime = Date.now();

    const downloadingDataRegExp = /Downloading data\.\.\. \(([0-9]*)\/([0-9]*)\)/;
    let lastProgressLevel = 0;

    const moduleArgs = {
      locateFile: (url: string) => `${wasmFolder}/${url}`,
      print: (text: string) => { inLogger.log(`[C++][out] ${text}`); },
      printErr: (text: string) => { inLogger.error(`[C++][err] ${text}`); },
      setStatus: (text: string) => {

        if (!text) {
          return;
        }

        // is the current message a "Downloading data..." one?
        const capture = downloadingDataRegExp.exec(text);
        if (!capture) {
          // no -> just a regular status message
          inLogger.log(`[JS][wasm][status] ${text}`);
          return;
        }

        const current = parseFloat(capture[1]);
        const total = parseFloat(capture[2]);
        const percent = Math.floor((current / total) * 100);

        if (lastProgressLevel === percent) {
          return;
        }

        lastProgressLevel = percent;
        this._onProgress(percent);
      },
      onRuntimeInitialized: () => {
        inLogger.log("[JS][wasm] runtime initialized");
      },
      canvas: this._canvas,
      preinitializedWebGLContext: this._webglCtx,
      noInitialRun: true,
      noExitRuntime: true,
    };

    // @ts-ignore
    this._module = await selfDriving3dCars(moduleArgs);

    const loadStopTime = Date.now();
    const loadElapsedTime = ((loadStopTime - loadStartTime) / 1000).toFixed(3);
    inLogger.log(`[JS] loaded ${loadElapsedTime}sec`);

  }

  private _initializeWasmApplication(inLogger: Logger): void {

    inLogger.log("[JS][wasm] initializing");
    const initStartTime = Date.now();

    const wasmFunctions = {
      startApplication: this._module.cwrap('startApplication', undefined, ['number', 'number', 'number', 'number']),
      updateApplication: this._module.cwrap('updateApplication', undefined, ['number']),
      renderApplication: this._module.cwrap('renderApplication', undefined, []),
    };

    this._wasmApplicationStartFunc = wasmFunctions.startApplication;
    this._wasmApplicationUpdateFunc = wasmFunctions.updateApplication;
    this._wasmApplicationRenderFunc = wasmFunctions.renderApplication;

    this._isInitialized = true;

    const initStopTime = Date.now();
    const initElapsedTime = ((initStopTime - initStartTime) / 1000).toFixed(3);
    inLogger.log(`[JS][wasm] initialized ${initElapsedTime}sec`);

  }

  private async _setupWasmApplication(
    multithreadingSupported: boolean,
    totalGenomes: number,
    totalCores: number,
    inLogger: Logger,
  ): Promise<void> {

    const wasmFolder = `wasm/${multithreadingSupported ? "pthread" : "webworker"}`;

    //
    //
    //

    await this._fetchWasmScript(wasmFolder, inLogger);
    await this._loadWasmApplication(wasmFolder, inLogger);
    this._initializeWasmApplication(inLogger);

    //
    //
    //

    inLogger.log("[JS][wasm] running");

    this._wasmApplicationStartFunc(this._width, this._height, totalGenomes, totalCores);
  }

  update(deltaTime: number): void {
    if (!this._isInitialized || this._isAborted) {
      return;
    }

    if (this._wasmApplicationUpdateFunc) {
      this._wasmApplicationUpdateFunc(deltaTime);
    }
  }

  render(): void {
    if (!this._isInitialized || this._isAborted) {
      return;
    }

    if (this._wasmApplicationRenderFunc) {
      this._wasmApplicationRenderFunc();
    }
  }

  abort(): void {

    if (!this._isInitialized || this._isAborted) {
      return;
    }

    this._isAborted = true;

    if (!this._module) {
      return;
    }

    this._module.setStatus = (text: string) => {
      if (text) {
        console.error(`[JS][wasm][aborted] ${text}`);
      }
    };
  }
};
