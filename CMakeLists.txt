
cmake_minimum_required(VERSION 3.21)

project(plasmonic LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)

# Qt 6
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Network
)

# OpenCV
find_package(OpenCV REQUIRED COMPONENTS
    core
    imgproc
    imgcodecs
)

# Windows GUI application
qt_add_executable(plasmonic
    plasmonic/main.cpp

    plasmonic/Model/TcpClientModel.h
    plasmonic/Model/TcpClientModel.cpp

    plasmonic/Presenter/MainPresenter.h
    plasmonic/Presenter/MainPresenter.cpp

    plasmonic/View/MainWindow.h
    plasmonic/View/MainWindow.cpp
)

target_include_directories(plasmonic PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/plasmonic/Model
    ${CMAKE_CURRENT_SOURCE_DIR}/plasmonic/Presenter
    ${CMAKE_CURRENT_SOURCE_DIR}/plasmonic/View
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(plasmonic PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    ${OpenCV_LIBS}
)

target_include_directories(plasmonic PRIVATE
    ${OpenCV_INCLUDE_DIRS}
)

set_target_properties(plasmonic PROPERTIES
    WIN32_EXECUTABLE TRUE
)

if(WIN32)
    get_target_property(QMAKE_EXECUTABLE Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(QT_BIN_DIR "${QMAKE_EXECUTABLE}" DIRECTORY)

    add_custom_command(TARGET plasmonic POST_BUILD
        COMMAND "${QT_BIN_DIR}/windeployqt.exe"
                $<IF:$<CONFIG:Debug>,--debug,--release>
                "$<TARGET_FILE:plasmonic>"
        COMMENT "Deploying Qt libraries"
        VERBATIM
    )
endif()