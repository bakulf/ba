#include "bdelaylinefilter.h"
#include "bscriptengine.h"
#include "bapplication.h"

#include <iostream>

BDelaylineFilter::BDelaylineFilter(QObject* aParent,
                                   int aSize, double aFeedback, int aPosition)
: BEngineFilter(aParent, QString("delayline"))
, mSize(aSize)
, mFeedback(aFeedback)
, mPosition(aPosition)
{
}

BDelaylineFilter::~BDelaylineFilter()
{
}

void
BDelaylineFilter::output(double* aOutput)
{
  for (int j=0; j < maxiSettings::channels; ++j) {
    aOutput[j] = mMaxi.dl(aOutput[j], mSize, mFeedback, mPosition);
  }
}

QScriptValue
BDelaylineFilter::updateFunction(QScriptContext* aContext,
                                 QScriptEngine*)
{
  if (aContext->argumentCount() < 2) {
    std::cerr << "Delayline.update(size, feedback[, position]) used wrongly." << std::endl;
    return QScriptValue();
  }

  BDelaylineFilter* filter = static_cast<BDelaylineFilter*>(aContext->thisObject().toQObject());

  filter->mSize = (int)aContext->argument(0).toInt32();
  filter->mFeedback = aContext->argument(1).toNumber();

  if (aContext->argumentCount() > 2) {
    filter->mPosition = (int)aContext->argument(2).toInt32();
  } else {
    filter->mPosition = 0;
  }

  return QScriptValue();
}

QScriptValue
BDelaylineFilter::engineFunction(QScriptContext* aContext,
                                 QScriptEngine* aEngine)
{
  if (aContext->argumentCount() < 2) {
    std::cerr << "Delayline(size, feedback[, position]) used wrongly." << std::endl;
    return QScriptValue();
  }

  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);
  BDelaylineFilter* filter = new BDelaylineFilter(engine->app(),
                                                  (int)aContext->argument(0).toInt32(),
                                                  aContext->argument(1).toNumber(),
                                                  aContext->argumentCount() > 2
                                                    ? (int)aContext->argument(0).toInt32()
                                                    : 0);

  QScriptValue object = filter->objFilter(engine);

  if (aContext->isCalledAsConstructor()) {
    aContext->setThisObject(object);
    return QScriptValue();
  }

  object.setPrototype(aContext->callee().property("prototype"));
  return object;
}

void
BDelaylineFilter::engineProperties(QScriptEngine* aEngine, QScriptValue aValue)
{
  aValue.setProperty("size", aEngine->newFunction(sizeFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);

  aValue.setProperty("feedback", aEngine->newFunction(feedbackFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);

  aValue.setProperty("position", aEngine->newFunction(positionFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);
}

QScriptValue
BDelaylineFilter::sizeFunction(QScriptContext* aContext,
                               QScriptEngine*)
{
  BDelaylineFilter* filter = static_cast<BDelaylineFilter*>(aContext->thisObject().toQObject());

  if (aContext->argumentCount()) {
    filter->mSize = aContext->argument(0).toNumber();
  }

  return QScriptValue(filter->mSize);
}

QScriptValue
BDelaylineFilter::feedbackFunction(QScriptContext* aContext,
                                   QScriptEngine*)
{
  BDelaylineFilter* filter = static_cast<BDelaylineFilter*>(aContext->thisObject().toQObject());

  if (aContext->argumentCount()) {
    filter->mFeedback = aContext->argument(0).toNumber();
  }

  return QScriptValue(filter->mFeedback);
}

QScriptValue
BDelaylineFilter::positionFunction(QScriptContext* aContext,
                                   QScriptEngine*)
{
  BDelaylineFilter* filter = static_cast<BDelaylineFilter*>(aContext->thisObject().toQObject());

  if (aContext->argumentCount()) {
    filter->mPosition = aContext->argument(0).toNumber();
  }

  return QScriptValue(filter->mPosition);
}

QString
BDelaylineFilter::writeFilter()
{
  QString line;
  line.sprintf("Filter: %s - size %d || feedback %3.2f || position %d",
               qPrintable(name()), mSize, mFeedback, mPosition);
  return line;
}
