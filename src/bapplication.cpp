#include "bapplication.h"
#include "baudio.h"
#include "bbuffer.h"
#include "beventquit.h"
#include "beventrec.h"
#include "beventplay.h"

#include <QSocketNotifier>
#include <QStringList>
#include <QSettings>
#include <QTimer>
#include <QFile>

#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdio.h>

BApplication::BApplication(int argc, char** argv)
: QCoreApplication(argc, argv)
, mEscapeLoop(0)
{
  QStringList args = arguments();
  if (args.length() > 1) {
    mConfigFile = args[1];
  }
}

BApplication::~BApplication()
{
}

bool
BApplication::init()
{
  return readConfig() && initTerminal() && initAudio();
}

int
BApplication::exec()
{
  // Notification for the keyboard input
  QSocketNotifier* notifier = new QSocketNotifier(STDIN_FILENO,
                                                  QSocketNotifier::Read,
                                                  this);
  connect(notifier, SIGNAL(activated(int)),
          this, SLOT(readStdin(int)));

  // Refresh of the screen
  QTimer *timer = new QTimer(this);
  timer->start(100);
  connect(timer, SIGNAL(timeout()),
          this, SLOT(refreshScreen()));

  // Clear screen
  writeES("\033[2J", 4);

  // Event loop
  return QCoreApplication::exec();
}

void
BApplication::quit()
{
  resetTerminal();
  stopAudio();
  QCoreApplication::quit();

  printf("Bye!\n");
}

void
BApplication::selectBuffer(BBuffer* aBuffer)
{
  mCurrentBuffer = aBuffer;
}

void
BApplication::readStdin(int)
{
  char buf;

  // Error reading...
  if (read(STDIN_FILENO, &buf, 1) < 0) {
    quit();
  }

  mEventManager.handle(buf);
}

void
BApplication::refreshScreen()
{
  char str[1024];
  int line = 0;

  foreach (BBuffer* buffer, mBuffers) {
    // Line: line+1
    snprintf(str, sizeof(str), "\033[%d;0f\033[K", ++line);
    writeES(str, strlen(str));

    snprintf(str, sizeof(str), "Buffer - key: %c%s%s", buffer->key().toAscii(),
             buffer == mCurrentBuffer ? " -SELECTED- " : "",
             buffer->playing() ? " -PLAYING- " : "");
    writeReal(str, strlen(str));

    snprintf(str, sizeof(str), "\033[%d;0f\033[K", ++line);
    writeES(str, strlen(str));
    snprintf(str, sizeof(str), "\033[%d;0f\033[K", ++line);
    writeES(str, strlen(str));
    snprintf(str, sizeof(str), "\033[%d;0f\033[K", ++line);
    writeES(str, strlen(str));
    snprintf(str, sizeof(str), "\033[%d;0f\033[K", ++line);
    writeES(str, strlen(str));
  }

  // This is just for fun:
  if (mEscapeSequence) {
    struct winsize ws; 

    ioctl(1, TIOCGWINSZ, &ws);
    snprintf(str, sizeof(str), "\033[%d;0f\033[K", ws.ws_row);
    writeES(str, strlen(str));

    if (mAudio.recording()) {
      snprintf(str, sizeof(str), "Recording: %s", qPrintable(mAudio.recordingStr()));
      writeReal(str, strlen(str));
    }

    snprintf(str, sizeof(str), "\033[%d;%df", ws.ws_row, ws.ws_col);
    writeES(str, strlen(str));

    char buffer[] = "-\\|/";
    if (mEscapeLoop > 3)
      mEscapeLoop = 0;
    writeReal(&buffer[mEscapeLoop++], 1);
  }
}

bool
BApplication::readConfig()
{
  if (mConfigFile.isEmpty() ||
      !QFile::exists(mConfigFile)) {
    std::cerr << "Usage: ba <configFile>" << std::endl;
    return false;
  }

  QSettings settings(mConfigFile, QSettings::IniFormat);

  // Audio Settings
  if (settings.contains("audio/channels")) {
    maxiSettings::channels = settings.value("audio/channels").toInt();
  }

  if (settings.contains("audio/sampleRate")) {
    maxiSettings::sampleRate = settings.value("audio/sampleRate").toInt();
  }

  if (settings.contains("audio/bufferSize")) {
    maxiSettings::bufferSize = settings.value("audio/bufferSize").toInt();
  }

  // Terminal
  mEscapeSequence = settings.value("terminal/escapeSequence").toBool();

  // Global Keys - quit
  if (settings.contains("globalKeys/quit")) {
    QString value = settings.value("globalKeys/quit").toString();
    if (value.length() < 0 || value.length() > 1) {
      std::cerr << "The globalKeys/quit must be 1 single char." << std::endl;
      return false;
    }

    mEventManager.add(value[0], new BEventQuit(this));
  }

  // Global Keys - rec
  if (settings.contains("globalKeys/rec")) {
    QString value = settings.value("globalKeys/rec").toString();
    if (value.length() < 0 || value.length() > 1) {
      std::cerr << "The globalKeys/rec must be 1 single char." << std::endl;
      return false;
    }

    mEventManager.add(value[0], new BEventRec(this));
  }

  if (settings.contains("globalKeys/play")) {
    QString value = settings.value("globalKeys/play").toString();
    if (value.length() < 0 || value.length() > 1) {
      std::cerr << "The globalKeys/play must be 1 single char." << std::endl;
      return false;
    }

    mEventManager.add(value[0], new BEventPlay(this));
  }

  // List of buffers
  QStringList groups = settings.childGroups();
  for (int i = 0; ; ++i) {
    char key[256];
    snprintf(key, sizeof(key), "buffer_%d", i);
    if (!groups.contains(key)) {
      break;
    }

    BBuffer *buffer = BBuffer::create(this, settings, i);
    if (!buffer) {
      return false;
    }

    mBuffers << buffer;
  }

  if (!mBuffers.length()) {
    std::cerr << "No buffer configured?!?" << std::endl;
    return false;
  }

  mCurrentBuffer = mBuffers[0];

  // FIXME: all the fun part
  return true;
}

bool
BApplication::initAudio()
{
  return mAudio.init();
}

void
BApplication::stopAudio()
{
  mAudio.terminate();
}

bool
BApplication::initTerminal()
{
  if (tcgetattr(STDIN_FILENO, &mOldTerminalSettings)) {
    std::cerr << "Error reading the terminal settings." << std::endl;
    return false;
  }

  termios newSettings;
  memcpy(&newSettings, &mOldTerminalSettings, sizeof(termios));
  newSettings.c_lflag &= ~(ICANON | ECHO);
  newSettings.c_cc[VTIME] = 0;
  newSettings.c_cc[VMIN] = 1; 

  if (tcsetattr(STDIN_FILENO, TCSANOW, &newSettings)) {
    std::cerr << "Error changing the terminal settings." << std::endl;
    return false;
  }

  return true;
}

void
BApplication::resetTerminal()
{
  tcsetattr(STDIN_FILENO, TCSANOW, &mOldTerminalSettings);
}

void
BApplication::writeES(const char* buffer, unsigned int length)
{
  if (mEscapeSequence) {
    writeReal(buffer, length);
  }
}

void
BApplication::writeReal(const char* buffer, unsigned int length)
{
  unsigned int done = 0;
  while (done < length) {
    int ret = write(STDOUT_FILENO, buffer + done, length - done);
    if (ret <= 0) {
      break;
    }

    done += ret;
  }
}
