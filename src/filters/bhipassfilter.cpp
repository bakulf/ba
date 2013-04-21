#include "bhipassfilter.h"
#include "bscriptengine.h"
#include "bapplication.h"

BHipassFilter::BHipassFilter(QObject* aParent,
                             BGenerator* aCutOff)
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
    aOutput[j] = mMaxi.hipass(aOutput[j], mCutOff->get());
  }
}

QScriptValue
BHipassFilter::updateFunction(QScriptContext* aContext,
                              QScriptEngine*)
{
  if (aContext->argumentCount() < 1) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Hipass.update(cutOff) used wrongly.");
  }

  BGeneratorRef cutOff = BGenerator::numberToGenerator(aContext->argument(0));
  if (!cutOff) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Hipass.update(cutOff) used wrongly.");
  }

  BEngineFilterShell* shell = static_cast<BEngineFilterShell*>(aContext->thisObject().toQObject());
  BHipassFilterRef filter = static_cast<BHipassFilter*>(shell->get());

  filter->mCutOff = cutOff;

  return QScriptValue();
}

QScriptValue
BHipassFilter::engineFunction(QScriptContext* aContext,
                              QScriptEngine* aEngine)
{
  if (aContext->argumentCount() < 1) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Hipass(cutOff) used wrongly.");
  }

  BGeneratorRef cutOff = BGenerator::numberToGenerator(aContext->argument(0));
  if (!cutOff) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Hipass(cutOff) used wrongly.");
  }

  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);
  BHipassFilterRef filter = new BHipassFilter(engine->app(), cutOff);

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

METHOD_FUNCTION(BHipassFilter, BEngineFilterShell, cutOffFunction,
                mCutOff, "Hipass", "cutOff")

QString
BHipassFilter::writeFilter()
{
  QString line;
  line.sprintf("Filter: %s - cutOff %3.2f", qPrintable(name()), mCutOff->get());
  return line;
}
