cmake_minimum_required(VERSION 3.16)
project(DoomsdayPath VERSION 0.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)

qt_standard_project_setup()

qt_add_executable(DoomsdayPath
    src/main.cpp
    src/MainWindow.h
    src/MainWindow.cpp
    src/MainMenuPage.h
    src/MainMenuPage.cpp
    src/NewGamePage.h
    src/NewGamePage.cpp
    src/LoadingPage.h
    src/LoadingPage.cpp
    src/GamePage.h
    src/GamePage.cpp
    src/DifficultyFactor.h
    src/DifficultyFactor.cpp
    src/GameTime.h
    src/GameTime.cpp
    src/PlayerStats.h
    src/PlayerStats.cpp
    src/Action.h
    src/Action.cpp
    src/Item.h
    src/Item.cpp
    src/Inventory.h
    src/Inventory.cpp
    src/BackpackPage.h
    src/BackpackPage.cpp
    src/SettingsPage.h
    src/SettingsPage.cpp
)

target_link_libraries(DoomsdayPath PRIVATE Qt6::Core Qt6::Widgets Qt6::Gui)

if(ANDROID)
    set_target_properties(DoomsdayPath PROPERTIES
        QT_ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android"
    )
else()
    set_target_properties(DoomsdayPath PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
endif()
