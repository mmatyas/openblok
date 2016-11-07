# Emscripten how to

## 0. Set up Emscripten

Duh.

## 1. Build SDL2_mixer

```sh
cd external
wget https://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-2.0.1.tar.gz
tar xzf SDL2_mixer-2.0.1.tar.gz
ln -s SDL2_mixer-2.0.1 SDL2_mixer
cd SDL2_mixer
mkdir build && cd build
emconfigure ../configure \
  CFLAGS="-O3 -s USE_OGG=1 -s USE_VORBIS=1" \
  --prefix=`pwd`/../installdir \
  --disable-sdltest --enable-shared \
  --enable-music-ogg --disable-music-mp3 \
  --disable-music-flac --disable-music-midi --disable-music-mod
emmake make install
```

## 2. Apply the Emscripten patch and build OpenBlok

```sh
git apply etc/emscripten.patch
mkdir build_js && cd build_js
emconfigure cmake .. -DBUILD_TESTS=OFF -DCMAKE_BUILD_TYPE=Release
emmake make
```

## 3. Link SDL2_mixer to OpenBlok

```sh
em++ -o openblok.html \
  src/openblok.bc \
  ../external/SDL2_mixer/installdir/lib/libSDL2_mixer.so \
  -s USE_SDL=2 \
  -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["png"]' \
  -s USE_SDL_TTF=2 \
  -s USE_OGG=1 -s USE_VORBIS=1 \
  -std=c++14 -O3 \
  --preload-file ../data@data
```
