#!/usr/bin/env python3

import json
import sys
import os

def validate_theme_file(file_path):
    """Validate a theme JSON file"""
    required_colors = [
        "primaryColor", "secondaryColor", "backgroundColor",
        "surfaceColor", "textColor", "accentColor", "errorColor"
    ]
    
    try:
        with open(file_path, 'r') as f:
            theme_data = json.load(f)
    except json.JSONDecodeError as e:
        print(f"Error: Invalid JSON in {file_path}: {e}")
        return False
    except Exception as e:
        print(f"Error: Could not read {file_path}: {e}")
        return False
    
    # Check if all required colors are present
    missing_colors = []
    for color in required_colors:
        # Check both formats (with and without "m_" prefix)
        if color not in theme_data and f"m_{color}" not in theme_data:
            missing_colors.append(color)
    
    if missing_colors:
        print(f"Error: Missing colors in {file_path}: {', '.join(missing_colors)}")
        return False
    
    # Validate color values
    invalid_colors = []
    for color_key in required_colors:
        # Get the color value (check both formats)
        color_value = None
        if color_key in theme_data:
            color_value = theme_data[color_key]
        elif f"m_{color_key}" in theme_data:
            color_value = theme_data[f"m_{color_key}"]
        
        if color_value and not is_valid_hex_color(color_value):
            invalid_colors.append(f"{color_key} ({color_value})")
    
    if invalid_colors:
        print(f"Error: Invalid color values in {file_path}: {', '.join(invalid_colors)}")
        return False
    
    print(f"Success: {file_path} is valid")
    return True

def is_valid_hex_color(color):
    """Check if a string is a valid hex color"""
    if not isinstance(color, str):
        return False
    
    # Remove # if present
    if color.startswith('#'):
        color = color[1:]
    
    # Check if it's a valid hex string of length 3, 6, or 8
    if len(color) not in [3, 6, 8]:
        return False
    
    try:
        int(color, 16)
        return True
    except ValueError:
        return False

def main():
    if len(sys.argv) < 2:
        print("Usage: validate-themes.py <theme-file-or-directory>")
        sys.exit(1)
    
    path = sys.argv[1]
    
    if os.path.isfile(path):
        # Validate single file
        if path.endswith('.json'):
            success = validate_theme_file(path)
            sys.exit(0 if success else 1)
        else:
            print(f"Error: {path} is not a JSON file")
            sys.exit(1)
    elif os.path.isdir(path):
        # Validate all JSON files in directory
        failed_files = []
        for file_name in os.listdir(path):
            if file_name.endswith('.json'):
                file_path = os.path.join(path, file_name)
                if not validate_theme_file(file_path):
                    failed_files.append(file_name)
        
        if failed_files:
            print(f"Error: {len(failed_files)} theme files failed validation")
            sys.exit(1)
        else:
            print("Success: All theme files are valid")
            sys.exit(0)
    else:
        print(f"Error: {path} is not a file or directory")
        sys.exit(1)

if __name__ == "__main__":
    main()