#include "bapplication.h"
#include "baudio.h"
#include "bbuffer.h"
#include "bevent.h"
#include "bscriptengine.h"
#include "btimer.h"
#include "bgenerator.h"
#include "bmutexlocker.h"

#include <QDomElement>
#include <QSocketNotifier>
#include <QStringList>
#include <QTimer>
#include <QFile>

#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdio.h>

BApplication::BApplication(int argc, char** argv)
: QCoreApplication(argc, argv)
, mInputChannels(-1)
, mTerminalInitialized(false)
, mEscapeLoop(0)
, mGeneratorToken(0)
{
  QStringList args = arguments();
  if (args.length() > 1) {
    mConfigFile = args[1];
  }

  mTimer = new BTimer(this);
  mScriptEngine = new BScriptEngine(this);
}

BApplication::~BApplication()
{
  stopAudio();
  resetTerminal();
}

bool
BApplication::init()
{
  return readConfig() && initAudio() && initTerminal();
}

int
BApplication::exec()
{
  // Notification for the keyboard input
  mStdinNotifier = new QSocketNotifier(STDIN_FILENO,
                                       QSocketNotifier::Read,
                                       this);
  connect(mStdinNotifier, SIGNAL(activated(int)),
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
  if (read(STDIN_FILENO, &buf, 1) <= 0) {
    mStdinNotifier->deleteLater();
    mStdinNotifier = NULL;
    return;
  }

  mEventManager.handle(buf);
}

void
BApplication::refreshScreen()
{
  char str[1024];
  int line = 0;
  size_t size;

  BMUTEXLOCKER

  foreach (BBuffer* buffer, mBuffers) {
    // Line: line+1
    size = snprintf(str, sizeof(str), "\033[%d;0f\033[K", ++line);
    writeES(str, size);

    size = snprintf(str, sizeof(str), "Buffer: %s%s\n",
                    buffer == mCurrentBuffer ? " -SELECTED- " : "",
                    buffer->playing() ? " -PLAYING- " : "");
    writeReal(str, size);

    size = snprintf(str, sizeof(str), "\033[%d;0f\033[K", ++line);
    writeES(str, size);
    size = snprintf(str, sizeof(str), "  %s\n", qPrintable(buffer->info()));
    writeReal(str, size);

    size = snprintf(str, sizeof(str), "\033[%d;0f\033[K", ++line);
    writeES(str, size);
    size = snprintf(str, sizeof(str), "  Loop: %3s - Speed: %f - Volume: ",
                    buffer->loop() ? "yes" : "no",
                    buffer->speed()->get());
    writeReal(str, size);

    QString volume = BEngine::writeVolume(buffer->engine());
    writeReal(qPrintable(volume), volume.length());

    QStringList filters = BEngine::writeFilters(buffer->engine());
    foreach (QString filter, filters) {
      size = snprintf(str, sizeof(str), "\033[%d;0f\033[K", ++line);
      writeES(str, size);

      size = snprintf(str, sizeof(str), "  %s", qPrintable(filter));
      writeReal(str, size);
    }

    size = snprintf(str, sizeof(str), "\033[%d;0f\033[K", ++line);
    writeES(str, size);
  }

  // Error messages:
  foreach (const Message& msg, mMessages) {
    size = snprintf(str, sizeof(str), "\033[%d;0f\033[K",  ++line);
    writeES(str, size);

    size = snprintf(str, sizeof(str), "[%s] %s",
                    qPrintable(msg.mTime.toString("hh:mm:ss.zzz")),
                    qPrintable(msg.mMessage));
    writeReal(str, size);
  }

  struct winsize ws;
  memset(&ws, 0, sizeof(winsize));

  QStringList filters = BEngine::writeFilters(mAudio.engine());

  if (mEscapeSequence) {
    ioctl(1, TIOCGWINSZ, &ws);

    for (; line < ws.ws_row - filters.length() - 1;) {
      size = snprintf(str, sizeof(str), "\033[%d;0f\033[K", ++line);
      writeES(str, size);
    }
  }

  foreach (const QString& filter, filters) {
    size = snprintf(str, sizeof(str), "\033[%d;0f\033[K",  ++line);
    writeES(str, size);

    size = snprintf(str, sizeof(str), "%s",  qPrintable(filter));
    writeReal(str, size);
  }

  size = snprintf(str, sizeof(str), "\033[%d;0f\033[K", ++line);
  writeES(str, size);

  size = snprintf(str, sizeof(str), "Volume: ");
  writeReal(str, size);

  QString volume = BEngine::writeVolume(mAudio.engine());
  writeReal(qPrintable(volume), volume.length());

  if (mAudio.recording()) {
    size = snprintf(str, sizeof(str), " - Recording: %s\r",
                    qPrintable(mAudio.recordingStr()));
    writeReal(str, size);
  }

  if (mEscapeSequence) {
    size = snprintf(str, sizeof(str), "\033[%d;%df", ws.ws_row, ws.ws_col);
    writeES(str, size);

    // This is just for fun:
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

  QFile file(mConfigFile);
  if (!file.open(QIODevice::ReadOnly)) {
    std::cerr << "Error: the config file '" << qPrintable(mConfigFile)
              << "' cannot be opened." << std::endl;
    return false;
  }

  QDomDocument dom;
  QString errorMsg;
  int errorLine(0);
  int errorColumn(0);
  if (!dom.setContent(&file, false, &errorMsg, &errorLine, &errorColumn)) {
    std::cerr << "Error: the config file '" << qPrintable(mConfigFile)
              << "' cannot be parsed: " << qPrintable(errorMsg)
              << "(" << errorLine << ":" << errorColumn << ")" << std::endl;
    return false;
  }

  QDomElement root = dom.documentElement();
  if (root.tagName() != "BA") {
    std::cerr << "Error: the config file '"
              << qPrintable(mConfigFile)
              << "' is not valid for this application." << std::endl;
    return false;
  }

  QDomNode n = root.firstChild();
  for (; !n.isNull(); n = n.nextSibling()) {
    QDomElement e = n.toElement();

    if (e.tagName() == "audio") {
      if (!readConfigAudio(e)) {
        return false;
      }
    }

    if (e.tagName() == "terminal") {
      if (!readConfigTerminal(e)) {
        return false;
      }
    }

    else if (e.tagName() == "code") {
      if (!readConfigCode(e)) {
        return false;
      }
    }
  }

  if (mInputChannels <= 0) {
    mInputChannels = maxiSettings::channels;
  }

  if (mInputChannels > maxiSettings::channels) {
    std::cerr << "Error: the config file '"
              << qPrintable(mConfigFile)
              << "' inputChannels cannot not be greater than outputChannels." << std::endl;
  }

  return true;
}

bool
BApplication::readConfigAudio(QDomElement& aElement)
{
  QDomNode n = aElement.firstChild();
  for (; !n.isNull(); n = n.nextSibling()) {
    QDomElement e = n.toElement();

    if (e.tagName() == "inputChannels") {
      mInputChannels = e.text().toInt();
    }

    if (e.tagName() == "outputChannels") {
      maxiSettings::channels = e.text().toInt();
    }

    if (e.tagName() == "sampleRate") {
      maxiSettings::sampleRate = e.text().toInt();
    }

    if (e.tagName() == "bufferSize") {
      maxiSettings::bufferSize = e.text().toInt();
    }

    if (e.tagName() == "inputDevice") {
      mInputDevice = e.text();
    }

    if (e.tagName() == "outputDevice") {
      mOutputDevice = e.text();
    }

    if (e.tagName() == "buffers") {
      int buffers = e.text().toInt();
      for (int i = 0; i < buffers; ++i) {
        BBuffer *buffer = new BBuffer(this);
        mBuffers << buffer;
      }
    }
  }

  if (!mBuffers.length()) {
    std::cerr << "No buffer configured?!?" << std::endl;
    return false;
  }

  mCurrentBuffer = mBuffers[0];
  return true;
}

bool
BApplication::readConfigTerminal(QDomElement& aElement)
{
  QDomNode n = aElement.firstChild();
  for (; !n.isNull(); n = n.nextSibling()) {
    QDomElement e = n.toElement();

    if (e.tagName() == "escapeSequence") {
      mEscapeSequence = e.text() == "true";
    }
  }

  return true;
}

bool
BApplication::readConfigCode(QDomElement& aElement)
{
  QDomNode n = aElement.firstChild();
  for (; !n.isNull(); n = n.nextSibling()) {
    QDomElement e = n.toElement();

    if (e.tagName() == "key") {
      if (!e.hasAttribute("char")) {
        std::cerr << "Any key must have a char attribute." << std::endl;
        return false;
      }

      QString ch = e.attribute("char");
      if (ch.length() != 1) {
        std::cerr << "Any key must have a char attribute." << std::endl;
        return false;
      }

      mEventManager.add(e.attribute("char")[0].toAscii(),
                        new BEvent(this, e.text()));
    }
  }

  return true;
}

bool
BApplication::initAudio()
{
  return mAudio.init(this);
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
    mEscapeSequence = false;
    return true;
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

  mTerminalInitialized = true;
  return true;
}

void
BApplication::resetTerminal()
{
  if (mTerminalInitialized) {
    tcsetattr(STDIN_FILENO, TCSANOW, &mOldTerminalSettings);
  }
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

void
BApplication::registerGenerator(BGenerator* aGenerator)
{
  mGenerators << aGenerator;
}

void
BApplication::updateGenerators()
{
  ++mGeneratorToken;

  foreach (const BGeneratorRef& generator, mGenerators) {
    generator->generate(mGeneratorToken);
  }
}

void
BApplication::printMessage(QString aMsg)
{
  Message msg;
  msg.mMessage = aMsg;
  msg.mTime = QDateTime::currentDateTime();

  mMessages << msg;

  while (mMessages.length() > MAX_ERROR_MESSAGES) {
    mMessages.pop_front();
  }
}
