#include "beventquit.h"

BEventQuit::BEventQuit(BApplication* aApp)
: BEvent(aApp)
, mApp(aApp)
{
}

BEventQuit::~BEventQuit()
{
}

void
BEventQuit::run()
{
  mApp->quit();
}
