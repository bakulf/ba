#include "beventplay.h"
#include "bapplication.h"
#include "baudio.h"

BEventPlay::BEventPlay(BApplication* aApp)
: BEvent(aApp)
, mApp(aApp)
{
}

BEventPlay::~BEventPlay()
{
}

void
BEventPlay::run()
{
  if (mApp->currentBuffer()->playing()) {
    mApp->currentBuffer()->stop();
  } else {
    mApp->currentBuffer()->play();
  }
}
