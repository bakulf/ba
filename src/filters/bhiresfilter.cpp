#include "bhiresfilter.h"
#include "bscriptengine.h"
#include "bapplication.h"

#include <iostream>

BHiresFilter::BHiresFilter(QObject* aParent, double aCutOff, double aResonance)
: BEngineFilter(aParent, QString("hires"))
, mCutOff(aCutOff)
, mResonance(aResonance)
{
}

BHiresFilter::~BHiresFilter()
{
}

void
BHiresFilter::output(double* aOutput)
{
  for (int j=0; j < maxiSettings::channels; ++j) {
    aOutput[j] = mMaxi.hires(aOutput[j], mCutOff, mResonance);
  }
}

QScriptValue
BHiresFilter::updateFunction(QScriptContext* aContext,
                             QScriptEngine*)
{
  if (aContext->argumentCount() < 2) {
    std::cerr << "Hires.update(cutOff, resonance) used wrongly." << std::endl;
    return QScriptValue();
  }

  BHiresFilter* filter = static_cast<BHiresFilter*>(aContext->thisObject().toQObject());
  if (!filter) {
    std::cerr << "Hires.update(cutOff, resonance) used wrongly." << std::endl;
    return QScriptValue();
  }

  filter->mCutOff = aContext->argument(0).toNumber();
  filter->mResonance = aContext->argument(1).toNumber();

  return QScriptValue();
}

QScriptValue
BHiresFilter::engineFunction(QScriptContext* aContext,
                             QScriptEngine* aEngine)
{
  if (aContext->argumentCount() < 2) {
    std::cerr << "Hires(cutOff, resonance) used wrongly." << std::endl;
    return QScriptValue();
  }

  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);
  BHiresFilter* filter = new BHiresFilter(engine->app(),
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
BHiresFilter::engineProperties(QScriptEngine* aEngine, QScriptValue aValue)
{
  aValue.setProperty("cutOff", aEngine->newFunction(cutOffFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);

  aValue.setProperty("resonance", aEngine->newFunction(resonanceFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);
}

QScriptValue
BHiresFilter::cutOffFunction(QScriptContext* aContext,
                             QScriptEngine*)
{
  BHiresFilter* filter = static_cast<BHiresFilter*>(aContext->thisObject().toQObject());

  if (aContext->argumentCount()) {
    filter->mCutOff = aContext->argument(0).toNumber();
  }

  return QScriptValue(filter->mCutOff);
}

QScriptValue
BHiresFilter::resonanceFunction(QScriptContext* aContext,
                                QScriptEngine*)
{
  BHiresFilter* filter = static_cast<BHiresFilter*>(aContext->thisObject().toQObject());

  if (aContext->argumentCount()) {
    filter->mResonance = aContext->argument(0).toNumber();
  }

  return QScriptValue(filter->mResonance);
}

QString
BHiresFilter::writeFilter()
{
  QString line;
  line.sprintf("Filter: %s - cutOff %3.2f || resonance %3.2f", qPrintable(name()),
               mCutOff, mResonance);
  return line;
}
