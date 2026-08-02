cmake_minimum_required(VERSION 3.5)



#option(USE_ITWIN "ITWIN" ON)

#if(USE_ITWIN)
#else()
#option(USE_HASP "Включить HASP" ON)
#endif()

#option(USE_DEMO "Демо-версия для Москвы" ON)

if(USE_ITWIN)
    set(VER_MAJOR 1)
    set(GIDR_ICO    "gidr.ico")

    if(USE_DEMO)
        set(ITWIN_TEPLO "Twin")
        set(ITWIN_TEPLO_RUS "Twin")
        set(COMPANY     "Twin")
        set(VER_COMPANY_NAME  "Twin")
        set(TGID_KZ     "")
        set(VER_COPYRIGHT_STR "\\xa9 2025 Twin")
        set(VER_COPYRIGHT_STR_RUS "© 2025 Twin")
        set(EXE_NAME    "twin.exe")
    else()
        set(ITWIN_TEPLO "ITwin Teplo")
        set(ITWIN_TEPLO_RUS "ITwin Teplo")
#        set(COMPANY     "ITwin")
  
        set(COMPANY     "ETS-Digital")
        
        set(VER_COMPANY_NAME  "ООО \\\"ЭТС-ЦИФРА\\\"")
        set(TGID_KZ     "https://ets-digital.energoanalitika.ru/")
        set(VER_COPYRIGHT_STR "\\xa9 2025 ETS DIGITAL")
        set(VER_COPYRIGHT_STR_RUS " 2025 ETS DIGITAL")
        set(EXE_NAME    "itwin.exe")
    endif()

else()

option(USE_GID9 "Синхоронизация с Demo9" ON)
    
    
    set(VER_MAJOR 2)
    set(ITWIN_TEPLO "Tgid-07")
    set(ITWIN_TEPLO_RUS "ТГИД-07")
    set(COMPANY     "Sirius")
    set(VER_COMPANY_NAME "ПКФ \\\"Сириус\\\"")
    set(TGID_KZ     "http://tgid.kz")
    set(EXE_NAME    "gid8.exe")
    set(GIDR_ICO    "gidr_tgid.ico")
    set(VER_COPYRIGHT_STR "\\xa9 1990-2025 PKF Sirius")
    set(VER_COPYRIGHT_STR_RUS "© 1990-2025 ПКФ Сириус")

endif()


set(VER_MINOR 1)
set(VER_PATCH 13)


set(VER_COMPANY_STR ${COMPANY})
set(VER_PRODUCTNAME_STR ${ITWIN_TEPLO})
set(VER_FILEDESCRIPTION_STR ${ITWIN_TEPLO})

#set( ITWIN_TEPLO)
#VER_FILE_VERSION_STR
#VER_PRODUCT_VERSION_STR


# Дата в формате YYYY.MM.DD
#string(TIMESTAMP BUILD_DATE "%Y.%m.%d")
string(TIMESTAMP BUILD_DATE "%d.%m.%Y")

# Генерация version0.h из шаблона
configure_file(
    ${CMAKE_SOURCE_DIR}/version0.h.in
    ${CMAKE_BINARY_DIR}/version0.h
    @ONLY
)

# Чтобы version0.h находился в include-пути
#include_directories(${CMAKE_BINARY_DIR})


configure_file(
    ${CMAKE_SOURCE_DIR}/gid8.rc.in
    ${CMAKE_BINARY_DIR}/gid8.rc
    @ONLY
)

#if (USE_ITWIN)
#    set(MY_PROJECT_NAME itwin)
#else()
#    set(MY_PROJECT_NAME gid8)
#endif()

#project(${MY_PROJECT_NAME} VERSION 0.1 LANGUAGES CXX)
project(gid8 VERSION 0.1 LANGUAGES CXX)





set(CMAKE_INCLUDE_CURRENT_DIR ON)

#set(LIBRARY_PATH, ${CMAKE_CURRENT_SOURCE_DIR}/hasp/)
#set(LIBRARY_NAME, libhasp_windows.lib)
#link_directories(${LIBRARY_PATH})


set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

IF (!WIN32)
execute_process(COMMAND nproc OUTPUT_VARIABLE NPROC OUTPUT_STRIP_TRAILING_WHITESPACE)
set(CMAKE_BUILD_PARALLEL_LEVEL ${NPROC})
set(CMAKE_CXX_FLAGS "-O2 -march=native -pipe -fuse-ld=lld")
set(CMAKE_C_FLAGS "-O2 -march=native -pipe")
set(CMAKE_EXE_LINKER_FLAGS "-fuse-ld=lld")
set(CMAKE_C_COMPILER_LAUNCHER ccache)
set(CMAKE_CXX_COMPILER_LAUNCHER ccache)
ENDIF()

