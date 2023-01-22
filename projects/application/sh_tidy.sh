

# cd src

clang-tidy \
  ./src/main.cpp \
  -extra-arg=-std=c++17 \
  -- \
  -I./src \
  -I../libraries/geronimo/src \
  -I../libraries/machine-learning/src \
  -I/usr/include/SDL2/ \
  -DMY_DEFINES \
  ...
