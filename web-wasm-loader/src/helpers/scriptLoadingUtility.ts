
export const scriptLoadingUtility = (src: string): Promise<void> => {
  return new Promise((resolve, reject) => {
    const scriptElement = document.createElement("script") as HTMLScriptElement;
    scriptElement.src = src;
    scriptElement.addEventListener('load', () => resolve());
    scriptElement.addEventListener('error', (err) => reject(err));
    document.head.appendChild(scriptElement);
  });
};
