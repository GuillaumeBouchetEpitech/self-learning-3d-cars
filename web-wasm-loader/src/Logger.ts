
class Logger {

  private _divElement: HTMLDivElement;
  private _lines: string[];
  private _maxLines: number;

  constructor(textAreaElement: HTMLDivElement) {
    this._divElement = textAreaElement;
    this._divElement.innerHTML = ""; // <= clear any browser cache
    this._lines = [];
    this._maxLines = 30;
  }

  log(...args: any[]): void {

    if (args.length == 0)
      return;

    const text = Array.prototype.slice.call(args).join(' ');

    console.log(text);

    this._pushText(text);
  }

  error(...args: any[]): void {

    if (args.length == 0)
      return;

    const text = Array.prototype.slice.call(args).join(' ');

    console.error(text);

    this._pushText(`[ERR] - ${text}`);
  }

  _pushText(text: string): void {

    this._lines.push(text);
    if (this._lines.length > this._maxLines)
      this._lines.splice(0, this._lines.length - this._maxLines);

    this._divElement.innerHTML = this._lines
      .map(line => {

        if (line.indexOf("[JS]") === 0) {
          return `<span style="color: rgb(200,200,150);">${line}</span>`;
        }
        if (line.indexOf("[C++]") === 0) {
          return `<span style="color: rgb(150,150,200);">${line}</span>`;
        }
        return line;
      })
      .join("<br>");

    // force focus on last line
    this._divElement.scrollTop = this._divElement.scrollHeight;
  }

  get size() {
    return this._lines.length;
  }

  peekLast(): string | undefined {
    if (this._lines.length > 0)
      return this._lines[this._lines.length - 1];
    return undefined;
  }

  popLast(): void {
    if (this._lines.length > 0)
      this._lines.splice(this._lines.length - 1, 1);
  }
}

export default Logger;
