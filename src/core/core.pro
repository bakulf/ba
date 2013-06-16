TEMPLATE  = app
TARGET    = ba

QT += core
QT += script
QT += xml

MOC_DIR     = .moc
OBJECTS_DIR = .obj

LIBS += ../../libs/maximilian/libmaximilian.a \
        ../../libs/rtaudio/librtaudio.a \
        ../filters/libfilters.a \
        ../generators/libgenerators.a

QMAKE_CXXFLAGS *= -Wall -ggdb

DEFINES += BA_UNITTEST

INCLUDEPATH += ../filters \
               ../generators \
               ../../libs/maximilian \
               ../../libs/rtaudio

DEPENDPATH  += $${INCLUDEPATH}

include(../../audio.pri)

HEADERS = bapplication.h \
          bref.h \
          baudio.h \
          bbuffer.h \
          bevent.h \
          bscriptengine.h \
          bengine.h \
          bmutexlocker.h \
          btimer.h \
          bgenerator.h

SOURCES = main.cpp \
          bapplication.cpp \
          baudio.cpp \
          bbuffer.cpp \
          bevent.cpp \
          bscriptengine.cpp \
          bengine.cpp \
          bmutexlocker.cpp \
          btimer.cpp \
          bgenerator.cpp
