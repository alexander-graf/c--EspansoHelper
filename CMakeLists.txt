cmake_minimum_required(VERSION 3.16)
project(espanso_helper VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Source files
set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
)

set(HEADERS
    src/mainwindow.h
)

set(RESOURCES
    resources.qrc
)

# Windows resource file for application icon
if(WIN32)
    set(RESOURCES ${RESOURCES} app.rc)
endif()

# Create executable
add_executable(espanso_helper ${SOURCES} ${HEADERS} ${RESOURCES})

# Link Qt6 libraries
target_link_libraries(espanso_helper Qt6::Core Qt6::Widgets)

# Set application icon for Windows
if(WIN32)
    set_target_properties(espanso_helper PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif() 