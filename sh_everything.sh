#!/bin/bash

DIR_ROOT=$PWD

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

if [ -z "${DIR_LIB_GERONIMO}" ]; then
  echo "the env var 'DIR_LIB_GERONIMO' is missing (see the README.md to install/set it)"
  exit 1
fi
echo "the env var 'DIR_LIB_GERONIMO' was found"

#
#
#
#
#

func_ensure_dependencies() {

  ALL_DEPENDENCY_NAMES=${*}

  for dependency_name in $ALL_DEPENDENCY_NAMES;
  do

    echo "check if local dependency is present: $dependency_name"

    if test -f "${dependency_name}"; then
      echo "${dependency_name} was found"
    else
      echo "${dependency_name} was not found, possibly not built"
      exit 1
    fi

  done
}

declare -a all_dependency_names=(
  "${DIR_LIB_GERONIMO}/lib/native/lib-geronimo-system.a"
  "${DIR_LIB_GERONIMO}/lib/native/lib-geronimo-graphic.a"
  "${DIR_LIB_GERONIMO}/lib/native/lib-geronimo-physics.a"
  "${DIR_LIB_GERONIMO}/thirdparties/lib/native/lib-bullet-physics-dynamics.a"
  "${DIR_LIB_GERONIMO}/thirdparties/lib/native/lib-bullet-physics-collision.a"
  "${DIR_LIB_GERONIMO}/thirdparties/lib/native/lib-bullet-physics-linearmath.a"
)

func_ensure_dependencies ${all_dependency_names[*]}

case $WEB_WASM_AVAILABLE in
yes)

  declare -a all_dependency_names=(
    "${DIR_LIB_GERONIMO}/lib/web-wasm/lib-geronimo-system.bc"
    "${DIR_LIB_GERONIMO}/lib/web-wasm/lib-geronimo-graphic.bc"
    "${DIR_LIB_GERONIMO}/lib/web-wasm/lib-geronimo-physics.bc"
    "${DIR_LIB_GERONIMO}/thirdparties/lib/web-wasm/lib-bullet-physics-dynamics.bc"
    "${DIR_LIB_GERONIMO}/thirdparties/lib/web-wasm/lib-bullet-physics-collision.bc"
    "${DIR_LIB_GERONIMO}/thirdparties/lib/web-wasm/lib-bullet-physics-linearmath.bc"
  )

  func_ensure_dependencies ${all_dependency_names[*]}

  ;;
esac

#
#
#
#
#

echo "building projects libraries machine-learning"
echo "  native version"
cd ./projects/libraries/machine-learning
make build_mode="release" build_platform="native" all -j6

case $WEB_WASM_AVAILABLE in
yes)
  echo "  web-wasm version"
  make build_mode="release" build_platform="web_wasm" all -j6
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
cd ./projects/application
make build_mode="release" build_platform="native_pthread" all -j6

case $WEB_WASM_AVAILABLE in
yes)
  echo "  web-wasm version (webworker)"
  make build_mode="release" build_platform="web_wasm_webworker" all -j6
  echo "  web-wasm version (pthread)"
  make build_mode="release" build_platform="web_wasm_pthread" all -j6
  ;;
esac

cd $DIR_ROOT

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

