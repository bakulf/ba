TEMPLATE  = app
TARGET    = ba

MOC_DIR     = .moc
OBJECTS_DIR = .obj

LIBS = -lrtaudio

QMAKE_CXXFLAGS *= -Wall -ggdb

INCLUDEPATH += events
DEPENDPATH  += $${INCLUDEPATH}

EVENTS_HEADERS = beventquit.h \
                 beventrec.h \
                 beventplay.h \
                 beventselectbuffer.h \

EVENTS_SOURCES = beventquit.cpp \
                 beventrec.cpp \
                 beventplay.cpp \
                 beventselectbuffer.cpp

HEADERS = bapplication.h \
          baudio.h \
          bbuffer.h \
          bevent.h \
          maximilian.h \
          $${EVENTS_HEADERS}

SOURCES = main.cpp \
          bapplication.cpp \
          baudio.cpp \
          bbuffer.cpp \
          bevent.cpp \
          maximilian.cpp \
          $${EVENTS_SOURCES}
