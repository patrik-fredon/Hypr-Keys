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

# Install QML file
install(FILES main.qml DESTINATION ${CMAKE_INSTALL_DATADIR}/fredon-modal-cheat)

# Install themes directory
install(DIRECTORY themes/ 
        DESTINATION ${CMAKE_INSTALL_DATADIR}/fredon-modal-cheat/themes
        FILES_MATCHING PATTERN "*.json")

# Install scripts directory
install(DIRECTORY scripts/ 
        DESTINATION ${CMAKE_INSTALL_DATADIR}/fredon-modal-cheat/scripts
        FILES_MATCHING PATTERN "*.py")

# Add a custom target for theme validation
add_custom_target(validate-themes
    COMMAND ${CMAKE_SOURCE_DIR}/scripts/validate-themes.py ${CMAKE_SOURCE_DIR}/themes
    COMMENT "Validating theme files"
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
