/*
 * Copyright (c) 2021-2022, Liav A. <liavalb@hotmail.co.il>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Types.h>

namespace Kernel::Graphics {

// Note: Address 0x50 is expected to be the DDC2 (EDID) i2c address.
static constexpr u8 ddc2_i2c_address = 0x50;

enum class DisplayRotation {
    Normal = 0,
    Rotate90 = 1,
    Rotate180 = 2,
    Rotate270 = 3
};

enum class FramebufferMode {
    Linear = 0,
    Tiled = 1,
    Compressed = 2
};

enum class PixelFormat {
    BGRx8888 = 0,
    RGBx8888 = 1,
    BGR888 = 2,
    RGB888 = 3,
    BGR565 = 4,
    RGB565 = 5,
    RGBA8888 = 6,
    BGRA8888 = 7
};

struct DisplayMode {
    size_t horizontal_active;
    size_t horizontal_front_porch;
    size_t horizontal_sync_pulse;
    size_t horizontal_blank;
    size_t vertical_active;
    size_t vertical_front_porch;
    size_t vertical_sync_pulse;
    size_t vertical_blank;
    size_t pixel_clock;
    bool interlaced;
    PixelFormat format;
};

}
