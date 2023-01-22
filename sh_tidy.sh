

# cd src

clang-tidy \
  ./src/main.cpp \
  -extra-arg=-std=c++17 \
  -- \
  -I./src \
  -I$DIR_LIB_GERONIMO/src \
  -I$DIR_LIB_GERONIMO/thirdparties \
  -I/usr/include/SDL2/ \
  -I./thirdparties/dependencies/basic-genetic-algorithm/src \
  -DMY_DEFINES \
  ...
