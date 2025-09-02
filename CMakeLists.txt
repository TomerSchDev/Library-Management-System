cmake_minimum_required(VERSION 3.31)
project(Library_Management_System)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Set up a variable to hold the directory where UI files are located
set(UI_DIR "${CMAKE_SOURCE_DIR}/ui")

# Find Qt6 core modules, including the SQL module
find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        Sql
        REQUIRED
)

# Collect all headers (*.h) from the 'include' subdirectory.
file(GLOB_RECURSE PROJECT_HEADERS ${CMAKE_SOURCE_DIR}/include/*.h)
# Collect all source files (*.cpp) from the 'src' subdirectory.
file(GLOB_RECURSE PROJECT_SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)
# Collect all UI files from the new 'ui' subdirectory.
file(GLOB_RECURSE PROJECT_UIS "${UI_DIR}/*.ui")

# Add executable and source files. Add headers for IDE/project indexing.
add_executable(Library_Management_System
        main.cpp
        ${PROJECT_SOURCES}
        ${PROJECT_HEADERS}
        ${PROJECT_UIS}
)

# Add include/ directory for user headers
target_include_directories(Library_Management_System PRIVATE ${CMAKE_SOURCE_DIR}/include)
# Tell AUTOUIC where to find the UI files
target_include_directories(Library_Management_System PRIVATE ${UI_DIR})

# Link Qt6 libs, including the Sql module
target_link_libraries(Library_Management_System
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Sql
)

# Windows-specific: Copy Qt DLLs and platform plugin after build
if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    # The user's build log indicates the Qt installation path.
    # We will hardcode this path to bypass a common configuration issue.
    # IMPORTANT: If you move your Qt installation, you must update this path.
    set(QT_INSTALL_PATH "C:/Qt1/6.9.2/mingw_64")

    set(DEBUG_SUFFIX)
    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif ()

    # We now directly use the hardcoded path.
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()

    foreach (QT_LIB Core Gui Widgets Sql)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach (QT_LIB)

    # Add command to copy the SQLite database driver to the correct directory
    if (EXISTS "${QT_INSTALL_PATH}/plugins/sqldrivers/qsqlite${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/sqldrivers/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/sqldrivers/qsqlite${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/sqldrivers/")
    endif ()
endif ()