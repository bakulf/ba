#ifndef _BA_EVENTSELECTBUFFER_H_
#define _BA_EVENTSELECTBUFFER_H_

#include "bevent.h"
#include "bapplication.h"

class BBuffer;

class BEventSelectBuffer : public BEvent
{
  Q_OBJECT

public:
  BEventSelectBuffer(BApplication* aParent, BBuffer* aBuffer);
  virtual ~BEventSelectBuffer();

  virtual void run();

private:
  BApplication* mApp;
  BBuffer* mBuffer;
};

#endif
