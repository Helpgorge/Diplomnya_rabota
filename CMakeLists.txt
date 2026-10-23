
cmake_minimum_required(VERSION 3.21)

# ============================================================
# Project
# ============================================================

project(plasmonic LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)

# ============================================================
# Dependencies
# ============================================================

# Qt 6
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Network
    Concurrent
)

# OpenCV
find_package(OpenCV REQUIRED COMPONENTS
    core
    imgproc
    imgcodecs
)

# GStreamer
set(GSTREAMER_ROOT "C:/libraries/gstreamer/msvc_x86_64")

find_package(PkgConfig REQUIRED)

set(ENV{PKG_CONFIG_PATH}
    "${GSTREAMER_ROOT}/lib/pkgconfig"
)

pkg_check_modules(GSTREAMER REQUIRED IMPORTED_TARGET
    gstreamer-1.0
    gstreamer-app-1.0
)

# ============================================================
# Executable
# ============================================================

qt_add_executable(plasmonic
    plasmonic/main.cpp

    # Core
    plasmonic/Core/Logger.h
    plasmonic/Core/Logger.cpp

    # Model
    plasmonic/Model/CameraControlModel.h
    plasmonic/Model/CameraControlModel.cpp
    plasmonic/Model/ImageProcessingModel.h
    plasmonic/Model/ImageProcessingModel.cpp

    plasmonic/Model/VideoStreamModel.h
    plasmonic/Model/VideoStreamModel.cpp

    # Presenter
    plasmonic/Presenter/MainPresenter.h
    plasmonic/Presenter/MainPresenter.cpp
    plasmonic/Presenter/VideoPresenter.h
    plasmonic/Presenter/VideoPresenter.cpp

    # View
    plasmonic/View/MainWindow.h
    plasmonic/View/MainWindow.cpp

    plasmonic/View/VideoWindow.h
    plasmonic/View/VideoWindow.cpp
    
    plasmonic/View/ControlCameraWindow.h
    plasmonic/View/ControlCameraWindow.cpp
)

# ============================================================
# Include directories
# ============================================================

target_include_directories(plasmonic PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/plasmonic/Core
    ${CMAKE_CURRENT_SOURCE_DIR}/plasmonic/Model
    ${CMAKE_CURRENT_SOURCE_DIR}/plasmonic/Presenter
    ${CMAKE_CURRENT_SOURCE_DIR}/plasmonic/View

    ${OpenCV_INCLUDE_DIRS}
)

# ============================================================
# Link libraries
# ============================================================

target_link_libraries(plasmonic PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    Qt6::Concurrent

    ${OpenCV_LIBS}

    PkgConfig::GSTREAMER
)

# ============================================================
# Windows configuration
# ============================================================

set_target_properties(plasmonic PROPERTIES
    WIN32_EXECUTABLE TRUE
)

if(WIN32)
    get_target_property(QMAKE_EXECUTABLE
        Qt6::qmake IMPORTED_LOCATION
    )

    get_filename_component(QT_BIN_DIR
        "${QMAKE_EXECUTABLE}" DIRECTORY
    )

    add_custom_command(TARGET plasmonic POST_BUILD
        COMMAND "${QT_BIN_DIR}/windeployqt.exe"
                $<IF:$<CONFIG:Debug>,--debug,--release>
                "$<TARGET_FILE:plasmonic>"
        COMMENT "Deploying Qt libraries"
        VERBATIM
    )
endif()
