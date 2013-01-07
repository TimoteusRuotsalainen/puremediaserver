TEMPLATE = app
TARGET = libremediaserver
QT += network script
CONFIG += debug qtestlib
DESTDIR = ./debug

HEADERS += libremediaserver.h \
           citp-lib.h \
           PacketCreator.h \
           PeerInformationSocket.h \
           msex.h	\
           CITPDefines.h \
           MSEXDefines.h

SOURCES += main.cpp \
           libremediaserver.cpp \
           citp-lib.cpp \
           PacketCreator.cpp \
           PeerInformationSocket.cpp \
           msex.cpp

FORMS += \
    libremediaserver.ui

#INCLUDEPATH += ./

LIBS += -L./debug

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../debug/release/ -lcitp
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../debug/debug/ -lcitp
#else:symbian: LIBS += -lcitp
#else:unix: LIBS += -L$$PWD/../debug/ -lcitp
#INCLUDEPATH += $$PWD/../debug
#DEPENDPATH += $$PWD/../debug
