#ifndef _BA_EVENTQUIT_H_
#define _BA_EVENTQUIT_H_

#include "bevent.h"
#include "bapplication.h"

class BEventQuit : public BEvent
{
  Q_OBJECT

public:
  BEventQuit(BApplication* aParent);
  virtual ~BEventQuit();

  virtual void run();

private:
  BApplication* mApp;
};

#endif