#set(CMAKE_CXX_FLAGS "/utf-8")
#set(CMAKE_C_FLAGS "/utf-8")


#if(MINGW)
#    add_compile_options(-fstack-protector)
#    add_link_options(-fstack-protector)
#endif()


if (MSVC)
    add_compile_options(/Zc:__cplusplus /permissive- /bigobj)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    add_compile_options(/wd4828)
    add_compile_options(/wd4267)

#    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
#    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

endif()


# Заставляем CMake считать rc всегда устаревшим
set_source_files_properties(
    ${CMAKE_CURRENT_BINARY_DIR}/version0.h
    PROPERTIES OBJECT_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt"
)


IF (WIN32)

#    set_source_files_properties(${CMAKE_BINARY_DIR}/gid8.rc PROPERTIES LANGUAGE RC)


# Заставляем CMake считать rc всегда устаревшим
set_source_files_properties(
    ${CMAKE_CURRENT_BINARY_DIR}/gid8.rc
    PROPERTIES OBJECT_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt"
)

    
    set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_BINARY_DIR}/gid8.rc")

#    if(USE_ITWIN)
#        set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/gid8.rc")
#    else()
#        set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/gid8_tgid.rc")
#    endif()
ENDIF()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools Sql Core Qml
Network PrintSupport Xml 
#HttpServer
)

find_package(Qt6 REQUIRED COMPONENTS Widgets LinguistTools Sql Core
Network PrintSupport Xml 
#HttpServer 
Svg
Qml
#OpcUa
#Core5Compat
#ZLIB
)



#set(BOOST_ROOT "C:/cpp/boost_1_85_0")

#set(Boost_USE_STATIC_LIBS OFF)
#set(Boost_USE_MULTITHREADED ON)
#set(Boost_USE_STATIC_RUNTIME OFF)


#find_package(Boost 1.85.0 REQUIRED geometry) # Укажите необходимые компоненты Boost
#find_package(Boost REQUIRED COMPONENTS geometry) # Укажите необходимые компоненты Boost

# Подключить необходимые компоненты Boost
#target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIRS})
#target_link_libraries(${PROJECT_NAME} PRIVATE ${Boost_LIBRARIES})


IF (WIN32)

IF(DEFINED ENV{BOOST_ROOT})
    set(BOOST_ROOT $ENV{BOOST_ROOT})
else()
    message(FATAL_ERROR  "Set the environment variable BOOST_ROOT")
endif()

IF(IS_DIRECTORY ${BOOST_ROOT})
    include_directories(${BOOST_ROOT})
else()
    message(FATAL_ERROR  "Boost directiory ${BOOST_ROOT} don't exists")
endif()

include_directories(C:/cpp/proj-9.4.1/src)

ENDIF()


#find_package(PkgConfig REQUIRED)
#pkg_check_modules(CAIRO REQUIRED cairo)
#pkg_check_modules(RSVG REQUIRED librsvg-2.0)
#
#include_directories(${CAIRO_INCLUDE_DIRS} ${RSVG_INCLUDE_DIRS})
#link_directories(${CAIRO_LIBRARY_DIRS} ${RSVG_LIBRARY_DIRS})
#
##add_executable(myapp main.cpp)
#target_link_libraries(${PROJECT_NAME} ${CAIRO_LIBRARIES} ${RSVG_LIBRARIES})




set(MINIZIP_DIR ${CMAKE_SOURCE_DIR}/lib/minizip)

set(MINIZIP_SOURCES
    ${MINIZIP_DIR}/zip.c
    ${MINIZIP_DIR}/unzip.c
    ${MINIZIP_DIR}/ioapi.c
)

#include_directories("C:/cpp/boost_1_85_0")

set(QXLSX_PARENTPATH lib/)
set(QXLSX_HEADERPATH lib/QXlsx/header/)
set(QXLSX_SOURCEPATH lib/QXlsx/source/)

include_directories(${QXLSX_HEADERPATH})

file(GLOB QXLSX_CPP "${QXLSX_SOURCEPATH}/*.cpp")
file(GLOB QXLSX_H "${QXLSX_HEADERPATH}/*.h")


