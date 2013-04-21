TEMPLATE  = app
TARGET    = ba

qT += core
QT += script
QT += xml

MOC_DIR     = .moc
OBJECTS_DIR = .obj

LIBS = -lrtaudio

QMAKE_CXXFLAGS *= -Wall -ggdb

DEFINES += BA_UNITTEST

INCLUDEPATH += filters \
               generators

DEPENDPATH  += $${INCLUDEPATH}

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
                     bnumbergenerator.h

GENERATORS_SOURCES = bnoisegenerator.cpp \
                     bnumbergenerator.cpp

HEADERS = bapplication.h \
          baudio.h \
          bbuffer.h \
          bevent.h \
          bscriptengine.h \
          bengine.h \
          bmutexlocker.h \
          btimer.h \
          bgenerator.h \
          maximilian.h \
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
          maximilian.cpp \
          $${FILTERS_SOURCES} \
          $${GENERATORS_SOURCES}
