
clang-tidy \
  ./src/geronimo/system/*.cpp \
  ./src/geronimo/graphic/*.cpp \
  ./src/geronimo/physic/*.cpp \
  ./src/geronimo/audio/*.cpp \
  -extra-arg=-std=c++17 \
  -- \
  -I./src/ \
  -I/usr/include/SDL2/ \
  -DMY_DEFINES \
  ...
