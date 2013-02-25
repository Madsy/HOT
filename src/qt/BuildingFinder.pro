TEMPLATE    = app
RESOURCES   = ../../resources/BuildingFinder.qrc
FORMS       = BuildingFinder.ui \
    BuildingFinderCoordinateDialog.ui
QT          += opengl
unix:LIBS += -L/usr/local/lib -lm -lcurl -lopencv_core -lopencv_imgproc -lopencv_highgui
win32:LIBS += X:/modify/me/curl.lib
HEADERS     = QTMain.h ../BingApi.hpp \
    QtMain.h \
    myglwidget.h \
    ../rectDetect.hpp \
    ../BingApi.hpp \
    CoordinateDialog.h
SOURCES     = QtMain.cpp \
    myglwidget.cpp \
    ../rectDetect.cpp \
    CoordinateDialog.cpp
