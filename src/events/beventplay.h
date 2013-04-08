#ifndef _BA_EVENTPLAY_H_
#define _BA_EVENTPLAY_H_

#include "bevent.h"

class BBuffer;
class BApplication;

class BEventPlay : public BEvent
{
  Q_OBJECT

public:
  BEventPlay(BApplication* aParent);
  virtual ~BEventPlay();

  virtual void run();

private:
  BApplication* mApp;
};

#endif
