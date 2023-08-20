(() => {
  // src/Logger.ts
  var Logger = class {
    constructor(textAreaElement) {
      this._divElement = textAreaElement;
      this._divElement.innerHTML = "";
      this._lines = [];
      this._maxLines = 30;
    }
    log(...args) {
      if (args.length == 0)
        return;
      const text = Array.prototype.slice.call(args).join(" ");
      console.log(text);
      this._pushText(text);
    }
    error(...args) {
      if (args.length == 0)
        return;
      const text = Array.prototype.slice.call(args).join(" ");
      console.error(text);
      this._pushText("[ERR] - ".concat(text));
    }
    _pushText(text) {
      this._lines.push(text);
      if (this._lines.length > this._maxLines)
        this._lines.splice(0, this._lines.length - this._maxLines);
      this._divElement.innerHTML = this._lines.map((line) => {
        if (/^(\[JS\]).*$/.test(line)) {
          return '<span style="color: rgb(200,200,150);">'.concat(line, "</span>");
        }
        if (/^(\[C\+\+\]).*$/.test(line)) {
          return '<span style="color: rgb(150,150,200);">'.concat(line, "</span>");
        }
        return line;
      }).join("<br>");
      this._divElement.scrollTop = this._divElement.scrollHeight;
    }
    get size() {
      return this._lines.length;
    }
    peekLast() {
      if (this._lines.length > 0)
        return this._lines[this._lines.length - 1];
      return void 0;
    }
    popLast() {
      if (this._lines.length > 0)
        this._lines.splice(this._lines.length - 1, 1);
    }
  };
  var Logger_default = Logger;

  // src/helpers/extractVarsFromUrl.ts
  var urlVarsRegexp = /[?&]+([^=&]+)=([^&]*)/gi;
  var extractVarsFromUrl = () => {
    const urlVars = {};
    let cap = null;
    while (cap = urlVarsRegexp.exec(window.location.href)) {
      const key = cap[1];
      const value = cap[2];
      urlVars[key] = value;
    }
    return urlVars;
  };

  // src/helpers/scriptLoadingUtility.ts
  var scriptLoadingUtility = (src) => {
    return new Promise((resolve, reject) => {
      const scriptElement = document.createElement("script");
      scriptElement.src = src;
      scriptElement.addEventListener("load", () => resolve);
      scriptElement.addEventListener("error", reject);
      document.head.appendChild(scriptElement);
    });
  };

  // src/helpers/getWebGl2Context.ts
  var getWebGl2Context = (inCanvas) => {
    if (!window.WebGL2RenderingContext) {
      throw new Error("missing WebGL2 feature (unsuported)");
    }
    const renderingContextAttribs = {
      // Boolean that indicates if the canvas contains an alpha buffer.
      alpha: false,
      // Boolean that indicates whether or not to perform anti-aliasing.
      antialias: false,
      // Boolean that indicates that the drawing buffer has a depth
      // buffer of at least 16 bits.
      depth: true,
      // Boolean that indicates if a context will be created if the
      // system performance is low.
      failIfMajorPerformanceCaveat: false,
      // A hint to the user agent indicating what configuration of GPU is
      // suitable for the WebGL2 context. Possible values are:
      // "default":
      //     Let the user agent decide which GPU configuration is most
      //     suitable. This is the default value.
      // "high-performance":
      //     Prioritizes rendering performance over power consumption.
      // "low-power":
      //     Prioritizes power saving over rendering performance.
      powerPreference: "high-performance",
      // Boolean that indicates that the page compositor will assume the
      // drawing buffer contains colors with pre-multiplied alpha.
      premultipliedAlpha: true,
      // If the value is true the buffers will not be cleared and will
      // preserve their values until cleared or overwritten by the author.
      preserveDrawingBuffer: true,
      // Boolean that indicates that the drawing buffer has a
      // stencil buffer of at least 8 bits.
      stencil: false
    };
    const webglCtx = inCanvas.getContext("webgl2", renderingContextAttribs);
    if (!webglCtx) {
      const errMsg = "WebGL2 context failed (initialisation)";
      throw new Error(errMsg);
    }
    return webglCtx;
  };

  // src/environment/isWasmSupported.ts
  var isWasmSupported = () => {
    const wasmSupported = (() => {
      try {
        if (typeof WebAssembly === "object" && typeof WebAssembly.instantiate === "function") {
          const module = new WebAssembly.Module(Uint8Array.of(0, 97, 115, 109, 1, 0, 0, 0));
          if (module instanceof WebAssembly.Module)
            return new WebAssembly.Instance(module) instanceof WebAssembly.Instance;
        }
      } catch (err) {
      }
      return false;
    })();
    return wasmSupported;
  };

  // src/environment/isWebWorkerSupported.ts
  var isWebWorkerSupported = () => {
    return !!window.Worker;
  };

  // src/environment/isWebGL2Supported.ts
  var isWebGL2Supported = () => {
    return !!window.WebGL2RenderingContext;
  };

  // src/environment/isMultithreadingSupported.ts
  var isMultithreadingSupported = () => {
    const initialTest = window.SharedArrayBuffer !== void 0;
    if (initialTest) {
      const tmpMemorySize = 8;
      const opts = {
        initial: tmpMemorySize,
        maximum: tmpMemorySize,
        shared: true
      };
      const wasmMemory = new WebAssembly.Memory(opts);
      if (!(wasmMemory.buffer instanceof SharedArrayBuffer))
        return false;
    }
    return initialTest;
  };

  // src/Application.ts
  var Application = class {
    constructor(canvas, onProgress, onError) {
      this._width = 800;
      this._height = 600;
      this._isInitialized = false;
      this._isAborted = false;
      this._canvas = canvas;
      this._onProgress = onProgress;
      this._onError = onError;
    }
    async initialize(width, height, totalGenomes, totalCores, inLogger) {
      this._width = width;
      this._height = height;
      if (!isWebWorkerSupported()) {
        throw new Error("missing WebWorker feature (unsupported)");
      }
      inLogger.log("[JS][check] WebWorker feature => supported");
      if (!isWasmSupported()) {
        throw new Error("missing WebAssembly feature (unsupported)");
      }
      inLogger.log("[JS][check] WebAssembly feature => supported");
      const multithreadingSupported = isMultithreadingSupported();
      if (multithreadingSupported) {
        inLogger.log("[JS][check] multithreading => supported");
      } else {
        inLogger.log("[JS][check] multithreading => NOT supported");
        inLogger.log("[JS][check] => falling back to the webworker version");
      }
      if (!isWebGL2Supported()) {
        throw new Error("missing WebGL2 feature (unsupported)");
      }
      inLogger.log("[JS][check] WebGL2 feature => supported");
      const onContextCreationError = () => {
        this._isAborted = true;
        if (this._onError)
          this._onError("Could not create a WebGL2 context.");
      };
      this._canvas.addEventListener("webglcontextcreationerror", onContextCreationError, false);
      const onWebGlContextLost = () => {
        this._isAborted = true;
        if (this._onError)
          this._onError("WebGL2 context lost. You will need to reload the page.");
      };
      this._canvas.addEventListener("webglcontextlost", onWebGlContextLost, false);
      const onContextMenu = (event) => {
        event.preventDefault();
      };
      this._canvas.addEventListener("contextmenu", onContextMenu, false);
      this._webglCtx = getWebGl2Context(this._canvas);
      inLogger.log("[JS] WebGL2 context initialized");
      const wasmFolder = "wasm/".concat(multithreadingSupported ? "pthread" : "webworker");
      return new Promise((resolve, reject) => {
        const Module = {
          downloadingDataRegExp: /Downloading data\.\.\. \(([0-9]*)\/([0-9]*)\)/,
          lastProgressLevel: 0,
          locateFile: (url) => {
            return "".concat(wasmFolder, "/").concat(url);
          },
          print: (text) => {
            inLogger.log("[C++][out] ".concat(text));
          },
          printErr: (text) => {
            inLogger.error("[C++][err] ".concat(text));
          },
          setStatus: (text) => {
            if (!text)
              return;
            const capture = Module.downloadingDataRegExp.exec(text);
            if (capture) {
              const current = parseFloat(capture[1]);
              const total = parseFloat(capture[2]);
              const percent = Math.floor(current / total * 100);
              if (Module.lastProgressLevel !== percent) {
                Module.lastProgressLevel = percent;
                this._onProgress(percent);
              }
            } else {
              inLogger.log("[JS][wasm][status] ".concat(text));
            }
          },
          onRuntimeInitialized: () => {
            inLogger.log("[JS][wasm] loaded");
            inLogger.log("[JS][wasm] initialising");
            const wasmFunctions = {
              startApplication: window.Module.cwrap("startApplication", void 0, ["number", "number", "number", "number"]),
              updateApplication: window.Module.cwrap("updateApplication", void 0, ["number"]),
              renderApplication: window.Module.cwrap("renderApplication", void 0, [])
            };
            this._wasmApplicationUpdateFunc = wasmFunctions.updateApplication;
            this._wasmApplicationRenderFunc = wasmFunctions.renderApplication;
            wasmFunctions.startApplication(this._width, this._height, totalGenomes, totalCores);
            this._isInitialized = true;
            inLogger.log("[JS][wasm] initialized");
            resolve();
          },
          canvas: this._canvas,
          preinitializedWebGLContext: this._webglCtx,
          noInitialRun: true,
          noExitRuntime: true
        };
        window.Module = Module;
        inLogger.log("[JS][wasm] loading");
        scriptLoadingUtility("./".concat(wasmFolder, "/index.js")).catch(reject);
      });
    }
    update(deltaTime) {
      if (!this._isInitialized || this._isAborted)
        return;
      if (this._wasmApplicationUpdateFunc)
        this._wasmApplicationUpdateFunc(deltaTime);
    }
    render() {
      if (!this._isInitialized || this._isAborted)
        return;
      if (this._wasmApplicationRenderFunc)
        this._wasmApplicationRenderFunc();
    }
    abort() {
      if (!this._isInitialized || this._isAborted)
        return;
      this._isAborted = true;
      const currModule = window.Module;
      if (currModule) {
        currModule.setStatus = (text) => {
          if (text)
            console.error("[JS][wasm][aborted] ".concat(text));
        };
      }
    }
  };

  // src/main.ts
  var findOrFailHtmlElement = (elementId) => {
    const textAreaElement = document.querySelector(elementId);
    if (!textAreaElement)
      throw new Error("DOM elements not found, id=".concat(elementId));
    return textAreaElement;
  };
  var onGlobalPageLoad = async () => {
    let isRunning = true;
    const loggerOutputElement = findOrFailHtmlElement("#loggerOutput");
    const logger = new Logger_default(loggerOutputElement);
    logger.log("[JS] page loaded");
    const onInitialGlobalPageError = (event) => {
      isRunning = false;
      logger.error("[JS] exception, message=".concat(event.message));
    };
    window.addEventListener("error", onInitialGlobalPageError);
    const errorText = findOrFailHtmlElement("#errorText");
    const renderArea = findOrFailHtmlElement("#renderArea");
    const mainCanvas = findOrFailHtmlElement("#canvas");
    const buttons = {
      switchTo3cpuCores: findOrFailHtmlElement("#try_with_3_cpu_cores"),
      switchTo6cpuCores: findOrFailHtmlElement("#try_with_6_cpu_cores")
    };
    const hide = (htmlElem) => {
      if (htmlElem.style.display !== "none")
        htmlElem.style.display = "none";
    };
    const show = (htmlElem) => {
      if (htmlElem.style.display !== "block")
        htmlElem.style.display = "block";
    };
    const showErrorText = (htmlText) => {
      errorText.innerHTML = htmlText;
      hide(mainCanvas);
      show(errorText);
    };
    const showCanvas = () => {
      hide(errorText);
      show(mainCanvas);
    };
    const resize = (width, height) => {
      renderArea.style.width = "".concat(width, "px");
      renderArea.style.height = "".concat(height, "px");
      mainCanvas.width = width;
      mainCanvas.height = height;
      mainCanvas.style.width = "".concat(width, "px");
      mainCanvas.style.height = "".concat(height, "px");
    };
    const onNextGlobalPageError = (event) => {
      onInitialGlobalPageError(event);
      showErrorText("fatal error, event=".concat(event));
    };
    window.removeEventListener("error", onInitialGlobalPageError);
    window.addEventListener("error", onNextGlobalPageError);
    const getInteger = (text, defaultValue) => text ? parseInt(text, 10) : defaultValue;
    const urlVars = extractVarsFromUrl();
    const config = {
      width: getInteger(urlVars.width, 800),
      height: getInteger(urlVars.height, 600),
      totalGenomes: getInteger(urlVars.totalGenomes, 1e3),
      totalCores: getInteger(urlVars.totalCores, 3)
      // initialMemory: getInteger(urlVars.initialMemory, 128),
    };
    resize(config.width, config.height);
    const setupActiveButton = (currButton, className, totalCores) => {
      currButton.disabled = false;
      if (!currButton.classList.contains(className))
        currButton.classList.add(className);
      if (currButton.classList.contains("grayButton"))
        currButton.classList.remove("grayButton");
      currButton.addEventListener("click", () => {
        window.location.href = window.location.pathname + "?totalCores=".concat(totalCores);
      });
    };
    if (config.totalCores != 3) {
      buttons.switchTo6cpuCores.disabled = true;
      setupActiveButton(buttons.switchTo3cpuCores, "blueButton", 3);
    } else {
      buttons.switchTo3cpuCores.disabled = true;
      setupActiveButton(buttons.switchTo6cpuCores, "redButton", 6);
    }
    const onProgress = (percent) => {
      const statusMsg = "Loading wasm [".concat(percent, "%]");
      if (logger.size > 0 && logger.peekLast().indexOf("Loading wasm [") >= 0)
        logger.popLast();
      logger.log("[JS] ".concat(statusMsg));
      showErrorText(statusMsg);
    };
    const myApplication = new Application(mainCanvas, onProgress, showErrorText);
    try {
      await myApplication.initialize(
        config.width,
        config.height,
        config.totalGenomes,
        config.totalCores,
        logger
      );
      showCanvas();
    } catch (err) {
      logger.error('[JS] dependencies check failed: message="'.concat(err.message, '"'));
      showErrorText([
        "this browser isn't compatible",
        "error message:",
        "=> ".concat(err.message)
      ].join("<br>"));
    }
    let previousTime = Date.now();
    const onFrame = () => {
      if (isRunning)
        requestAnimationFrame(onFrame);
      const currTime = Date.now();
      const deltaTime = currTime - previousTime;
      previousTime = currTime;
      myApplication.update(deltaTime);
      myApplication.render();
    };
    onFrame();
  };
  window.addEventListener("load", onGlobalPageLoad);
})();
