
import Logger from "./Logger";
import { Application } from "./Application";
import { extractVarsFromUrl } from "./helpers/index";

const findOrFailHtmlElement = <T extends Element>(elementId: string): T => {
  const textAreaElement = document.querySelector<T>(elementId);
  if (!textAreaElement)
    throw new Error(`DOM elements not found, id=${elementId}`);
  return textAreaElement;
}

const onGlobalPageLoad = async () => {

  let isRunning = true;

  const textAreaElement = findOrFailHtmlElement<HTMLTextAreaElement>("#loggerOutput");
  const logger = new Logger(textAreaElement);

  logger.log("[JS] page loaded");

  const onInitialGlobalPageError = (event: ErrorEvent) => {
    isRunning = false;
    logger.error(`[JS] exception, event=${event}`);
  };
  window.addEventListener("error", onInitialGlobalPageError);

  const errorText = findOrFailHtmlElement<HTMLParagraphElement>("#errorText")!;
  const renderArea = findOrFailHtmlElement<HTMLDivElement>("#renderArea")!;
  const mainCanvas = findOrFailHtmlElement<HTMLCanvasElement>("#canvas")!;
  const buttons = {
    switchTo3cpuCores: findOrFailHtmlElement<HTMLButtonElement>("#try_with_3_cpu_cores")!,
    switchTo6cpuCores: findOrFailHtmlElement<HTMLButtonElement>("#try_with_6_cpu_cores")!,
  };

  const hide = (htmlElem: HTMLElement) => {
    if (htmlElem.style.display !== 'none')
      htmlElem.style.display = 'none';
  }
  const show = (htmlElem: HTMLElement) => {
    if (htmlElem.style.display !== 'block')
      htmlElem.style.display = 'block';
  }
  const showErrorText = (htmlText: string) => {
    errorText.innerHTML = htmlText;
    hide(mainCanvas);
    show(errorText);
  };
  const showCanvas = () => {
    hide(errorText);
    show(mainCanvas);
  };
  const resize = (width: number, height: number) => {
    renderArea.style.width = `${width}px`;
    renderArea.style.height = `${height}px`;
    mainCanvas.width = width;
    mainCanvas.height = height;
    mainCanvas.style.width = `${width}px`;
    mainCanvas.style.height = `${height}px`;
  };

  const onNextGlobalPageError = (event: ErrorEvent) => {
    onInitialGlobalPageError(event);
    showErrorText(`fatal error, event=${event}`);
  };
  window.removeEventListener("error", onInitialGlobalPageError);
  window.addEventListener("error", onNextGlobalPageError);

  //
  //
  // configuration's logic

  const getInteger = (text: string, defaultValue: number): number => text ? parseInt(text, 10) : defaultValue;

  const urlVars = extractVarsFromUrl();
  const config: {
    width: number;
    height: number;
    totalGenomes: number;
    totalCores: number;
  } = {
    width: getInteger(urlVars.width, 800),
    height: getInteger(urlVars.height, 600),
    totalGenomes: getInteger(urlVars.totalGenomes, 1000),
    totalCores: getInteger(urlVars.totalCores, 3),
    // initialMemory: getInteger(urlVars.initialMemory, 128),
  };

  resize(config.width, config.height);

  //
  //
  // buttons' logic

  const setupActiveButton = (currButton: HTMLButtonElement, className: string, totalCores: number) => {

    currButton.disabled = false;

    // add active class
    if (!currButton.classList.contains(className))
      currButton.classList.add(className);

    // remove grey class
    if (currButton.classList.contains('grayButton'))
      currButton.classList.remove('grayButton');

    // handle events
    currButton.addEventListener("click", () => {
      // simple reload
      window.location.href = window.location.pathname + `?totalCores=${totalCores}`;
    });
  }

  if (config.totalCores != 3) {
    buttons.switchTo6cpuCores.disabled = true;
    setupActiveButton(buttons.switchTo3cpuCores, 'blueButton', 3);
  }
  else {
    buttons.switchTo3cpuCores.disabled = true;
    setupActiveButton(buttons.switchTo6cpuCores, 'redButton', 6);
  }

  //
  //
  //

  const onProgress = (percent: number) => {
    const statusMsg = `Loading wasm [${percent}%]`;

    // remove the last logged entry if it was a progress updated
    if (logger.size > 0 && logger.peekLast()!.indexOf("Loading wasm [") >= 0)
      logger.popLast();

    logger.log(`[JS] ${statusMsg}`);
    showErrorText(statusMsg);
  };

  const myApplication = new Application(mainCanvas, onProgress, showErrorText);

  try {

    await myApplication.initialize(
      config.width,
      config.height,
      config.totalGenomes,
      config.totalCores,
      logger,
    );

    showCanvas();
  }
  catch (err) {
    logger.error(`[JS] dependencies check failed: message="${err.message}"`);
    showErrorText([
      "this browser isn't compatible",
      "error message:",
      `=> ${err.message}`,
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
