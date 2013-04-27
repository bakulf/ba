CONFIG += alsa

contains(CONFIG, pulse) {
  DEFINES += __LINUX_PULSE__
  LIBS += -lpulse-simple -lpulse
}

contains(CONFIG, oss) {
  DEFINES += __LINUX_OSS__
}

contains(CONFIG, jack) {
  DEFINES += __UNIX_JACK__
  LIBS += `pkg-config --cflags --libs jack`
}

contains(CONFIG, alsa) {
  DEFINES += __LINUX_ALSA__
  LIBS += -lasound
}

mac {
  DEFINES += __MACOSX_CORE__
  LIBS += -framework CoreAudio
}

contains(CONFIG, win_ds) {
  DEFINES += __WINDOWS_DS__
}

contains(CONFIG, win_asio) {
  DEFINES += __WINDOWS_ASIO__
}
