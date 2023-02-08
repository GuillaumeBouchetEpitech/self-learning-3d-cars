#!/bin/bash

FIRST_ARG=$1

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

DIR_DEPENDENCIES=$PWD/thirdparties/dependencies

declare -a all_dependencies_array=(
  "BASIC_GENETIC_ALGORITHM;basic-genetic-algorithm;GuillaumeBouchetEpitech/basic-genetic-algorithm;v0.0.4"
  "GERONIMO;geronimo;GuillaumeBouchetEpitech/geronimo;v0.0.7"
)

mkdir -p $DIR_DEPENDENCIES

func_clone_repo() {

  IN_ALL_DEP_ARRAY=${*}

  for CURR_DEP_STR in $IN_ALL_DEP_ARRAY;
  do

    while IFS=';' read -ra CURR_SPLIT_STR; do

      CURR_NAME="${CURR_SPLIT_STR[0]}"
      CURR_DEST_DIR="${CURR_SPLIT_STR[1]}"
      CURR_GIT_URL="${CURR_SPLIT_STR[2]}"
      CURR_GIT_TAG="${CURR_SPLIT_STR[3]}"

    done <<< "$CURR_DEP_STR"

    echo ""
    echo "###"
    echo "###"
    echo "### processing dependency $CURR_NAME"
    echo "###"
    echo "###"
    echo ""

    if [ -d "$DIR_DEPENDENCIES/$CURR_DEST_DIR" ]
    then

      echo ""
      echo "###"
      echo "### dependency $CURR_NAME folder was found"
      echo "###"
      echo ""

      case $INTERACTIVE_MODE in
      off)

        echo ""
        echo "###"
        echo "### interacive mode is off -> skipping"
        echo "###"
        echo ""

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

      echo ""
      echo "###"
      echo "### dependency $CURR_NAME is missing -> will install"
      echo "###"
      echo ""

      must_install=yes
    fi



    case $must_install in
    yes)

      echo ""
      echo "###"
      echo "### dependency $CURR_NAME will now be downloaded"
      echo "###"
      echo ""

      cd $DIR_DEPENDENCIES

      # reset
      rm -rf $CURR_DEST_DIR

      # clone (but we only ask for one commit, which is very light)
      git clone --quiet --depth 1 --branch $CURR_GIT_TAG https://github.com/$CURR_GIT_URL $CURR_DEST_DIR

      if [ -d "$DIR_DEPENDENCIES/$CURR_DEST_DIR" ]
      then
        echo ""
        echo "###"
        echo "### dependency $CURR_NAME successfully downloaded"
        echo "###"
        echo ""
      else
        echo ""
        echo "###"
        echo "### dependency $CURR_NAME failed to be downloaded"
        echo "###"
        echo ""
        exit 1;
      fi

      ;;
    esac

  done
}

func_clone_repo ${all_dependencies_array[*]}

tree -L 1 thirdparties/dependencies
