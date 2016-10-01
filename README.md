OpenBlok
========

A customizable, open-source falling block game.

Features:

- [x] Ghost piece support
- [x] Hold support
- [x] Next pieces support
- [x] DAS support
- [x] SRS pieces
- [x] 7-piece bag random generator
- [x] Sonic drop and Zangi-move support (optional)
- [x] Lock delay, line clear delay support
- [x] Infinity lock support (optional)
- [x] Wall kick, floor kick support (TGM-based)
- [ ] T-spin support
- [ ] ARE support
- [ ] Gamepad support
- [ ] Menu
- [ ] Battle mode
- [ ] Plugins


Dependencies
------------

- C++14 supporting compiler (GCC 4.9 or better)
- SDL2, SDL2_image, SDL2_ttf
- fontconfig
- gcov, lcov (only if you want to generate test coverage report)

Eg. on Debian: `sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libfontconfig1-dev`


Build
-----

```sh
mkdir build && cd build
cmake ..
make
cd .. && ./build/src/openblok
```

To run the tests, call `./build/tests/openblok_test` or run the `make coverage` target.
