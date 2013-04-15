#include "bgatefilter.h"
#include "bscriptengine.h"
#include "bapplication.h"

#include <iostream>

BGateFilter::BGateFilter(QObject* aParent,
                         double aThreshold,
                         long aHoldtime,
                         double aAttach,
                         double aRelease)
: BEngineFilter(aParent, QString("gate"))
, mThreshold(aThreshold)
, mHoldtime(aHoldtime)
, mAttach(aAttach)
, mRelease(aRelease)
{
}

BGateFilter::~BGateFilter()
{
}

void
BGateFilter::output(double* aOutput)
{
  for (int j=0; j < maxiSettings::channels; ++j) {
    aOutput[j] = mMaxi.gate(aOutput[j], mThreshold, mHoldtime, mAttach, mRelease);
  }
}

QScriptValue
BGateFilter::updateFunction(QScriptContext* aContext,
                            QScriptEngine*)
{
  if (aContext->argumentCount() < 1) {
    std::cerr << "Gate.update(threshold, [holdtime=1, attach=1, release=0.9995]) used wrongly." << std::endl;
    return QScriptValue();
  }

  BGateFilter* filter = static_cast<BGateFilter*>(aContext->thisObject().toQObject());

  filter->mThreshold = aContext->argument(0).toNumber();

  if (aContext->argumentCount() > 1) {
    filter->mHoldtime = aContext->argument(1).toNumber();
  } else {
    filter->mHoldtime = 1;
  }

  if (aContext->argumentCount() > 2) {
    filter->mAttach = aContext->argument(2).toNumber();
  } else {
    filter->mAttach = 1;
  }

  if (aContext->argumentCount() > 3) {
    filter->mRelease = aContext->argument(3).toNumber();
  } else {
    filter->mRelease = 0.9995;
  }

  return QScriptValue();
}

QScriptValue
BGateFilter::engineFunction(QScriptContext* aContext,
                            QScriptEngine* aEngine)
{
  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);
  BGateFilter* filter = new BGateFilter(engine->app(),
                                        aContext->argumentCount() > 0
                                        ?  aContext->argument(0).toNumber() : 0.9,
                                        aContext->argumentCount() > 1
                                        ?  aContext->argument(0).toNumber() : 1,
                                        aContext->argumentCount() > 2
                                        ?  aContext->argument(0).toNumber() : 1,
                                        aContext->argumentCount() > 3
                                        ?  aContext->argument(0).toNumber() : 0.9995);

  QScriptValue object = filter->objFilter(engine);

  if (aContext->isCalledAsConstructor()) {
    aContext->setThisObject(object);
    return QScriptValue();
  }

  object.setPrototype(aContext->callee().property("prototype"));
  return object;
}

void
BGateFilter::engineProperties(QScriptEngine* aEngine, QScriptValue aValue)
{
  aValue.setProperty("threshold", aEngine->newFunction(thresholdFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);

  aValue.setProperty("holdtime", aEngine->newFunction(holdtimeFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);

  aValue.setProperty("attach", aEngine->newFunction(attachFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);

  aValue.setProperty("release", aEngine->newFunction(releaseFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);
}

QScriptValue
BGateFilter::thresholdFunction(QScriptContext* aContext,
                               QScriptEngine*)
{
  BGateFilter* filter = static_cast<BGateFilter*>(aContext->thisObject().toQObject());

  if (aContext->argumentCount()) {
    filter->mThreshold = aContext->argument(0).toNumber();
  }

  return QScriptValue(filter->mThreshold);
}

QScriptValue
BGateFilter::holdtimeFunction(QScriptContext* aContext,
                              QScriptEngine*)
{
  BGateFilter* filter = static_cast<BGateFilter*>(aContext->thisObject().toQObject());

  if (aContext->argumentCount()) {
    filter->mHoldtime = aContext->argument(0).toNumber();
  }

  return QScriptValue((double)filter->mHoldtime);
}

QScriptValue
BGateFilter::attachFunction(QScriptContext* aContext,
                            QScriptEngine*)
{
  BGateFilter* filter = static_cast<BGateFilter*>(aContext->thisObject().toQObject());

  if (aContext->argumentCount()) {
    filter->mAttach = aContext->argument(0).toNumber();
  }

  return QScriptValue(filter->mAttach);
}

QScriptValue
BGateFilter::releaseFunction(QScriptContext* aContext,
                             QScriptEngine*)
{
  BGateFilter* filter = static_cast<BGateFilter*>(aContext->thisObject().toQObject());

  if (aContext->argumentCount()) {
    filter->mRelease = aContext->argument(0).toNumber();
  }

  return QScriptValue(filter->mRelease);
}

QString
BGateFilter::writeFilter()
{
  QString line;
  line.sprintf("Filter: %s - threshold %3.2f || holdtime %ld || attach %3.2f || release %3.2f",
               qPrintable(name()), mThreshold, mHoldtime, mAttach, mRelease);
  return line;
}
