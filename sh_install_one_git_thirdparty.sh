#!/bin/bash

DIR_DEPENDENCIES=$1
CURR_NAME=$2
CURR_DEST_DIR=$3
CURR_GIT_URL=$4
CURR_GIT_TAG=$5
INTERACTIVE_MODE_ARG=$6

case $INTERACTIVE_MODE_ARG in
not-interactive)
  IS_INTERACTIVE_MODE=off
  ;;
*)
  IS_INTERACTIVE_MODE=on
  ;;
esac

#
# INIT

mkdir -p $DIR_DEPENDENCIES

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

  case $IS_INTERACTIVE_MODE in
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
    echo "### [FATAL ERROR] dependency $CURR_NAME failed to be downloaded"
    echo "###"
    echo ""
    exit 1;
  fi

  ;;

*)
  echo ""
  echo "###"
  echo "### dependency $CURR_NAME was not downloaded"
  echo "###"
  echo ""
  ;;
esac
