#!/bin/sh

set -xe

# Linux Build
CFLAGS="-Wall `pkg-config --cflags raylib`"
LIBS="`pkg-config --libs raylib` -lm -ldl -lpthread"
clang $CFLAGS -o f_visualizer main.c $LIBS

# Windows Cross-Compilation
x86_64-w64-mingw32-gcc -o f_visualizer.exe main.c \
    -I/mnt/c/raylib/include \
    -L/mnt/c/raylib/lib \
    -lraylib -lopengl32 -lgdi32 -lwinmm -mwindows

# Copy Windows Executable to C:\raylib
mv f_visualizer.exe /mnt/c/raylib/

# Run the Executable from Windows
/mnt/c/raylib/f_visualizer.exe
