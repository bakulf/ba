TEMPLATE = lib
CONFIG += staticlib

QT += core
QT += script

MOC_DIR     = .moc
OBJECTS_DIR = .obj

QMAKE_CXXFLAGS *= -Wall -ggdb

DEFINES += BA_UNITTEST

INCLUDEPATH += ../core \
               ../../libs/maximilian \
               ../../libs/rtaudio

DEPENDPATH  += $${INCLUDEPATH}

include(../../audio.pri)

HEADERS = bnoisegenerator.h \
          bsinewavegenerator.h \
          bcoswavegenerator.h \
          bphasorgenerator.h \
          bsawgenerator.h \
          btrianglegenerator.h \
          bsquaregenerator.h \
          bpulsegenerator.h \
          bsinebufgenerator.h \
          bsinebuf4generator.h \
          bsawngenerator.h \
          brectgenerator.h \
          bopgenerator.h \
          bnumbergenerator.h

SOURCES = bnoisegenerator.cpp \
          bsinewavegenerator.cpp \
          bcoswavegenerator.cpp \
          bphasorgenerator.cpp \
          bsawgenerator.cpp \
          btrianglegenerator.cpp \
          bsquaregenerator.cpp \
          bpulsegenerator.cpp \
          bsinebufgenerator.cpp \
          bsinebuf4generator.cpp \
          bsawngenerator.cpp \
          brectgenerator.cpp \
          bopgenerator.cpp \
          bnumbergenerator.cpp
