#!/bin/bash

# Get the directory of this script
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Check if cmake is available
if command -v cmake &> /dev/null; then
    echo "Building with CMake..."
    
    # Create build directory
    mkdir -p "$SCRIPT_DIR/build"
    cd "$SCRIPT_DIR/build"
    
    # Run CMake
    cmake .. || exit 1
    
    # Build the project
    make || exit 1
    
    # Copy QML file to build directory
    cp "$SCRIPT_DIR/main.qml" "$SCRIPT_DIR/build/" 2>/dev/null || true
    
    echo "Build successful with CMake!"
    echo "Executable is at: $SCRIPT_DIR/build/hyprland-cheatsheet-popup-fredon"
    
else
    echo "Error: CMake not found. Please install cmake."
    exit 1
fi
