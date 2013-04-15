#include "bevent.h"
#include "bapplication.h"
#include "bscriptengine.h"
#include "bmutexlocker.h"

#include <iostream>

BEvent::BEvent(BApplication* aApp, QString aCode)
: QObject(aApp)
, mApp(aApp)
, mCode(aCode)
{
}

BEvent::~BEvent()
{
}

void
BEvent::run()
{
  BScriptEngine* engine = mApp->js();

  BMUTEXLOCKER
  QScriptValue result = engine->evaluate(mCode);

  if (engine->hasUncaughtException()) {
    int line = engine->uncaughtExceptionLineNumber();
    std::cerr << "Uncaught exception at line" << line << ": "
              << qPrintable(result.toString()) << std::endl;
  }
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
