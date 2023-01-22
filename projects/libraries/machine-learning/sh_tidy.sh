
clang-tidy \
  ./src/machine-learning/*.cpp \
  -extra-arg=-std=c++17 \
  -- \
  -I$DIR_LIB_GERONIMO/src \
  -I/usr/include/SDL2/ \
  -DMY_DEFINES \
  ...
