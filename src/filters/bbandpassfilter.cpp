#include "bbandpassfilter.h"
#include "bscriptengine.h"
#include "bapplication.h"

#include <iostream>

BBandpassFilter::BBandpassFilter(QObject* aParent,
                                 double aCutOff, double aResonance)
: BEngineFilter(aParent, QString("bandpass"))
, mCutOff(aCutOff)
, mResonance(aResonance)
{
}

BBandpassFilter::~BBandpassFilter()
{
}

void
BBandpassFilter::output(double* aOutput)
{
  for (int j=0; j < maxiSettings::channels; ++j) {
    aOutput[j] = mMaxi.bandpass(aOutput[j], mCutOff, mResonance);
  }
}

QScriptValue
BBandpassFilter::updateFunction(QScriptContext* aContext,
                                QScriptEngine*)
{
  if (aContext->argumentCount() < 2) {
    std::cerr << "Bandpass.update(cutOff, resonance) used wrongly." << std::endl;
    return QScriptValue();
  }

  BBandpassFilter* filter = static_cast<BBandpassFilter*>(aContext->thisObject().toQObject());

  filter->mCutOff = aContext->argument(0).toNumber();
  filter->mResonance = aContext->argument(1).toNumber();

  return QScriptValue();
}

QScriptValue
BBandpassFilter::engineFunction(QScriptContext* aContext,
                                QScriptEngine* aEngine)
{
  if (aContext->argumentCount() < 2) {
    std::cerr << "Bandpass(cutOff, resonance) used wrongly." << std::endl;
    return QScriptValue();
  }

  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);
  BBandpassFilter* filter = new BBandpassFilter(engine->app(),
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
BBandpassFilter::engineProperties(QScriptEngine* aEngine, QScriptValue aValue)
{
  aValue.setProperty("cutOff", aEngine->newFunction(cutOffFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);

  aValue.setProperty("resonance", aEngine->newFunction(resonanceFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);
}

QScriptValue
BBandpassFilter::cutOffFunction(QScriptContext* aContext,
                                QScriptEngine*)
{
  BBandpassFilter* filter = static_cast<BBandpassFilter*>(aContext->thisObject().toQObject());

  if (aContext->argumentCount()) {
    filter->mCutOff = aContext->argument(0).toNumber();
  }

  return QScriptValue(filter->mCutOff);
}

QScriptValue
BBandpassFilter::resonanceFunction(QScriptContext* aContext,
                                   QScriptEngine*)
{
  BBandpassFilter* filter = static_cast<BBandpassFilter*>(aContext->thisObject().toQObject());

  if (aContext->argumentCount()) {
    filter->mResonance = aContext->argument(0).toNumber();
  }

  return QScriptValue(filter->mResonance);
}

QString
BBandpassFilter::writeFilter()
{
  QString line;
  line.sprintf("Filter: %s - cutOff %3.2f || resonance %3.2f", qPrintable(name()),
              mCutOff, mResonance);
  return line;
}
