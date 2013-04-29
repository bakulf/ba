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
    QString e;
    e.sprintf("Uncaught exception at line %d: %s", line,
              qPrintable(result.toString()));
    engine->app()->printMessage(e);
  }
}

BEventManager::BEventManager()
{
}


BEventManager::~BEventManager()
{
}

void
BEventManager::add(char aCh, BEvent* aEvent)
{
  mMap[aCh] = aEvent;
}

void
BEventManager::handle(char aCh)
{
  if (!mMap.contains(aCh)) {
    return;
  }

  mMap[aCh]->run();
}
