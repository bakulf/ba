TEMPLATE = lib
CONFIG += staticlib

QT += core
QT += script

MOC_DIR     = .moc
OBJECTS_DIR = .obj

QMAKE_CXXFLAGS *= -Wall -ggdb

DEFINES += BA_UNITTEST

INCLUDEPATH += ../core \
               ../generators \
               ../../libs/maximilian \
               ../../libs/rtaudio

DEPENDPATH  += $${INCLUDEPATH}

include(../../audio.pri)

HEADERS = bchorusfilter.h \
          bloresfilter.h \
          bhiresfilter.h \
          bbandpassfilter.h \
          bhipassfilter.h \
          blopassfilter.h \
          bdelaylinefilter.h \
          bgatefilter.h \
          bcompressorfilter.h \
          bdistortionfilter.h \
          bflangerfilter.h

SOURCES = bchorusfilter.cpp \
          bloresfilter.cpp \
          bhiresfilter.cpp \
          bbandpassfilter.cpp \
          bhipassfilter.cpp \
          blopassfilter.cpp \
          bdelaylinefilter.cpp \
          bgatefilter.cpp \
          bcompressorfilter.cpp \
          bdistortionfilter.cpp \
          bflangerfilter.cpp
