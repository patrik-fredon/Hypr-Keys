cmake_minimum_required(VERSION 3.16)
project(hyprland-cheatsheet-popup-fredon VERSION 1.0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable automatic processing of Qt meta-object system
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find required packages
find_package(Qt6 REQUIRED COMPONENTS Core Quick)

# Create the executable
add_executable(hyprland-cheatsheet-popup-fredon
    main.cpp
    KeybindModel.cpp
    KeybindModel.h
    ThemeManager.cpp
    ThemeManager.h
)

# Link required libraries
target_link_libraries(hyprland-cheatsheet-popup-fredon
    Qt6::Core
    Qt6::Quick
)

# Install the executable
install(TARGETS hyprland-cheatsheet-popup-fredon
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
