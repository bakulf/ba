#include "blopassfilter.h"
#include "bscriptengine.h"
#include "bapplication.h"

#include <iostream>

BLopassFilter::BLopassFilter(QObject* aParent, double aCutOff)
: BEngineFilter(aParent, QString("lopass"))
, mCutOff(aCutOff)
{
}

BLopassFilter::~BLopassFilter()
{
}

void
BLopassFilter::output(double* aOutput)
{
  for (int j=0; j < maxiSettings::channels; ++j) {
    aOutput[j] = mMaxi.lopass(aOutput[j], mCutOff);
  }
}

QScriptValue
BLopassFilter::updateFunction(QScriptContext* aContext,
                              QScriptEngine*)
{
  if (aContext->argumentCount() < 1) {
    std::cerr << "Lopass.update(cutOff) used wrongly." << std::endl;
    return QScriptValue();
  }

  BLopassFilter* filter = static_cast<BLopassFilter*>(aContext->thisObject().toQObject());

  filter->mCutOff = aContext->argument(0).toNumber();

  return QScriptValue();
}

QScriptValue
BLopassFilter::engineFunction(QScriptContext* aContext,
                              QScriptEngine* aEngine)
{
  if (aContext->argumentCount() < 1) {
    std::cerr << "Lopass(cutOff) used wrongly." << std::endl;
    return QScriptValue();
  }

  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);
  BLopassFilter* filter = new BLopassFilter(engine->app(),
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
BLopassFilter::engineProperties(QScriptEngine* aEngine, QScriptValue aValue)
{
  aValue.setProperty("cutOff", aEngine->newFunction(cutOffFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);
}

QScriptValue
BLopassFilter::cutOffFunction(QScriptContext* aContext,
                              QScriptEngine*)
{
  BLopassFilter* filter = static_cast<BLopassFilter*>(aContext->thisObject().toQObject());

  if (aContext->argumentCount()) {
    filter->mCutOff = aContext->argument(0).toNumber();
  }

  return QScriptValue(filter->mCutOff);
}

QString
BLopassFilter::writeFilter()
{
  QString line;
  line.sprintf("Filter: %s - cutOff %3.2f", qPrintable(name()), mCutOff);
  return line;
}
