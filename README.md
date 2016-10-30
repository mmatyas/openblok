OpenBlok
========

A customizable, open-source falling block game.

Features:

- [x] Ghost piece support
- [x] Hold piece support
- [x] Next pieces support
- [x] DAS support
- [x] SRS, TGM and Classic rotation
- [x] Full wall- and floor kick support
- [x] 7-piece bag random generator
- [x] Lock delay, line clear delay support
- [x] Sonic drop and Zangi-move support
- [x] Infinity lock support
- [x] T-Spin and Mini T-Spin support
- [ ] ARE support
- [x] Gamepad support (partial)
- [ ] Menu
- [ ] Battle mode


Dependencies
------------

- CMake 3.1+
- C++14 supporting compiler (GCC 4.9 or better)
- SDL2, SDL2_image, SDL2_mixer, SDL2_ttf
- gcov, lcov (only for generating test coverage report)

Eg. on Debian: `sudo apt install cmake libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev`


Build
-----

```sh
mkdir build && cd build
cmake ..
make
cd .. && ./build/src/openblok
```

To run the tests, call `./build/tests/openblok_test` or run the `make coverage` target.
