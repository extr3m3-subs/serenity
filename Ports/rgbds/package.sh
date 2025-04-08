#!/usr/bin/env -S bash ../.port_include.sh

port="rgbds"
version="0.6.1"
useconfigure="true"
files=(
    "https://github.com/gbdev/rgbds/archive/refs/tags/v${version}.tar.gz#fdc48f5b416fd200598320dec7ffd1207516842771a55a15e5cdd04a243b0d74"
)
depends=(
    "zlib"
)

configure() {
    # Run cmake with SerenityOS's GCC toolchain
    run cmake -S . -B build \
        -DCMAKE_TOOLCHAIN_FILE="${SERENITY_BUILD_DIR}/CMakeToolchain.txt" \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/usr/local
}

build() {
    # Build using SerenityOS's GCC toolchain
    run cmake --build build -- -j$(nproc)  # You can adjust the number of parallel jobs with -j
}

install() {
    # Install binaries to the proper location
    run cmake --install build
}
