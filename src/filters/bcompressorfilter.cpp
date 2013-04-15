#include "bcompressorfilter.h"
#include "bscriptengine.h"
#include "bapplication.h"

#include <iostream>

BCompressorFilter::BCompressorFilter(QObject* aParent,
                                     double aRatio,
                                     double aThreshold,
                                     double aAttach,
                                     double aRelease)
: BEngineFilter(aParent, QString("compressor"))
, mRatio(aRatio)
, mThreshold(aThreshold)
, mAttach(aAttach)
, mRelease(aRelease)
{
}

BCompressorFilter::~BCompressorFilter()
{
}

void
BCompressorFilter::output(double* aOutput)
{
  for (int j=0; j < maxiSettings::channels; ++j) {
    aOutput[j] = mMaxi.compressor(aOutput[j], mRatio, mThreshold, mAttach, mRelease);
  }
}

QScriptValue
BCompressorFilter::updateFunction(QScriptContext* aContext,
                                  QScriptEngine*)
{
  if (aContext->argumentCount() < 1) {
    std::cerr << "Compressor.update(ratio, [threshold=0.9, attach=1, release=0.9995]) used wrongly." << std::endl;
    return QScriptValue();
  }

  BCompressorFilter* filter = static_cast<BCompressorFilter*>(aContext->thisObject().toQObject());

  filter->mRatio = aContext->argument(0).toNumber();

  if (aContext->argumentCount() > 1) {
    filter->mThreshold = aContext->argument(1).toNumber();
  } else {
    filter->mThreshold = 0.9;
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
BCompressorFilter::engineFunction(QScriptContext* aContext,
                                  QScriptEngine* aEngine)
{
  if (aContext->argumentCount() < 1) {
    std::cerr << "Compressor(ratio, [threshold=0.9, attach=1, release=0.9995]) used wrongly." << std::endl;
    return QScriptValue();
  }

  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);
  BCompressorFilter* filter = new BCompressorFilter(engine->app(),
                                        aContext->argument(0).toNumber(),
                                        aContext->argumentCount() > 1
                                        ?  aContext->argument(1).toNumber() : 0.9,
                                        aContext->argumentCount() > 2
                                        ?  aContext->argument(2).toNumber() : 1,
                                        aContext->argumentCount() > 3
                                        ?  aContext->argument(3).toNumber() : 0.9995);

  QScriptValue object = filter->objFilter(engine);

  if (aContext->isCalledAsConstructor()) {
    aContext->setThisObject(object);
    return QScriptValue();
  }

  object.setPrototype(aContext->callee().property("prototype"));
  return object;
}

void
BCompressorFilter::engineProperties(QScriptEngine* aEngine, QScriptValue aValue)
{
  aValue.setProperty("ratio", aEngine->newFunction(ratioFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);

  aValue.setProperty("threshold", aEngine->newFunction(thresholdFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);

  aValue.setProperty("attach", aEngine->newFunction(attachFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);

  aValue.setProperty("release", aEngine->newFunction(releaseFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);
}

QScriptValue
BCompressorFilter::ratioFunction(QScriptContext* aContext,
                                 QScriptEngine*)
{
  BCompressorFilter* filter = static_cast<BCompressorFilter*>(aContext->thisObject().toQObject());

  if (aContext->argumentCount()) {
    filter->mRatio = aContext->argument(0).toNumber();
  }

  return QScriptValue(filter->mRatio);
}

QScriptValue
BCompressorFilter::thresholdFunction(QScriptContext* aContext,
                                     QScriptEngine*)
{
  BCompressorFilter* filter = static_cast<BCompressorFilter*>(aContext->thisObject().toQObject());

  if (aContext->argumentCount()) {
    filter->mThreshold = aContext->argument(0).toNumber();
  }

  return QScriptValue(filter->mThreshold);
}

QScriptValue
BCompressorFilter::attachFunction(QScriptContext* aContext,
                                  QScriptEngine*)
{
  BCompressorFilter* filter = static_cast<BCompressorFilter*>(aContext->thisObject().toQObject());

  if (aContext->argumentCount()) {
    filter->mAttach = aContext->argument(0).toNumber();
  }

  return QScriptValue(filter->mAttach);
}

QScriptValue
BCompressorFilter::releaseFunction(QScriptContext* aContext,
                                   QScriptEngine*)
{
  BCompressorFilter* filter = static_cast<BCompressorFilter*>(aContext->thisObject().toQObject());

  if (aContext->argumentCount()) {
    filter->mRelease = aContext->argument(0).toNumber();
  }

  return QScriptValue(filter->mRelease);
}

QString
BCompressorFilter::writeFilter()
{
  QString line;
  line.sprintf("Filter: %s - ratio %3.2f || threshold %3.2f || attach %3.2f || release %3.2f",
               qPrintable(name()), mRatio, mThreshold, mAttach, mRelease);
  return line;
}
