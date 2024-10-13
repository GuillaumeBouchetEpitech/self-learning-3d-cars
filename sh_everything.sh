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

func_handle_third_parties() {

  DIR_THIRDPARTIES=$PWD/thirdparties
  DIR_DEPENDENCIES=$DIR_THIRDPARTIES/dependencies

  mkdir -p $DIR_DEPENDENCIES

  #
  #
  #
  #
  #

  func_ensure_wasm_compiler() {

    echo ""
    echo "###"
    echo "###"
    echo "### ensuring the cpp to wasm compiler (emsdk) is installed"
    echo "###"
    echo "###"
    echo ""

    EMSDK_VERSION=3.1.44

    if [ -z "${EMSDK}" ]; then

      echo " -> not installed"
      echo "   -> installing"

      echo "the env var 'EMSDK' is missing, the web-wasm builds will be skipped"
      echo " => check the readme if you want to install emscripten"
      echo " => it emscripten is laready installed, you may just need to run '. ./emsdk_env.sh' in this terminal"


      sh sh_install_one_git_thirdparty.sh \
        $DIR_DEPENDENCIES \
        "EMSDK" \
        "emsdk" \
        "emscripten-core/emsdk" \
        $EMSDK_VERSION \
        "not-interactive"

      cd $DIR_DEPENDENCIES/emsdk

    else

      echo " -> already installed"

      cd $EMSDK
    fi

    echo " -> ensuring the correct version is installed"

    ./emsdk install $EMSDK_VERSION

    echo " -> activating the correct version"

    ./emsdk activate --embedded $EMSDK_VERSION

    . ./emsdk_env.sh

    # em++ --clear-cache

    cd $DIR_ROOT

    echo " -> success"
  }

  func_ensure_wasm_compiler

  #
  #
  #
  #
  #

  func_ensure_thirdparties_presence() {

    echo ""
    echo "###"
    echo "###"
    echo "### ensuring the thirdparties are installed"
    echo "###"
    echo "###"
    echo ""

    sh sh_install_one_git_thirdparty.sh \
      $DIR_DEPENDENCIES \
      "BASIC_GENETIC_ALGORITHM" \
      "basic-genetic-algorithm" \
      "GuillaumeBouchetEpitech/basic-genetic-algorithm" \
      "v0.0.7" \
      "not-interactive"

    sh sh_install_one_git_thirdparty.sh \
      $DIR_DEPENDENCIES \
      "GERONIMO" \
      "geronimo" \
      "GuillaumeBouchetEpitech/geronimo" \
      "v0.0.16" \
      "not-interactive"

    tree -L 1 $DIR_DEPENDENCIES
  }

  func_ensure_thirdparties_presence

  #
  #
  #
  #
  #

  func_ensure_thirdparties_are_built() {

    echo ""
    echo "###"
    echo "###"
    echo "### building thirdparties libraries"
    echo "###"
    echo "###"
    echo ""

    #
    #
    #

    cd "$DIR_DEPENDENCIES/geronimo" || exit 1

    sh sh_everything.sh

    export DIR_LIB_GERONIMO=$DIR_DEPENDENCIES/geronimo

    cd "$DIR_ROOT" || exit 1

    #
    #
    #

    echo "#"
    echo "# native version"
    echo "#"

    cd "$DIR_DEPENDENCIES/basic-genetic-algorithm" || exit 1

    make build_mode="release" build_platform="native" all -j4

    echo "#"
    echo "# web-wasm version"
    echo "#"

    export EMSDK_NUM_CORE=2
    export EMCC_CORES=2

    make build_mode="release" build_platform="web-wasm" all -j4

    cd "$DIR_ROOT" || exit 1

  }

  func_ensure_thirdparties_are_built

}

func_handle_third_parties

#
#
#
#
#

func_build_main_application() {

  echo ""
  echo "###"
  echo "###"
  echo "### building main application"
  echo "###"
  echo "###"
  echo ""

  echo "#"
  echo "# native version"
  echo "#"

  make build_mode="release" build_platform="native-pthread" all -j4

  #
  #
  #

  echo "#"
  echo "# web-wasm version (webworker)"
  echo "#"

  make build_mode="release" build_platform="web-wasm-webworker" all -j4

  #
  #
  #

  echo "#"
  echo "# web-wasm version (pthread)"
  echo "#"

  make build_mode="release" build_platform="web-wasm-pthread" all -j4
}

func_build_main_application

#
#
#
#
#

func_build_wasm_loader_webapp() {

  echo ""
  echo "###"
  echo "###"
  echo "### building web-wasm-loader"
  echo "###"
  echo "###"
  echo ""

  #
  #
  #

  cd "$DIR_ROOT" || exit 1

  if [ -f "./dist/index.js" ]
  then

    echo " ===> 'dist' folder content up to date"
    echo " =====> skipping further checks"

  else

    echo " ===> 'dist' folder content is missing"
    echo " =====> checking"

    #
    #
    #

    cd "$DIR_ROOT/web-wasm-loader" || exit 1

    if [ -f "./js/bundle.js" ]
    then
      echo " =====> up to date bundle.js"
      echo " =======> skip bundling"
    else
      echo " =====> outdated bundle.js"
      echo " =======> bundling"

      #
      #
      #

      if [ -d "./node_modules" ]
      then
        echo " =====> up to date dependencies"
        echo " =======> skip install"
      else
        echo " =====> missing dependencies"
        echo " =======> installing"

        npm install
      fi

      #
      #
      #

      npm run release
    fi

    #
    #
    #

    echo " ===> updating the 'dist' folder"

    npm run update-dist

  fi

  cd "$DIR_ROOT" || exit 1

}

func_build_wasm_loader_webapp
