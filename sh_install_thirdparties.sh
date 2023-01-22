#!/bin/bash

FIRST_ARG=$1

# echo "FIRST_ARG: ${1}"

case $FIRST_ARG in
not-interactive)
    INTERACTIVE_MODE=off
    ;;
*)
    INTERACTIVE_MODE=on
    ;;
esac

#
# INIT

DIR_DEPENDENCIES=$PWD/thirdparties/dependencies/

BASIC_GENETIC_ALGORITHM_DIR=basic-genetic-algorithm
BASIC_GENETIC_ALGORITHM_URL=GuillaumeBouchetEpitech/basic-genetic-algorithm
BASIC_GENETIC_ALGORITHM_TAG=v0.0.1

mkdir -p $DIR_DEPENDENCIES

clone_repo() {

    CURR_GIT_URL=$1
    CURR_GIT_TAG=$2
    CURR_DEST_DIR=$3

    if [ -d "$DIR_DEPENDENCIES/$CURR_DEST_DIR" ]
    then

        case $INTERACTIVE_MODE in
        off)
            must_install=no
            ;;
        *)
            echo ""
            echo "Directory $DIR_DEPENDENCIES/$CURR_DEST_DIR exists, replace?"
            echo "=> no:  1 (default)"
            echo "=> yes: 2"
            echo ""

            read USER_INPUT_PROJECT

            case $USER_INPUT_PROJECT in
            2)
                echo "replacing"
                echo ""
                must_install=yes
                ;;
            *)
                echo "not replacing"
                echo ""
                must_install=no
                ;;
            esac
            ;;
        esac

    else
        must_install=yes
    fi



    case $must_install in
    yes)
        echo ""
        echo "installing"
        echo ""

        cd $DIR_DEPENDENCIES

        # reset
        rm -rf $CURR_DEST_DIR

        # clone (but we only ask for one commit, which is very light)
        git clone --depth 1 --branch $CURR_GIT_TAG https://github.com/$CURR_GIT_URL $CURR_DEST_DIR
        ;;
    esac
}

clone_repo $BASIC_GENETIC_ALGORITHM_URL $BASIC_GENETIC_ALGORITHM_TAG $BASIC_GENETIC_ALGORITHM_DIR





