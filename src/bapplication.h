#ifndef _BA_APPLICATION_H_
#define _BA_APPLICATION_H_

#include "bevent.h"
#include "baudio.h"
#include "bbuffer.h"

#include <QCoreApplication>
#include <QList>

#include <termios.h>
#include <unistd.h>

class BBuffer;

class BApplication : public QCoreApplication
{
  Q_OBJECT

public:
  BApplication(int argc, char** argv);
  virtual ~BApplication();

  bool init();
  int exec();

  BEventManager& eventManager() { return mEventManager; }
  BAudio& audio() { return mAudio; }
  BBuffer* currentBuffer() { return mCurrentBuffer; }

public: // for events
  void quit();
  void selectBuffer(BBuffer* aBuffer);

private Q_SLOTS:
  void readStdin(int aSocket);
  void refreshScreen();

private:
  bool readConfig();

  bool initAudio();
  void stopAudio();

  bool initTerminal();
  void resetTerminal();

  void writeES(const char* buffer, unsigned int length);
  void writeReal(const char* buffer, unsigned int length);

private:
  QString mConfigFile;
  termios mOldTerminalSettings;

  bool mEscapeSequence;
  int mEscapeLoop;

  BAudio mAudio;

  QList<BBuffer*> mBuffers;
  BBuffer* mCurrentBuffer;

  BEventManager mEventManager;
};

#endif
