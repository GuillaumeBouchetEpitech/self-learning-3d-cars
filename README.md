
# Self Learning 3d Cars

## Table of Contents
- [Self Learning 3d Cars](#self-learning-3d-cars)
  - [Table of Contents](#table-of-contents)
  - [Online Demo](#online-demo)
  - [Diagrams](#diagrams)
- [Dependencies](#dependencies)
  - [Dependency: Emscripten 3.1.26 (for web-wasm build)](#dependency-emscripten-3126-for-web-wasm-build)
  - [Dependency: SDL2, GLESv2 (for native build)](#dependency-sdl2-glesv2-for-native-build)
  - [Dependency: Geronimo 0.0.2](#dependency-geronimo-002)
  - [Dependency: Basic Genetic Algorithm 0.0.1](#dependency-basic-genetic-algorithm-001)
- [How to Build](#how-to-build)
  - [Build Everything (will skip web-wasm if emscripten is absent)](#build-everything-will-skip-web-wasm-if-emscripten-is-absent)
- [How to Run](#how-to-run)
  - [Native Build](#native-build)
  - [Web Wasm Build - without multithreading support: webworkers (mobile friendly)](#web-wasm-build---without-multithreading-support-webworkers-mobile-friendly)
  - [Web Wasm Build - with multithreading support (desktop friendly)](#web-wasm-build---with-multithreading-support-desktop-friendly)
- [Thanks for watching!](#thanks-for-watching)

## Online Demo

**`/!\ important /!\`**

http://guillaumebouchetepitech.github.io/self-learning-3d-cars/dist/index.html

**`/!\ important /!\`**

## Diagrams

![diagram-activity-details.svg](./diagrams/diagram-activity-details.svg "diagram-activity-details.svg")
![diagram-usecase-relationships.svg](./diagrams/diagram-usecase-relationships.svg "diagram-usecase-relationships.svg")
![diagram-activity-threading.svg](./diagrams/diagram-activity-threading.svg "diagram-activity-threading.svg")

# Dependencies

## Dependency: Emscripten 3.1.26 (for web-wasm build)
```bash
git clone https://github.com/emscripten-core/emsdk.git

cd emsdk

./emsdk install 3.1.26
./emsdk activate --embedded 3.1.26

. ./emsdk_env.sh

em++ --clear-cache
```

## Dependency: SDL2, GLESv2 (for native build)
```
libsdl2-dev
libglesv2
```

## Dependency: Geronimo 0.0.2

This dependency will be donwloaded and built with the `Build Everything` method below

## Dependency: Basic Genetic Algorithm 0.0.1

This dependency will be donwloaded and built with the `Build Everything` method below


# How to Build

## Build Everything (will skip web-wasm if emscripten is absent)

```bash
chmod +x ./sh_everything.sh
./sh_everything.sh
# will tell if a dependency is missing
# will skip the web-wasm build if emscripten is not detected
```

# How to Run

## Native Build

```
./bin/exec
```

## Web Wasm Build - without multithreading support: webworkers (mobile friendly)

```bash
node dumbFileServer.js # launch the file server
```

then use **firefox/chrome browser (should support desktop and mobile)** to load `http://127.0.0.1:9000/dist/index.html`

## Web Wasm Build - with multithreading support (desktop friendly)

```bash
node dumbFileServer.js # launch the file server
```

then use **firefox/chrome browser (desktop adviseable)** to load `http://127.0.0.1:9001/dist/index.html`


# Thanks for watching!