qt_add_resources(srcs_for_exe 
resource.qrc
resource2.qrc
resource3.qrc
resource4.qrc
)

qt6_add_resources(srcs_for_exe 
resource.qrc
resource2.qrc
)



set(TS_FILES gid8_ru_RU.ts)

set(PROJECT_SOURCES

    pch.cpp


    boost/graph_boost.cpp 
    boost/proj_coord.cpp
    boost/hausdorff_distance.cpp
    boost/frechet_distance.cpp

    passport/passport.cpp

    gid_data/heatSystem.cpp 
    gid_data/heatSystem.h
    
    http/http.cpp 
    http/mongoose.c 
    http/http_png.cpp

#if (USE_HASP)

#    hasp/hasp.cpp
#    hasp/hasp_errors.cpp

#endif()

    hasp/psw.cpp

    tu/tu.cpp
    tu/TextFilterDialog.cpp
    tu/TuInputDialog.cpp
    tu/TuQDialog.cpp
    tu/ReportInputDialog.cpp
    tu/tu_itog.cpp

    set_line/set_line.cpp 

    alseco/alseco.cpp
    alseco/alsecodlg.cpp
    alseco/alsecodlg.h

    property/potr_fun.cpp
    property/prop_fun.cpp
    property/prop_fun_progr.cpp
    property/prop_fun2.cpp
    property/prop_fun2_progr.cpp
    property/CustomdateEdit.cpp
    property/read_tab_db.cpp
    property/PropertyTree.cpp
    property/variantdelegate.cpp
    property/PropertyDial.cpp
    property/viewTable.cpp 
    property/dialog.cpp
    property/HelpDialog.h

    property/date/CustomDateInput.cpp
    property/date/CustomDateInput.h
    property/date/CalendarPopup.h
        

    NewPassport/NewPassport.cpp 

    word/Remont_docx1.cpp 
    word/OpresTables.cpp
    word/KartaPovrezhdaemosti2.cpp

    remonts/remonts.cpp 
    remonts/opres.cpp 

    ribbon/ribbon.cpp
    ribbon/WToolButton.cpp
    ribbon/ribbonbuttongroup.cpp
    ribbon/ribbontabcontent.cpp

    ui/ParamDialog.h

    ui/Choose_loses_typeDialog.cpp
    ui/Choose_loses_typeDialog.h
    ui/Choose_loses_typeDialog.ui

    ui/Print_frDialog.cpp
    ui/Print_frDialog.h
    ui/Print_frDialog.ui

    ui/Nas_charDialog.cpp
    ui/Nas_charDialog.ui
    ui/Ch_password_right_newDialog.cpp
    ui/Ch_password_right_newDialog.ui
    ui/Ch_passwordDialog.cpp
    ui/Ch_passwordDialog.ui
    ui/Ch_password2Dialog.cpp
    ui/Ch_password2Dialog.ui
    ui2/OpenServer.cpp
    ui2/OpenServer.ui 
    ui/PercentDialog.cpp
    ui/PercentDialog.ui
    ui/Param1Dialog.cpp
    ui/Param1Dialog.ui
    ui/Param2Dialog.cpp
    ui/Param2Dialog.ui
    ui/PotrcxemaDialog.cpp
    ui/PotrcxemaDialog.ui

    ui/KtiDialog.cpp
    ui/KtiDialog.ui
    ui/Nagruz_zdanDialog.cpp
    ui/Nagruz_zdanDialog.ui
    ui/Nagr_treeDialog.cpp
    ui/Nagr_treeDialog.ui

    ui/UtfDialog.cpp
    ui/UtfDialog.ui

    ui2/Login2Dialog.cpp
    ui2/Login2Dialog.ui

    ui2/param_ut.ui
    ui2/param_pt.ui

    ui/inputdialog.cpp

    ui/Klassif_editDialog.cpp
    ui/Klassif_editDialog.ui

    ui/Colors2Dialog.cpp
    ui/Colors2Dialog.ui

#    http/Webserver_dialog2Dialog.cpp
#    http/Webserver_dialog2Dialog.ui

#    zip/qzip.cpp

    config.cpp
    protection.cpp
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainribbon.cpp
    mystd.cpp

    Shell/shell.cpp
    Shell/ShellEdit.cpp

    any/emf_clip.cpp
#    any/svg_emf.cpp

    any/rights.cpp 

    gidview/new_baza.cpp


    gidview/colorLine.cpp
    gidview/colorLine1.cpp
    gidview/drawLabel.cpp
    gidview/excel_list.cpp
    gidview/zaprosy.cpp
    gidview/b5.cpp
    gidview/GidWidget.cpp
    gidview/Scroll.cpp
    gidview/geodraw.cpp
    gidview/gidrAction.cpp
    gidview/gidrMenu.cpp
    gidview/gidrSlot.cpp
    gidview/gidrSlotRemont.cpp
    gidview/gidrToolBar.cpp
    gidview/gidr_calc.cpp
    gidview/gidr_draw.cpp
    gidview/gidr_add.cpp
    gidview/gidr_insert.cpp
    gidview/gidr_del.cpp
    gidview/gidr_find.cpp
    gidview/gidr_rbut.cpp
    gidview/primdraw.cpp
    gidview/colorgtd.cpp
    gidview/info.cpp
    gidview/podp.cpp 
    gidview/viewdb.cpp
    gidview/export.cpp 
    gidview/pts.cpp 
    gidview/draw_defect.cpp
    gidview/analiz.cpp
    gidview/nodeNagr.cpp
    gidview/gidr_info.cpp
    gidview/rpdrawerwidget.cpp
    gidview/sldrawerwidget.cpp
    gidview/styleline.cpp


    table/DbWindow.cpp
    table/QSortFilterSqlQueryModel.cpp
    table/excel.cpp

    db/buildValuesFromCsv.cpp
    db/sort_graph_rs_new.cpp
    db/db.cpp 
    db/insert.cpp
    db/init_db.cpp
    db/backup.cpp
    db/temp.cpp 

    pts/geo_select.cpp

    action/mainAction.cpp 
    action/dbAction.cpp
    action/mainSlot.cpp

    geo/geo_open.cpp 
    geo/geo_file.cpp 
    geo/Klassif.cpp                     
    geo/KlEdit.cpp     
    geo/wkb.cpp
    geo/parseWKB.cpp

    edit/menu.cpp 
    edit/edit.cpp
    edit/editdialog.cpp
    edit/help.cpp
    edit/menu.cpp
    edit/scooterwindow.cpp

    cxema/conv_tgid_qgis.cpp
    cxema/undo_gid.cpp
    cxema/csv.cpp
    cxema/alg.cpp
    cxema/read_nodes.cpp
    cxema/read_lines.cpp
    cxema/graph2.cpp 
    cxema/cxema.cpp
    cxema/cxema1.cpp
    cxema/coordlis.cpp
    cxema/cxema_edit.cpp
    cxema/export_fragment.cpp
    cxema/ado_def.cpp
    cxema/vydel.cpp 

    clip/polygon0.cpp
    clip/polygon1.cpp
    clip/polyio.cpp
    clip/vectmatr.cpp

    geodez/geodez.cpp
    geodez/wgs_kr.cpp

    pjezo/p_excel.cpp
    pjezo/Pjezo.cpp
    pjezo/PjezoAction.cpp
    pjezo/PjezoMenu.cpp

    maps/maps_u.cpp 
    maps/wms.cpp
    maps/wmts.cpp

    tg/tg.cpp
    tg/TGDialog.cpp
    tg/TGraph.cpp
    tg/tempgraph.cpp

    dialog/coordsDial.cpp

    dialog/TableDialog.cpp
    dialog/SliderDialog.cpp
    dialog/Nasos1Dialog.cpp

    dialog/MestSopr.cpp
    dialog/MestSopr2.cpp
    dialog/MestSopr3.cpp
    dialog/NoVisual.cpp
    dialog/DatePopup.cpp
    dialog/geof.cpp
#    dialog/inf.cpp
    dialog/klass.cpp
    dialog/kldial.cpp
    dialog/paramdr.cpp
    dialog/wexample.cpp
    dialog/ChMenuDial.cpp
    dialog/MMenuDial.cpp
    dialog/TableWindow.cpp 
    dialog/OpenServerDialog.cpp
    dialog/MyDialog.cpp
    dialog/PicturesDialog.cpp
    dialog/SaveGeometry.cpp
    dialog/SaveGeometry.h

    dxf/Dxfi.cpp
    dxf/dxfView.cpp
    dxf/Ent.cpp
    dxf/Entities.cpp
    dxf/layer.cpp
    dxf/ltype.cpp
    dxf/minimum.cpp
    dxf/Parser.cpp
    dxf/Token.cpp

    any/setting.cpp 
    any/iconvlite.cpp
    any/reset_shape.cpp
    any/rgb.cpp
    any/download.cpp
    any/download2.cpp
    any/any.cpp 
    any/MyMain.cpp 
    any/password.cpp
    any/md5.cpp

#    lib/qzip.cpp 

    docks/DockSprav.cpp 
    docks/DockPTS.cpp
    docks/DockGeo.cpp
    docks/DockAddr.cpp
    docks/DockGid.cpp

    init/init_pts.cpp
    init/init_files.cpp
    init/init_pics.cpp

#    zip2/zip.cpp


    lib/DuckX/thirdparty/zip/zip.cpp
    lib/DuckX/src/duckx.cpp
    lib/DuckX/thirdparty/pugixml/pugixml.cpp        

    lib/json.cpp     

    ${TS_FILES}

    ${QXLSX_CPP}
    ${QXLSX_H}


)


