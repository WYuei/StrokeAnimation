#-------------------------------------------------
#
# Project created by QtCreator 2019-07-20T12:39:33
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = StrokeAnimation
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        keybutton.cpp \
        main.cpp \
        mainwindow.cpp \
        scalemark.cpp \
        timebutton.cpp

HEADERS += \
        ControllerStruct.h \
        curve_draft.h \
        geometry_2d.h \
        keybutton.h \
        line_draft.h \
        mainwindow.h \
        my_math.h \
        my_vector_picture.h \
        scalemark.h \
        timebutton.h

FORMS += \
    animationwindow.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target



INCLUDEPATH += F:/openCV3.4.2/opencv/build/include/opencv \
               F:/openCV3.4.2/opencv/build/include/opencv2 \
               F:/openCV3.4.2/opencv/build/include



RESOURCES += \
    Resource/resouceqss.qrc \
    icon.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../openCV3.4.2/opencv/build/x64/vc15/lib/ -lopencv_world342
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../openCV3.4.2/opencv/build/x64/vc15/lib/ -lopencv_world342d
else:unix: LIBS += -L$$PWD/../../../openCV3.4.2/opencv/build/x64/vc15/lib/ -lopencv_world342

INCLUDEPATH += $$PWD/../../../openCV3.4.2/opencv/build/x64/vc15
DEPENDPATH += $$PWD/../../../openCV3.4.2/opencv/build/x64/vc15

DISTFILES += \
    sample08/1.txt
