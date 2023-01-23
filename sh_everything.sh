#!/bin/bash

DIR_ROOT=$PWD

#
#
#
#
#

echo ""
echo "#################################################"
echo "#                                               #"
echo "# IF THIS SCRIPT FAIL -> TRY THOSE TWO COMMANDS #"
echo "# -> 'chmod +x ./sh_everything.sh'              #"
echo "# -> './sh_everything.sh'                       #"
echo "#                                               #"
echo "#################################################"
echo ""

#
#
#
#
#

func_ensure_pkg() {

  PKG_NAME=$1

  echo "=> checking if the package ${PKG_NAME} is installed"

  RESULT=$(dpkg-query -W -f='${Status}' $PKG_NAME 2>/dev/null | grep -c "ok installed")

  case $RESULT in
  0)
    echo "  => the package ${PKG_NAME} is not installed, stopping now"
    exit 1
    ;;
  esac

  echo "  => the package ${PKG_NAME} is installed"
}

func_ensure_pkg libsdl2-dev
func_ensure_pkg libgles2

#
#
#
#
#

if [ -z "${EMSDK}" ]; then
  echo "the env var 'EMSDK' is missing, the web-wasm builds will be skipped"
  echo " => check the readme if you want to install emscripten"
  echo " => it emscripten is laready installed, you may just need to run '. ./emsdk_env.sh' in this terminal"
  WEB_WASM_AVAILABLE=no
else
  echo "the env var 'EMSDK' was found, the web-wasm builds will be included"
  WEB_WASM_AVAILABLE=yes
fi

#
#
#
#
#

echo "ensuring the thirdparties are installed"

sh sh_install_thirdparties.sh not-interactive

#
#
#
#
#

echo "building thirdparties libraries"
echo "  native version"

#
#
#

cd ./thirdparties/dependencies/geronimo

./sh_everything.sh

export DIR_LIB_GERONIMO=$DIR_ROOT/thirdparties/dependencies/geronimo

cd $DIR_ROOT

#
#
#

cd ./thirdparties/dependencies/basic-genetic-algorithm

make build_mode="release" build_platform="native" all -j6

case $WEB_WASM_AVAILABLE in
yes)
  echo "  web-wasm version"
  make build_mode="release" build_platform="web-wasm" all -j6
  ;;
esac

cd $DIR_ROOT

#
#
#
#
#

echo "building projects applicaton"
echo "  native version"
make build_mode="release" build_platform="native-pthread" all -j6

case $WEB_WASM_AVAILABLE in
yes)
  echo "  web-wasm version (webworker)"
  make build_mode="release" build_platform="web-wasm-webworker" all -j6
  echo "  web-wasm version (pthread)"
  make build_mode="release" build_platform="web-wasm-pthread" all -j6
  ;;
esac

#
#
#
#
#

case $WEB_WASM_AVAILABLE in
yes)
  echo "building web-wasm-loader"
  cd ./web-wasm-loader
  npm install
  npm run build
  cd $DIR_ROOT
  ;;
esac