if (USE_HASP)

list(APPEND PROJECT_SOURCES
    hasp/hasp.cpp
    hasp/hasp_errors.cpp

)
endif()





set(HEADERS
mystd.h
cxema/coordlis.h
cxema/cxema1.h
cxema/Graph.h
cxema/graph2.h

gidview/Scroll.h
gidview/GidWidget.h 
edit/menu.h 

table/DbWindow.h
db/db.h

ribbon/ribbon.h
ribbon/ribbonbuttongroup.h
ribbon/ribbontabcontent.h

gidview/primdrawline.h
gidview/primdrawnode.h
gidview/rpdrawerwidget.h
gidview/sldrawerwidget.h


geo/Klassif.h 
geo/geol.h

ui/OpenServer.h
ui/PercentDialog.h

ui/Password3.h
ui/Password3.ui


lib/DuckX/thirdparty/pugixml/pugiconfig.hpp
lib/DuckX/thirdparty/pugixml/pugixml.hpp
lib/DuckX/thirdparty/zip/miniz.h
lib/DuckX/thirdparty/zip/zip.h

lib/DuckX/src/constants.hpp
lib/DuckX/src/duckx.hpp
lib/DuckX/src/duckxiterator.hpp
  
    )

#target_include_directories(${PROJECT_NAME} PRIVATE
#QtPropertyBrowser/include/
#)

