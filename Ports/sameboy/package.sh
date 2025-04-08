#!/usr/bin/env -S bash ../.port_include.sh
port='sameboy'
version='0.15.8'
workdir="SameBoy-${version}"
useconfigure='false'
files=(
    "https://github.com/LIJI32/SameBoy/archive/refs/tags/v${version}.tar.gz"
)
depends=(
    'SDL2'
    'openal'
)

build() {
    # Build the SDL frontend
    run_make_in_build sdl CONF=release \
        CFLAGS="${CFLAGS} -I${SERENITY_INSTALL_ROOT}/usr/include/SDL2 -I." \
        LDFLAGS="${LDFLAGS} -lSDL2 -lopenal" \
        AUDIO_DRIVERS="sdl openal" \
        DATA_DIR="/usr/local/share/sameboy/" \
        SKIP_BOOTROMS=1
}

install() {
    # Create directories
    mkdir -p "${SERENITY_INSTALL_ROOT}/usr/local/bin"
    mkdir -p "${SERENITY_INSTALL_ROOT}/usr/local/share/sameboy"
    mkdir -p "${SERENITY_INSTALL_ROOT}/usr/local/share/applications"

    # Install binary
    cp -f build/bin/SDL/sameboy "${SERENITY_INSTALL_ROOT}/usr/local/bin/"
    chmod +x "${SERENITY_INSTALL_ROOT}/usr/local/bin/sameboy"

    # Install data files
    cp -r build/bin/SDL/Shaders "${SERENITY_INSTALL_ROOT}/usr/local/share/sameboy/"
    cp -r build/bin/SDL/Palettes "${SERENITY_INSTALL_ROOT}/usr/local/share/sameboy/"

    # Create desktop entry
    cat > "${SERENITY_INSTALL_ROOT}/usr/local/share/applications/sameboy.desktop" << EOF
[Desktop Entry]
Type=Application
Name=SameBoy
Comment=Game Boy and Game Boy Color emulator
Exec=/usr/local/bin/sameboy
Icon=sameboy
Categories=Game;Emulator;
EOF
}

post_install() {
    echo "SameBoy has been installed successfully!"
    echo "You can now run it by typing 'sameboy' in the terminal"
    echo "or by finding it in your applications menu."
    echo "Note: Boot ROMs are not included. You will need to provide your own."
}

launcher_name='SameBoy'
launcher_category='Games'
launcher_command='/usr/local/bin/sameboy' 