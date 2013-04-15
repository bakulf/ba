#ifndef _BA_APPLICATION_H_
#define _BA_APPLICATION_H_

#include "bevent.h"
#include "baudio.h"
#include "bbuffer.h"

#include <QCoreApplication>
#include <QScriptEngine>
#include <QList>

#include <termios.h>
#include <unistd.h>

class BBuffer;
class BScriptEngine;
class BTimer;

class QDomElement;
class QSocketNotifier;

class BApplication : public QCoreApplication
{
  Q_OBJECT
  Q_DISABLE_COPY(BApplication)

public:
  BApplication(int argc, char** argv);
  virtual ~BApplication();

  bool init();
  int exec();

  BEventManager& eventManager() { return mEventManager; }
  BAudio& audio() { return mAudio; }

  BBuffer* currentBuffer() const { return mCurrentBuffer; }
  QList<BBuffer*>& buffers() { return mBuffers; }

  BScriptEngine* js() const { return mScriptEngine; }

  BTimer* timer() const { return mTimer; }

public: // for events
  void quit();
  void selectBuffer(BBuffer* aBuffer);

private Q_SLOTS:
  void readStdin(int aSocket);
  void refreshScreen();

private:
  bool readConfig();
  bool readConfigAudio(QDomElement& e);
  bool readConfigTerminal(QDomElement& e);
  bool readConfigCode(QDomElement& e);

  bool initAudio();
  void stopAudio();

  bool initTerminal();
  void resetTerminal();

  void writeES(const char* buffer, unsigned int length);
  void writeReal(const char* buffer, unsigned int length);

private:
  QString mConfigFile;

  bool mTerminalInitialized;
  termios mOldTerminalSettings;
  QSocketNotifier* mStdinNotifier;

  bool mEscapeSequence;
  int mEscapeLoop;

  BAudio mAudio;

  QList<BBuffer*> mBuffers;
  BBuffer* mCurrentBuffer;

  BEventManager mEventManager;

  BTimer* mTimer;

  BScriptEngine* mScriptEngine;
};

#endif
