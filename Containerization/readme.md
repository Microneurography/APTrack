# About containerization

This feature is trying to make building and deploying easier by utilize docker.

## How to use

Currently it's still unfinished. It can get the `plugin-GUI` build, but nothing more. It still can't run the `plugin-GUI` for some audio device reason.

1. Install [docker](https://docs.docker.com/get-docker/).
2. Run `docker build .` inside the directory where the Dockerfile is.
3. It should build the `plugin-GUI` without any error, though it might take a long time.

### `Ubuntu 20.04`

To build and run the project on `Ubuntu 20.04`, in addition to `install_linux_dependencies.sh`, following dependencies will be needed:

* `gcc-8`, `g++-8` (or `clang`)
* `cmake`
* Run `export CC=gcc-8 CXX=g++-8` (or `export CC=clang CXX=clang++`) right before `cmake ..` and `make`

Note: The reason to use `gcc-8` or `clang-10` is that the JUCE version currently used by `plugin-GUI` is not compatible with `gcc-9`. This have already solved by the latest version of `JUCE`, but the update of `plugin-GUI` is still under planning. [See this issue for details](https://github.com/open-ephys/plugin-GUI/issues/329)

## GitHub Actions and CI/CD

This Dockerfile is also used to automatically build the image with GitHub Action `Build image and push` in this repo.

The Action `Build Lfplatency` also uses the Docker image built by `Build image and push`.