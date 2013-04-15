#include "bdistortionfilter.h"
#include "bscriptengine.h"
#include "bapplication.h"

#include <iostream>

BDistortionFilter::BDistortionFilter(QObject* aParent, Type aType, double aShape)
: BEngineFilter(aParent, aType == ATAN ? QString("atandistortion") : QString("fastatandistortion"))
, mShape(aShape)
, mType(aType)
{
}

BDistortionFilter::~BDistortionFilter()
{
}

void
BDistortionFilter::output(double* aOutput)
{
  if (mType == ATAN) {
    for (int j=0; j < maxiSettings::channels; ++j) {
      aOutput[j] = mMaxi.atanDist(aOutput[j], mShape);
    }
  } else {
    for (int j=0; j < maxiSettings::channels; ++j) {
      aOutput[j] = mMaxi.fastAtanDist(aOutput[j], mShape);
    }
  }
}

QScriptValue
BDistortionFilter::updateFunction(QScriptContext* aContext,
                                  QScriptEngine*)
{
  BDistortionFilter* filter = static_cast<BDistortionFilter*>(aContext->thisObject().toQObject());

  if (aContext->argumentCount() < 1) {
    std::cerr << (filter->mType == ATAN ? "AtanDistortion" : "FastAtanDistortion")
              << ".update(shape) used wrongly." << std::endl;
    return QScriptValue();
  }

  filter->mShape = aContext->argument(0).toNumber();

  return QScriptValue();
}

QScriptValue
BDistortionFilter::engineAtanFunction(QScriptContext* aContext,
                                      QScriptEngine* aEngine)
{
  return engineFunction(ATAN, aContext, aEngine);
}

QScriptValue
BDistortionFilter::engineFastAtanFunction(QScriptContext* aContext,
                                          QScriptEngine* aEngine)
{
  return engineFunction(FASTATAN, aContext, aEngine);
}

QScriptValue
BDistortionFilter::engineFunction(Type aType,
                                  QScriptContext* aContext,
                                  QScriptEngine* aEngine)
{
  if (aContext->argumentCount() < 1) {
    std::cerr << (aType == ATAN ? "AtanDistortion" : "FastAtanDistortion")
              << "(shape) used wrongly." << std::endl;
    return QScriptValue();
  }

  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);
  BDistortionFilter* filter = new BDistortionFilter(engine->app(),
                                    aType, aContext->argument(0).toNumber());

  QScriptValue object = filter->objFilter(engine);

  if (aContext->isCalledAsConstructor()) {
    aContext->setThisObject(object);
    return QScriptValue();
  }

  object.setPrototype(aContext->callee().property("prototype"));
  return object;
}

void
BDistortionFilter::engineProperties(QScriptEngine* aEngine, QScriptValue aValue)
{
  aValue.setProperty("shape", aEngine->newFunction(shapeFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);
}

QScriptValue
BDistortionFilter::shapeFunction(QScriptContext* aContext,
                                 QScriptEngine*)
{
  BDistortionFilter* filter = static_cast<BDistortionFilter*>(aContext->thisObject().toQObject());

  if (aContext->argumentCount()) {
    filter->mShape = aContext->argument(0).toNumber();
  }

  return QScriptValue(filter->mShape);
}

QString
BDistortionFilter::writeFilter()
{
  QString line;
  line.sprintf("Filter: %s - shape %3.2f",
               qPrintable(name()), mShape);
  return line;
}
