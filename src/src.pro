TEMPLATE  = app
TARGET    = ba

qT += core
QT += script
QT += xml

MOC_DIR     = .moc
OBJECTS_DIR = .obj

LIBS += ../libs/maximilian/libmaximilian.a \
        ../libs/rtaudio/librtaudio.a

QMAKE_CXXFLAGS *= -Wall -ggdb

DEFINES += BA_UNITTEST

INCLUDEPATH += filters \
               generators \
               ../libs/maximilian \
               ../libs/rtaudio

DEPENDPATH  += $${INCLUDEPATH}

include(../audio.pri)

FILTERS_HEADERS = bchorusfilter.h \
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

FILTERS_SOURCES = bchorusfilter.cpp \
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

GENERATORS_HEADERS = bnoisegenerator.h \
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

GENERATORS_SOURCES = bnoisegenerator.cpp \
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

HEADERS = bapplication.h \
          bref.h \
          baudio.h \
          bbuffer.h \
          bevent.h \
          bscriptengine.h \
          bengine.h \
          bmutexlocker.h \
          btimer.h \
          bgenerator.h \
          $${FILTERS_HEADERS} \
          $${GENERATORS_HEADERS}

SOURCES = main.cpp \
          bapplication.cpp \
          baudio.cpp \
          bbuffer.cpp \
          bevent.cpp \
          bscriptengine.cpp \
          bengine.cpp \
          bmutexlocker.cpp \
          btimer.cpp \
          bgenerator.cpp \
          $${FILTERS_SOURCES} \
          $${GENERATORS_SOURCES}
