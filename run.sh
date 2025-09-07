#!/bin/bash

# Get the directory of this script
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# If built executable exists, run it; otherwise build and run
if [ -f "$SCRIPT_DIR/build/hyprland-cheatsheet-popup-fredon" ]; then
    # Copy QML file to build directory if it doesn't exist there
    if [ ! -f "$SCRIPT_DIR/build/main.qml" ]; then
        cp "$SCRIPT_DIR/main.qml" "$SCRIPT_DIR/build/" 2>/dev/null || true
    fi
    cd "$SCRIPT_DIR/build"
    exec ./hyprland-cheatsheet-popup-fredon
else
    echo "Built executable not found. Building now..."
    "$SCRIPT_DIR/build.sh"
    if [ -f "$SCRIPT_DIR/build/hyprland-cheatsheet-popup-fredon" ]; then
        # Copy QML file to build directory
        cp "$SCRIPT_DIR/main.qml" "$SCRIPT_DIR/build/" 2>/dev/null || true
        cd "$SCRIPT_DIR/build"
        exec ./hyprland-cheatsheet-popup-fredon
    else
        echo "Build failed. Running with qmlscene..."
        exec qmlscene -I "$SCRIPT_DIR" "$SCRIPT_DIR/main.qml"
    fi
fi
