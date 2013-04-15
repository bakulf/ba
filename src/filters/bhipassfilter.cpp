#include "bhipassfilter.h"
#include "bscriptengine.h"
#include "bapplication.h"

#include <iostream>

BHipassFilter::BHipassFilter(QObject* aParent, double aCutOff)
: BEngineFilter(aParent, QString("hipass"))
, mCutOff(aCutOff)
{
}

BHipassFilter::~BHipassFilter()
{
}

void
BHipassFilter::output(double* aOutput)
{
  for (int j=0; j < maxiSettings::channels; ++j) {
    aOutput[j] = mMaxi.hipass(aOutput[j], mCutOff);
  }
}

QScriptValue
BHipassFilter::updateFunction(QScriptContext* aContext,
                              QScriptEngine*)
{
  if (aContext->argumentCount() < 1) {
    std::cerr << "Hipass.update(cutOff) used wrongly." << std::endl;
    return QScriptValue();
  }

  BHipassFilter* filter = static_cast<BHipassFilter*>(aContext->thisObject().toQObject());

  filter->mCutOff = aContext->argument(0).toNumber();

  return QScriptValue();
}

QScriptValue
BHipassFilter::engineFunction(QScriptContext* aContext,
                              QScriptEngine* aEngine)
{
  if (aContext->argumentCount() < 1) {
    std::cerr << "Hipass(cutOff) used wrongly." << std::endl;
    return QScriptValue();
  }

  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);
  BHipassFilter* filter = new BHipassFilter(engine->app(),
                                            aContext->argument(0).toNumber());

  QScriptValue object = filter->objFilter(engine);

  if (aContext->isCalledAsConstructor()) {
    aContext->setThisObject(object);
    return QScriptValue();
  }

  object.setPrototype(aContext->callee().property("prototype"));
  return object;
}

void
BHipassFilter::engineProperties(QScriptEngine* aEngine, QScriptValue aValue)
{
  aValue.setProperty("cutOff", aEngine->newFunction(cutOffFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);
}

QScriptValue
BHipassFilter::cutOffFunction(QScriptContext* aContext,
                              QScriptEngine*)
{
  BHipassFilter* filter = static_cast<BHipassFilter*>(aContext->thisObject().toQObject());

  if (aContext->argumentCount()) {
    filter->mCutOff = aContext->argument(0).toNumber();
  }

  return QScriptValue(filter->mCutOff);
}

QString
BHipassFilter::writeFilter()
{
  QString line;
  line.sprintf("Filter: %s - cutOff %3.2f", qPrintable(name()), mCutOff);
  return line;
}
