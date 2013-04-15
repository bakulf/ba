#include "bmutexlocker.h"

static QMutex gMutex;

BMutexLocker::BMutexLocker()
: QMutexLocker(&gMutex)
{
}

BMutexLocker::~BMutexLocker()
{
}
