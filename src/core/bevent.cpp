#include "bevent.h"
#include "bapplication.h"
#include "bscriptengine.h"
#include "bmutexlocker.h"

#include <iostream>

BEvent::BEvent(BApplication* aApp, QString aCode, QString aName)
: QObject(aApp)
, mApp(aApp)
, mCode(aCode)
, mName(aName)
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
    engine->app()->printMessage(QString("Uncaught exception at line %1: %2")
                                .arg(line).arg(result.toString()));
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

QString
BEventManager::handle(char aCh)
{
  if (!mMap.contains(aCh)) {
    return QString();
  }

  mMap[aCh]->run();
  return mMap[aCh]->name();
}
