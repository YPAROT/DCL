QT              += sql widgets
QT              += printsupport

CONFIG += c++11


# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    dcl_sqlmanager.cpp \
    edittabledialog.cpp \
    edittableviewform.cpp \
    etiquette.cpp \
    exportcsvdialog.cpp \
    importcomponentsdialog.cpp \
    main.cpp \
    mainwindow.cpp \
    qwidgetsettings.cpp \
    sqltableform.cpp \
    tableviewmanager.cpp

HEADERS += \
    dcl_sqlmanager.h \
    edittabledialog.h \
    edittableviewform.h \
    etiquette.h \
    exportcsvdialog.h \
    importcomponentsdialog.h \
    mainwindow.h \
    qwidgetsettings.h \
    sqltableform.h \
    tableviewmanager.h

FORMS += \
    edittabledialog.ui \
    edittableviewform.ui \
    exportcsvdialog.ui \
    importcomponentsdialog.ui \
    mainwindow.ui \
    sqltableform.ui

DESTDIR = $$_PRO_FILE_PWD_/bin/

QTCSV_LOCATION = $$DESTDIR
LIBS += -L$$QTCSV_LOCATION -lqtcsv
INCLUDEPATH += $$PWD/../qtcsv-master/include

message(=== Configuration of $$TARGET ===)
message(Qt version: $$[QT_VERSION])
message(binary will be created in folder: $$DESTDIR)

RESOURCES += \
    qressources.qrc

RC_FILE = ressources.rc

