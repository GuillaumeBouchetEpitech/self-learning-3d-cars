#!/bin/bash

DIR_ROOT=$PWD

cd ./projects/libraries/geronimo
make build_mode="release" build_platform="native" all
make build_mode="release" build_platform="web_wasm" all

cd $DIR_ROOT

cd ./projects/libraries/machine-learning
make build_mode="release" build_platform="native" all
make build_mode="release" build_platform="web_wasm" all

cd $DIR_ROOT

cd ./projects/application
make build_mode="release" build_platform="native_pthread" all
make build_mode="release" build_platform="web_wasm_webworker" all
make build_mode="release" build_platform="web_wasm_pthread" all

cd $DIR_ROOT

cd ./web-wasm-loader
npm install
npm run build
