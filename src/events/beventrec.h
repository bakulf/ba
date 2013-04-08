#ifndef _BA_EVENTREC_H_
#define _BA_EVENTREC_H_

#include "bevent.h"

class BBuffer;
class BApplication;

class BEventRec : public BEvent
{
  Q_OBJECT

public:
  BEventRec(BApplication* aParent);
  virtual ~BEventRec();

  virtual void run();

private:
  BApplication* mApp;
  BBuffer* mBuffer;
};

#endif
