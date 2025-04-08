# Make hacks
.INTERMEDIATE:

# Set target, configuration, version and destination folders
PLATFORM := serenity
DL_EXT := so
EXESUFFIX:=

# SerenityOS toolchain paths
SERENITY_ROOT := /home/extreme/serenity
SYSROOT := $(SERENITY_ROOT)/Build/x86_64/Root
TOOLCHAIN_BIN := $(SERENITY_ROOT)/Toolchain/Local/x86_64/bin

# Toolchain binaries
NATIVE_CC := $(TOOLCHAIN_BIN)/x86_64-pc-serenity-gcc
CC := $(TOOLCHAIN_BIN)/x86_64-pc-serenity-gcc
CXX := $(TOOLCHAIN_BIN)/x86_64-pc-serenity-g++
AR := $(TOOLCHAIN_BIN)/x86_64-pc-serenity-ar
RANLIB := $(TOOLCHAIN_BIN)/x86_64-pc-serenity-ranlib
STRIP := $(TOOLCHAIN_BIN)/x86_64-pc-serenity-strip
LD := $(TOOLCHAIN_BIN)/x86_64-pc-serenity-gcc

# Compiler flags for SerenityOS
CFLAGS += --sysroot=$(SYSROOT) -I$(SYSROOT)/usr/include
CXXFLAGS += --sysroot=$(SYSROOT) -I$(SYSROOT)/usr/include
LDFLAGS := --sysroot=$(SYSROOT) -L$(SYSROOT)/usr/lib -lSDL2 -lpthread -lm -lc -ldl -flto -lopenal

# C++ compiler flags
CXXFLAGS += -std=c++20 -fcoroutines
CXXFLAGS += $(filter-out -std=gnu11 -Wno-unknown-warning -Wno-unknown-warning-option,$(CFLAGS))

$(OBJ)/SDL/%.o: SDL/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BIN)/sameboy$(EXESUFFIX): $(BINARY_OBJECTS) $(LIBDIR)/libsameboy.a
	$(CXX) $(CXXFLAGS) $(BINARY_OBJECTS) -o $@ -lsameboy $(LDFLAGS) -lGL -lGLU 