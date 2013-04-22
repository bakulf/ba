#ifndef _BA_APPLICATION_H_
#define _BA_APPLICATION_H_

#include "bevent.h"
#include "baudio.h"
#include "bbuffer.h"
#include "bgenerator.h"

#include <QCoreApplication>
#include <QScriptEngine>
#include <QList>
#include <QDateTime>

#include <termios.h>
#include <unistd.h>

#define MAX_ERROR_MESSAGES 5

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
  QString& audioDevice() { return mAudioDevice; }

  BBuffer* currentBuffer() const { return mCurrentBuffer; }
  QList<BBuffer*>& buffers() { return mBuffers; }

  BScriptEngine* js() const { return mScriptEngine; }

  BTimer* timer() const { return mTimer; }

  void printMessage(QString aMsg);

public: // for events
  void quit();
  void selectBuffer(BBuffer* aBuffer);

public: // for generators
  void registerGenerator(BGenerator* aGenerator);
  void updateGenerators();

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

  QString mAudioDevice;

  BAudio mAudio;

  QList<BBuffer*> mBuffers;
  BBuffer* mCurrentBuffer;

  BEventManager mEventManager;

  BTimer* mTimer;

  BScriptEngine* mScriptEngine;

  QList<BGeneratorRef> mGenerators;
  quint64 mGeneratorToken;

  struct Message {
    QDateTime mTime;
    QString mMessage;
  };
  QList<Message> mMessages;
};

#endif
