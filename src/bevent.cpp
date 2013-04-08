#include "bevent.h"

#include <iostream>

BEvent::BEvent(QObject* aParent)
: QObject(aParent)
{
}

BEvent::~BEvent()
{
}

void
BEvent::run()
{
  std::cerr << "Some BEvent is not properly implemented!" << std::endl;
}

BEventManager::BEventManager()
{
}


BEventManager::~BEventManager()
{
}

void
BEventManager::add(QChar aCh, BEvent* aEvent)
{
  mMap[aCh] = aEvent;
}

void
BEventManager::handle(QChar aCh)
{
  if (!mMap.contains(aCh)) {
    return;
  }

  mMap[aCh]->run();
}
