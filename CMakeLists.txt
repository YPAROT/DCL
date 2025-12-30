cmake_minimum_required(VERSION 3.16)
project(GenericDCL VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Gui PrintSupport Sql Widgets)

qt_standard_project_setup()

#Chemins vers les fichiers d'en-tête et la bibliothèque qtcsv
set(QTCSV_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/qtcsv/include")
set(QTCSV_DLL_PATH "${CMAKE_SOURCE_DIR}/qtcsv/bin/libqtcsv.dll")
set(QTCSV_LIBRARY "${CMAKE_SOURCE_DIR}/qtcsv/bin/libqtcsv.dll.a")

# Vérifier que les fichiers existent
if(NOT EXISTS "${QTCSV_LIBRARY}")
   message(FATAL_ERROR "Le fichier libqtcsv.dll.a est introuvable dans ${QTCSV_LIBRARY}")
endif()

if(NOT EXISTS "${QTCSV_DLL_PATH}")
   message(FATAL_ERROR "Le fichier libqtcsv.dll est introuvable dans ${QTCSV_DLL_PATH}")
endif()

# Inclure les en-têtes de qtcsv
#include_directories(${QTCSV_INCLUDE_DIR})


qt_add_executable(GenericDCL WIN32 MACOSX_BUNDLE
    dcl_sqlmanager.cpp dcl_sqlmanager.h
    edittabledialog.cpp edittabledialog.h edittabledialog.ui
    edittableviewform.cpp edittableviewform.h edittableviewform.ui
    etiquette.cpp etiquette.h
    exportcsvdialog.cpp exportcsvdialog.h exportcsvdialog.ui
    importcomponentsdialog.cpp importcomponentsdialog.h importcomponentsdialog.ui
    main.cpp
    mainwindow.cpp mainwindow.h mainwindow.ui
    qwidgetsettings.cpp qwidgetsettings.h
    sqltableform.cpp sqltableform.h sqltableform.ui
    tableviewmanager.cpp tableviewmanager.h
)

target_compile_definitions(GenericDCL PRIVATE
    QT_DEPRECATED_WARNINGS
)

target_link_directories(GenericDCL PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/
)

target_link_libraries(GenericDCL PRIVATE
    Qt::Core
    Qt::Gui
    Qt::PrintSupport
    Qt::Sql
    Qt::Widgets
)

# Lier la bibliothèque qtcsv
target_link_libraries(GenericDCL PRIVATE "${QTCSV_LIBRARY}")

# Resources:
set(qressources_resource_files
    "createTables.sql"
)

qt_add_resources(GenericDCL "qressources"
    PREFIX
        "/files"
    FILES
        ${qressources_resource_files}
)
set(qressources1_resource_files
    "Edit Clear.png"
    "File export.png"
    "File import.png"
    "File new.png"
    "Help.png"
    "SaveAs.png"
    "close.png"
    "dclManager.png"
    "etiquettes.png"
    "export2csv.png"
    "logbook.png"
    "open.png"
    "refresh.png"
    "save2.png"
    "settings.png"
    "sql.png"
    "trash.png"
)

qt_add_resources(GenericDCL "qressources1"
    PREFIX
        "/images"
    FILES
        ${qressources1_resource_files}
)

install(TARGETS GenericDCL
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Copier la DLL de qtcsv dans le dossier de build (Windows uniquement)
if(WIN32)
    add_custom_command(TARGET GenericDCL POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            "${QTCSV_DLL_PATH}"
            "$<TARGET_FILE_DIR:GenericDCL>"
        COMMENT "Copie de la DLL qtcsv dans le dossier de build"
    )
endif()

# qt_generate_deploy_app_script(
#     TARGET GenericDCL
#     FILENAME_VARIABLE deploy_script
#     NO_UNSUPPORTED_PLATFORM_ERROR
# )
# install(SCRIPT ${deploy_script})
