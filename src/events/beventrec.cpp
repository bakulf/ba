#include "beventrec.h"
#include "bapplication.h"
#include "baudio.h"

BEventRec::BEventRec(BApplication* aApp)
: BEvent(aApp)
, mApp(aApp)
{
}

BEventRec::~BEventRec()
{
}

void
BEventRec::run()
{
  if (!mApp->audio().recording()) {
    mBuffer = mApp->currentBuffer();
    mApp->audio().recStart();
  } else {
    size_t size;
    double* data = mApp->audio().recStop(&size);
    mBuffer->setData(data, size);
  }
}
