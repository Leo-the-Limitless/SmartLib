# cmake_minimum_required(VERSION 3.16)

# project(SmartLib VERSION 0.1 LANGUAGES CXX)

# set(CMAKE_AUTOUIC ON)
# set(CMAKE_AUTOMOC ON)
# set(CMAKE_AUTORCC ON)

# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# # find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
# # find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# # Set the Qt installation prefix
# set(CMAKE_PREFIX_PATH "C:/Qt/6.8.1/mingw_64/lib/cmake")

# # Set Qt_DIR to the directory containing QtConfig.cmake
# set(Qt_DIR "C:/Qt/6.8.1/mingw_64/lib/cmake/Qt6")  # Modify if necessary

# find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)

# set(PROJECT_SOURCES
#         main.cpp
#         mainwindow.cpp
#         mainwindow.h
#         mainwindow.ui
# )

# qt_add_executable(SmartLib
#     MANUAL_FINALIZATION
#     ${PROJECT_SOURCES}
#     DatabaseManager.h
#     DatabaseManager.cpp
#     User.h
#     User.cpp
#     Book.h
#     Book.cpp
#     Transaction.h
#     Transaction.cpp
# )

# # Define target properties for Android with Qt 6 as:
# #    set_property(TARGET SmartLib APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
# #                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# # For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
# # else()
# #     if(ANDROID)
# #         add_library(SmartLib SHARED
# #             ${PROJECT_SOURCES}
# #         )
# # Define properties for Android with Qt 5 after find_package() calls as:
# #    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#     else()
#         add_executable(SmartLib
#             ${PROJECT_SOURCES}
#         )
#     endif()
# endif()

# target_link_libraries(SmartLib PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# # Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# # If you are developing for iOS or macOS you should consider setting an
# # explicit, fixed bundle identifier manually though.
# if(${QT_VERSION} VERSION_LESS 6.1.0)
#   set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.SmartLib)
# endif()
# set_target_properties(SmartLib PROPERTIES
#     ${BUNDLE_ID_OPTION}
#     MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
#     MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
#     MACOSX_BUNDLE TRUE
#     WIN32_EXECUTABLE TRUE
# )

# include(GNUInstallDirs)
# install(TARGETS SmartLib
#     BUNDLE DESTINATION .
#     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
# )

# if(QT_VERSION_MAJOR EQUAL 6)
#     qt_finalize_executable(SmartLib)
# endif()

cmake_minimum_required(VERSION 3.16)

project(SmartLib VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specify the path to Qt installation if not found automatically
set(CMAKE_PREFIX_PATH "C:/Qt/6.8.1/mingw_64/lib/cmake")

# Find Qt6 packages
find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    DatabaseManager.h
    DatabaseManager.cpp
    User.h
    User.cpp
    Book.h
    Book.cpp
    Transaction.h
    Transaction.cpp
)

qt_add_executable(SmartLib
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

# Ensure Qt6::Widgets and Qt6::Sql are linked properly
target_link_libraries(SmartLib PRIVATE Qt6::Widgets Qt6::Sql)

# macOS Bundle settings (optional)
if(APPLE)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.SmartLib)
    set_target_properties(SmartLib PROPERTIES
        ${BUNDLE_ID_OPTION}
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
    )
endif()

# Windows Executable settings
if(WIN32)
    set_target_properties(SmartLib PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

# Installation settings
include(GNUInstallDirs)
install(TARGETS SmartLib
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Finalize Qt executable for Qt6
qt_finalize_executable(SmartLib)