#include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/lib/OpenXLSX)



#------------------------------------------------------------------

#set(QXLSX_PARENTPATH ${CMAKE_CURRENT_SOURCE_DIR}/../lib/)
#set(QXLSX_HEADERPATH ${CMAKE_CURRENT_SOURCE_DIR}/../lib/QXlsx/header/)
#set(QXLSX_SOURCEPATH ${CMAKE_CURRENT_SOURCE_DIR}/../lib/QXlsx/source/)




#target_include_directories(${PROJECT_NAME} PRIVATE
#target_include_directories(${PROJECT_NAME} PRIVATE
# ${QXLSX_HEADERPATH}
# ${CMAKE_CURRENT_SOURCE_DIR}
#)


#------------------------------------------------------------------


    qt_add_executable(${PROJECT_NAME}
        MANUAL_FINALIZATION

#        ${QXLSX_SOURCEPATH}/*.cpp
#        ${QXLSX_HEADERPATH}/*.h
        
        
        ${PROJECT_SOURCES}
#        ${MINIZIP_SOURCES}
        ${SRC_FILES}
         ${srcs_for_exe}
         ${APP_ICON_RESOURCE_WINDOWS}
         ui/typepumpdialog.h ui/typepumpdialog.cpp ui/typepumpdialog.ui
         db/DataRepository.cpp
         pumps/PumpsHelper.cpp
         db/DataRepository.h
         pumps/PumpsHelper.h
         pumps/pumpgraphtemplate.h pumps/pumpgraphtemplate.cpp
         pumps/pumpgraphwidget.h pumps/pumpgraphwidget.cpp
         ui/pumpcomputedialog.h ui/pumpcomputedialog.cpp ui/pumpcomputedialog.ui
         pumps/PumpReport.h

         table/MultiHeaderDbWindow.cpp
         table/ResizableTableView.h table/ResizableTableView.cpp
         table/MultiHeaderTable.h table/MultiHeaderTable.cpp
         table/TeplopoteryHelper.cpp

    )

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})




target_link_libraries(${PROJECT_NAME} 
PRIVATE 
    Qt6::Widgets 
    Qt6::Sql
    Qt6::Core
    Qt6::Network
    Qt6::PrintSupport
    Qt6::Xml
#    Qt6::HttpServer
    Qt6::GuiPrivate
    Qt6::Svg
    Qt6::Qml
##########################
#    Qt::OpcUa

#    Qt6::Zlib

    )




