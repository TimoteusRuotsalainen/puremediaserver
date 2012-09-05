TEMPLATE = app
TARGET = PureMediaServer
QT += network script
CONFIG += debug
DESTDIR = ./debug

#win32: LIBS += wsock32.lib
#macx {
#	QMAKE_MAC_SDK=/Developer/SDKs/MacOSX10.4u.sdk
#	#####CONFIG += x86 ppc
#	CONFIG += x86
#}

HEADERS += PureMediaServer.h \
					citp-lib.h \
					PacketCreator.h \
                                        PeerInformationSocket.h \
					MediaServer.h	\				
                                        CITPDefines.h \
                                        MSEXDefines.h

SOURCES += main.cpp \
           PureMediaServer.cpp \
           citp-lib.cpp \
           PacketCreator.cpp \
           PeerInformationSocket.cpp \
           MediaServer.cpp

FORMS += PureMediaServer.ui \

#INCLUDEPATH += ./

LIBS += -L./debug -lcitp

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../debug/release/ -lcitp
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../debug/debug/ -lcitp
#else:symbian: LIBS += -lcitp
#else:unix: LIBS += -L$$PWD/../debug/ -lcitp
#INCLUDEPATH += $$PWD/../debug
#DEPENDPATH += $$PWD/../debug
