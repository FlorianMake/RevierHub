cmake_minimum_required(VERSION 3.16)
project(RevierHub VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(QT_QML_GENERATE_QMLLS_INI ON)
find_package(Qt6 REQUIRED COMPONENTS Quick Positioning Location)
qt_standard_project_setup(REQUIRES 6.7)
set(QMapLibre_DIR /home/retno/Development/maplibre-qt-prebuilt/maplibre-native-qt_v3.0.0_Qt6.7.3_Android/arm64-v8a/lib/cmake/QMapLibre)
find_package(QMapLibre REQUIRED COMPONENTS Core Location)
if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/importedcontent/CMakeLists.txt")
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/importedcontent)
endif()
qt_add_executable(appRevierHub main.cpp)
qt_add_qml_module(appRevierHub
    URI RevierHub
    QML_FILES Main.qml
    SOURCES getlivelocation.hpp getlivelocation.cpp
    SOURCES
    SOURCES Database/databasemanager.hpp Database/databasemanager.cpp
    SOURCES trailmanager.hpp trailmanager.cpp
)
set_target_properties(appRevierHub PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
    QT_ANDROID_PACKAGE_NAME "com.revierhub.app"
    QT_ANDROID_VERSION_NAME "1.0.0"
    QT_ANDROID_VERSION_CODE 1
    QT_ANDROID_EXTRA_PLUGINS "/home/retno/Development/maplibre-qt-prebuilt/maplibre-native-qt_v3.0.0_Qt6.7.3_Android/arm64-v8a/plugins/geoservices"
    QT_ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android"
    QT_ANDROID_EXTRA_LIBS "/home/retno/Android/Sdk/android_openssl/ssl_3/arm64-v8a/libssl_3.so;/home/retno/Android/Sdk/android_openssl/ssl_3/arm64-v8a/libcrypto_3.so"
)
target_link_libraries(appRevierHub
    PRIVATE Qt6::Quick
    PRIVATE Qt6::Positioning
    PRIVATE Qt6::Location
    PRIVATE QMapLibre::Core
    PRIVATE QMapLibre::Location
)
qmaplibre_location_setup_plugins(appRevierHub)
install(TARGETS appRevierHub
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)