if (USE_HASP)
IF (WIN32)
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/hasp/hasp_windows_x64.lib 
#    ${CMAKE_SOURCE_DIR}/lib/QXlsx/libQXlsx.a
    )
ENDIF()
endif()


#add_subdirectory(QXlsx)
#target_link_libraries(${PROJECT_NAME} PRIVATE QXlsx::QXlsx)


target_precompile_headers(${PROJECT_NAME} PRIVATE pch.h)


set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)


if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()

# Старый интерфейс загружает SQL, описания карточек, классификаторы и
# шаблоны отчётов с диска через argpath(). Держим эти ресурсы отдельно от
# C++-исходников и кладём рядом с exe при каждой сборке.
set(TGID_RUNTIME_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../runtime")
if(NOT EXISTS "${TGID_RUNTIME_DIR}/sql3/us.sql")
    message(FATAL_ERROR "gid8 runtime assets were not found: ${TGID_RUNTIME_DIR}")
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory
        "${TGID_RUNTIME_DIR}"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    COMMENT "Deploying legacy gid8 SQL, classifiers and report templates"
    VERBATIM
)

if(WIN32)
    get_target_property(_qt_qmake_executable Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qt_qmake_executable}" DIRECTORY)

    find_program(TGID_WINDEPLOYQT_EXECUTABLE
        NAMES windeployqt
        HINTS "${_qt_bin_dir}"
        NO_DEFAULT_PATH
        REQUIRED
    )

    set(TGID_POSTGRES_BIN "" CACHE PATH
        "Directory containing libpq.dll and its PostgreSQL runtime dependencies"
    )
    if(NOT TGID_POSTGRES_BIN)
        find_program(TGID_PG_CONFIG_EXECUTABLE
            NAMES pg_config
            HINTS
                "C:/Program Files/PostgreSQL/18/bin"
                "C:/Program Files/PostgreSQL/17/bin"
                "C:/Program Files/PostgreSQL/16/bin"
                "C:/Program Files/PostgreSQL/15/bin"
        )
        if(TGID_PG_CONFIG_EXECUTABLE)
            execute_process(
                COMMAND "${TGID_PG_CONFIG_EXECUTABLE}" --bindir
                OUTPUT_VARIABLE _postgres_bin
                OUTPUT_STRIP_TRAILING_WHITESPACE
                COMMAND_ERROR_IS_FATAL ANY
            )
            set(TGID_POSTGRES_BIN "${_postgres_bin}" CACHE PATH
                "Directory containing libpq.dll and its PostgreSQL runtime dependencies"
                FORCE
            )
        endif()
    endif()

    if(NOT EXISTS "${TGID_POSTGRES_BIN}/libpq.dll")
        message(FATAL_ERROR
            "PostgreSQL libpq.dll was not found. Set TGID_POSTGRES_BIN to the PostgreSQL bin directory."
        )
    endif()

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND powershell.exe -NoProfile -ExecutionPolicy Bypass
            -File "${CMAKE_CURRENT_SOURCE_DIR}/../../tools/deploy_qt_runtime.ps1"
            -App "$<TARGET_FILE:${PROJECT_NAME}>"
            -WindeployQt "${TGID_WINDEPLOYQT_EXECUTABLE}"
            -PostgresBin "${TGID_POSTGRES_BIN}"
            -Configuration "$<CONFIG>"
        COMMENT "Deploying Qt SQL and PostgreSQL runtime for the legacy gid8 UI"
        VERBATIM
    )
endif()




install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ)
install(DIRECTORY lib/QXlsx/header/ DESTINATION include/QXlsx)
install(DIRECTORY lib/QXlsx/source/ DESTINATION src/QXlsx)

target_compile_definitions(${PROJECT_NAME} PRIVATE USE_ITWIN=$<BOOL:${USE_ITWIN}>)
target_compile_definitions(${PROJECT_NAME} PRIVATE USE_HASP=$<BOOL:${USE_HASP}>)
target_compile_definitions(${PROJECT_NAME} PRIVATE USE_GID9=$<BOOL:${USE_GID9}>)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    ITwin_Teplo=\"${ITWIN_TEPLO}\"
    TGID_KZ=\"${TGID_KZ}\"
    GIDR_ICO=\"${GIDR_ICO}\"
)




