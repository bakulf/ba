#include "bloresfilter.h"
#include "bscriptengine.h"
#include "bapplication.h"

#include <iostream>

BLoresFilter::BLoresFilter(QObject* aParent, double aCutOff, double aResonance)
: BEngineFilter(aParent, QString("lores"))
, mCutOff(aCutOff)
, mResonance(aResonance)
{
}

BLoresFilter::~BLoresFilter()
{
}

void
BLoresFilter::output(double* aOutput)
{
  for (int j=0; j < maxiSettings::channels; ++j) {
    aOutput[j] = mMaxi.lores(aOutput[j], mCutOff, mResonance);
  }
}

QScriptValue
BLoresFilter::updateFunction(QScriptContext* aContext,
                             QScriptEngine*)
{
  if (aContext->argumentCount() < 2) {
    std::cerr << "Lores.update(cutOff, resonance) used wrongly." << std::endl;
    return QScriptValue();
  }

  BLoresFilter* filter = static_cast<BLoresFilter*>(aContext->thisObject().toQObject());

  filter->mCutOff = aContext->argument(0).toNumber();
  filter->mResonance = aContext->argument(1).toNumber();

  return QScriptValue();
}

QScriptValue
BLoresFilter::engineFunction(QScriptContext* aContext,
                             QScriptEngine* aEngine)
{
  if (aContext->argumentCount() < 2) {
    std::cerr << "Lores(cutOff, resonance) used wrongly." << std::endl;
    return QScriptValue();
  }

  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);
  BLoresFilter* filter = new BLoresFilter(engine->app(),
                                          aContext->argument(0).toNumber(),
                                          aContext->argument(1).toNumber());

  QScriptValue object = filter->objFilter(engine);

  if (aContext->isCalledAsConstructor()) {
    aContext->setThisObject(object);
    return QScriptValue();
  }

  object.setPrototype(aContext->callee().property("prototype"));
  return object;
}

void
BLoresFilter::engineProperties(QScriptEngine* aEngine, QScriptValue aValue)
{
  aValue.setProperty("cutOff", aEngine->newFunction(cutOffFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);

  aValue.setProperty("resonance", aEngine->newFunction(resonanceFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);
}

QScriptValue
BLoresFilter::cutOffFunction(QScriptContext* aContext,
                             QScriptEngine*)
{
  BLoresFilter* filter = static_cast<BLoresFilter*>(aContext->thisObject().toQObject());

  if (aContext->argumentCount()) {
    filter->mCutOff = aContext->argument(0).toNumber();
  }

  return QScriptValue(filter->mCutOff);
}

QScriptValue
BLoresFilter::resonanceFunction(QScriptContext* aContext,
                                QScriptEngine*)
{
  BLoresFilter* filter = static_cast<BLoresFilter*>(aContext->thisObject().toQObject());

  if (aContext->argumentCount()) {
    filter->mResonance = aContext->argument(0).toNumber();
  }

  return QScriptValue(filter->mResonance);
}

QString
BLoresFilter::writeFilter()
{
  QString line;
  line.sprintf("Filter: %s - cutOff %3.2f || resonance %3.2f", qPrintable(name()),
               mCutOff, mResonance);
  return line;
}
