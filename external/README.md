This directory contains `git subtree`s. They were added like this:

```sh
# from the project root
git subtree add --prefix external/libSDL2pp https://github.com/libSDL2pp/libSDL2pp.git master --squash
```

And you can update them like this:

```sh
# from the project root
git subtree pull --prefix external/libSDL2pp https://github.com/libSDL2pp/libSDL2pp.git master --squash
```
