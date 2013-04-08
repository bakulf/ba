#include "beventselectbuffer.h"

BEventSelectBuffer::BEventSelectBuffer(BApplication* aApp,
                                       BBuffer* aBuffer)
: BEvent(aApp)
, mApp(aApp)
, mBuffer(aBuffer)
{
}

BEventSelectBuffer::~BEventSelectBuffer()
{
}

void
BEventSelectBuffer::run()
{
  mApp->selectBuffer(mBuffer);
}
