cmake_minimum_required(VERSION 3.16)
project(DoomsdayPath VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui)

set(SOURCES
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
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
)



if(ANDROID)
    set(CMAKE_ANDROID_API 24)
    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android" CACHE STRING "")
    qt_android_generate_deployment_settings(${PROJECT_NAME})
    qt_android_add_apk_target(${PROJECT_NAME})
endif()
