#!/usr/bin/env -S bash ../.port_include.sh

port="cppp"
version="master"
useconfigure="true"
depends=(
    "zlib"
)

workdir="$port"

configure() {
    run make CC="/home/extreme/serenity/Toolchain/Local/x86_64/bin/x86_64-pc-serenity-gcc"
}

build() {
    run make -j$(nproc)
}

install() {
    run_nocd mkdir -p "${SERENITY_INSTALL_ROOT}/usr/local/bin"
    run_nocd mkdir -p "${SERENITY_INSTALL_ROOT}/usr/local/share/man/man1"
    if [ -f "./cppp/cppp" ]; then
        run_nocd cp ./cppp/cppp "${SERENITY_INSTALL_ROOT}/usr/local/bin/."
    else
        echo "Error: $(pwd)cppp binary not found! Aborting installation."
        exit 1
    fi

    if [ -f "./cppp/cppp.1" ]; then
        run_nocd cp ./cppp/cppp.1 "${SERENITY_INSTALL_ROOT}/usr/local/share/man/man1/."
    else
        echo "Warning: cppp.1 man page not found, skipping."
    fi
}
