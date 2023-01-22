
# Geronimo

## How to Build

### Build native C++ libraries

```bash
# slow on modest internet connection
sh sh_install_thirdparties.sh

sh sh_build.sh
# this script is interactive:
# -> select platform
#   -> "native"
# -> then mode
#   -> "release"
# -> then rebuild
#   -> "yes"
```

Files expected to be created:
* ./lib/native/
  * lib-bullet-physics-collision.a
  * lib-bullet-physics-dynamics.a
  * lib-bullet-physics-linearmath.a
  * lib-geronimo-audio.a
  * lib-geronimo-graphic.a
  * lib-geronimo-physic.a
  * lib-geronimo-system.a

### Build webassembly (wasm) C++ libraries

```bash
# need emsdk 3.1.26
# => check: sh_install_tools.sh

# slow on modest internet connection
sh sh_install_thirdparties.sh

sh sh_build.sh
# this script is interactive:
# -> select platform
#   -> "web wasm"
# -> then mode
#   -> "release"
# -> then rebuild
#   -> "yes"
```

Files expected to be created:
* ./lib/web-wasm/
  * lib-bullet-physics-collision.bc
  * lib-bullet-physics-dynamics.bc
  * lib-bullet-physics-linearmath.bc
  * lib-geronimo-audio.bc
  * lib-geronimo-graphic.bc
  * lib-geronimo-physic.bc
  * lib-geronimo-system.bc


