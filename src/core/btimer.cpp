#include "btimer.h"
#include "bapplication.h"
#include "bscriptengine.h"
#include "bmutexlocker.h"

#include <QTimer>

BTimer::BTimer(BApplication* aApp)
: QObject(aApp)
, mApp(aApp)
, mTimeId(0)
{
}

BTimer::~BTimer()
{
}

QScriptValue
BTimer::setTimeout(QScriptContext* aContext, QScriptValue aValue, int aTimeout)
{
  if (!aValue.isFunction()) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "SetTimeout() value must be a function.");
  }

  int id = ++mTimeId;
  BTimerData* data = new BTimerData(this, aValue, aTimeout, true, id);
  mTimeouts << data;

  return QScriptValue(id);
}

void
BTimer::clearTimeout(int aTimeout)
{
  foreach (BTimerData* data, mTimeouts) {
    if (data->id() == aTimeout) {
      mTimeouts.removeOne(data);
      data->deleteLater();
      break;
    }
  }
}

QScriptValue
BTimer::setInterval(QScriptContext* aContext, QScriptValue aValue, int aInterval)
{
  if (!aValue.isFunction()) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "SetInterval() value must be a function.");
  }

  int id = ++mTimeId;
  BTimerData* data = new BTimerData(this, aValue, aInterval, false, id);
  mIntervals << data;
  return QScriptValue(id);
}

void
BTimer::clearInterval(int aInterval)
{
  foreach (BTimerData* data, mIntervals) {
    if (data->id() == aInterval) {
      mIntervals.removeOne(data);
      data->deleteLater();
      break;
    }
  }
}

BTimerData::BTimerData(BTimer* aTimer,
                       QScriptValue aValue,
                       int aTimeout,
                       bool aSingleShot,
                       int aId)
: QObject(aTimer)
, mParent(aTimer)
, mValue(aValue)
, mId(aId)
{
  mTimer = new QTimer(this);
  connect(mTimer, SIGNAL(timeout()), this, SLOT(timeout()));
  mTimer->start(aTimeout);
  mTimer->setSingleShot(aSingleShot);
}

BTimerData::~BTimerData()
{
}

void
BTimerData::timeout()
{
  BMUTEXLOCKER

  mValue.call();
  if (mValue.engine()->hasUncaughtException()) {
    BScriptEngine* engine = static_cast<BScriptEngine*>(mValue.engine());

    int line = engine->uncaughtExceptionLineNumber();
    QScriptValue result = engine->uncaughtException();
    engine->app()->printMessage(QString("Uncaught exception at line %1: %2")
                                .arg(line).arg(result.toString()));
  }
}
