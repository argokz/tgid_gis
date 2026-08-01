cmake_minimum_required(VERSION 3.24)

project(tgid_app
    VERSION 0.1.0
    DESCRIPTION "Новый клиент ТГИД для объектной PostGIS-БД"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 6.8 REQUIRED COMPONENTS Core Concurrent Gui Widgets Sql)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(tgid_app
    src/main.cpp
    src/db/DatabaseConfig.cpp
    src/db/DatabaseConfig.h
    src/db/DatabaseConnection.cpp
    src/db/DatabaseConnection.h
    src/repo/ClosedConsumerRepository.cpp
    src/repo/ClosedConsumerRepository.h
    src/repo/ClosedPipeSectionRepository.cpp
    src/repo/ClosedPipeSectionRepository.h
    src/repo/DisconnectedConsumerRepository.cpp
    src/repo/DisconnectedConsumerRepository.h
    src/repo/DisconnectedPipeSectionRepository.cpp
    src/repo/DisconnectedPipeSectionRepository.h
    src/repo/ZeroLoadConsumerRepository.cpp
    src/repo/ZeroLoadConsumerRepository.h
    src/repo/FragmentRepository.cpp
    src/repo/FragmentRepository.h
    src/repo/HeatConsumptionReportRepository.cpp
    src/repo/HeatConsumptionReportRepository.h
    src/repo/LayerCatalogRepository.cpp
    src/repo/LayerCatalogRepository.h
    src/repo/MapRepository.cpp
    src/repo/MapRepository.h
    src/repo/ObjectRepository.cpp
    src/repo/ObjectRepository.h
    src/repo/PipeLengthReportRepository.cpp
    src/repo/PipeLengthReportRepository.h
    src/repo/PipeVolumeReportRepository.cpp
    src/repo/PipeVolumeReportRepository.h
    src/repo/SearchRepository.cpp
    src/repo/SearchRepository.h
    src/ui/MapView.cpp
    src/ui/MapView.h
    src/ui/MainWindow.cpp
    src/ui/MainWindow.h
)

target_include_directories(tgid_app
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(tgid_app
    PRIVATE
        Qt6::Core
        Qt6::Concurrent
        Qt6::Gui
        Qt6::Widgets
        Qt6::Sql
)

target_compile_definitions(tgid_app
    PRIVATE
        TGID_APP_VERSION="${PROJECT_VERSION}"
)

if(MSVC)
    target_compile_options(tgid_app PRIVATE /W4 /permissive- /utf-8)
else()
    target_compile_options(tgid_app PRIVATE -Wall -Wextra -Wpedantic)
endif()

set_target_properties(tgid_app PROPERTIES
    WIN32_EXECUTABLE TRUE
)

install(TARGETS tgid_app
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)
