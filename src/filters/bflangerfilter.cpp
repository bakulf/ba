#include "bflangerfilter.h"
#include "bscriptengine.h"
#include "bapplication.h"

#include <iostream>

BFlangerFilter::BFlangerFilter(QObject* aParent, int aDelay, double aFeedback,
                               double aSpeed, double aDepth)
: BEngineFilter(aParent, QString("flanger"))
, mDelay(aDelay)
, mFeedback(aFeedback)
, mSpeed(aSpeed)
, mDepth(aDepth)
{
}

BFlangerFilter::~BFlangerFilter()
{
}

void
BFlangerFilter::output(double* aOutput)
{
  for (int j=0; j < maxiSettings::channels; ++j) {
    aOutput[j] = mMaxi.flange(aOutput[j], mDelay, mFeedback, mSpeed, mDepth);
  }
}

QScriptValue
BFlangerFilter::updateFunction(QScriptContext* aContext,
                               QScriptEngine*)
{
  if (aContext->argumentCount() < 4) {
    std::cerr << "Flanger.update(delay, feedback, speed, depth) used wrongly." << std::endl;
    return QScriptValue();
  }

  BFlangerFilter* filter = static_cast<BFlangerFilter*>(aContext->thisObject().toQObject());

  filter->mDelay = aContext->argument(0).toInt32();
  filter->mFeedback = aContext->argument(1).toNumber();
  filter->mSpeed = aContext->argument(2).toNumber();
  filter->mDepth = aContext->argument(3).toNumber();

  return QScriptValue();
}

QScriptValue
BFlangerFilter::engineFunction(QScriptContext* aContext,
                               QScriptEngine* aEngine)
{
  if (aContext->argumentCount() < 4) {
    std::cerr << "Flanger(delay, feedback, speed, depth) used wrongly." << std::endl;
    return QScriptValue();
  }

  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);
  BFlangerFilter* filter = new BFlangerFilter(engine->app(),
                                              aContext->argument(0).toInt32(),
                                              aContext->argument(1).toNumber(),
                                              aContext->argument(2).toNumber(),
                                              aContext->argument(3).toNumber());

  QScriptValue object = filter->objFilter(engine);

  if (aContext->isCalledAsConstructor()) {
    aContext->setThisObject(object);
    return QScriptValue();
  }

  object.setPrototype(aContext->callee().property("prototype"));
  return object;
}

void
BFlangerFilter::engineProperties(QScriptEngine* aEngine, QScriptValue aValue)
{
  aValue.setProperty("delay", aEngine->newFunction(delayFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);

  aValue.setProperty("feedback", aEngine->newFunction(feedbackFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);

  aValue.setProperty("speed", aEngine->newFunction(speedFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);

  aValue.setProperty("depth", aEngine->newFunction(depthFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);
}

QScriptValue
BFlangerFilter::delayFunction(QScriptContext* aContext,
                              QScriptEngine*)
{
  BFlangerFilter* filter = static_cast<BFlangerFilter*>(aContext->thisObject().toQObject());

  if (aContext->argumentCount()) {
    filter->mDelay = aContext->argument(0).toInt32();
  }

  return QScriptValue(filter->mDelay);
}

QScriptValue
BFlangerFilter::feedbackFunction(QScriptContext* aContext,
                                 QScriptEngine*)
{
  BFlangerFilter* filter = static_cast<BFlangerFilter*>(aContext->thisObject().toQObject());

  if (aContext->argumentCount()) {
    filter->mFeedback = aContext->argument(0).toNumber();
  }

  return QScriptValue(filter->mFeedback);
}

QScriptValue
BFlangerFilter::speedFunction(QScriptContext* aContext,
                              QScriptEngine*)
{
  BFlangerFilter* filter = static_cast<BFlangerFilter*>(aContext->thisObject().toQObject());

  if (aContext->argumentCount()) {
    filter->mSpeed = aContext->argument(0).toNumber();
  }

  return QScriptValue(filter->mSpeed);
}

QScriptValue
BFlangerFilter::depthFunction(QScriptContext* aContext,
                              QScriptEngine*)
{
  BFlangerFilter* filter = static_cast<BFlangerFilter*>(aContext->thisObject().toQObject());

  if (aContext->argumentCount()) {
    filter->mDepth = aContext->argument(0).toNumber();
  }

  return QScriptValue(filter->mDepth);
}

QString
BFlangerFilter::writeFilter()
{
  QString line;
  line.sprintf("Filter: %s - delay %d || feedback %3.2f || speed %3.2f || depth %3.2f",
               qPrintable(name()), mDelay, mFeedback, mSpeed, mDepth);
  return line;
}
