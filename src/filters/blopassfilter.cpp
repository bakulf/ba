#include "blopassfilter.h"
#include "bscriptengine.h"
#include "bapplication.h"

BLopassFilter::BLopassFilter(QObject* aParent,
                             BGenerator* aCutOff)
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
    aOutput[j] = mMaxi.lopass(aOutput[j], mCutOff->get());
  }
}

QScriptValue
BLopassFilter::updateFunction(QScriptContext* aContext,
                              QScriptEngine*)
{
  if (aContext->argumentCount() < 1) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Lopass.update(cutOff) used wrongly.");
  }

  BGeneratorRef cutOff = BGenerator::numberToGenerator(aContext->argument(0));
  if (!cutOff) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Lopass.update(cutOff) used wrongly.");
  }

  BEngineFilterShell* shell = static_cast<BEngineFilterShell*>(aContext->thisObject().toQObject());
  BLopassFilterRef filter = static_cast<BLopassFilter*>(shell->get());

  filter->mCutOff = cutOff;

  return QScriptValue();
}

QScriptValue
BLopassFilter::engineFunction(QScriptContext* aContext,
                              QScriptEngine* aEngine)
{
  if (aContext->argumentCount() < 1) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Lopass(cutOff) used wrongly.");
  }

  BGeneratorRef cutOff = BGenerator::numberToGenerator(aContext->argument(0));
  if (!cutOff) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Lopass(cutOff) used wrongly.");
  }

  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);
  BLopassFilterRef filter = new BLopassFilter(engine->app(), cutOff);

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

METHOD_FUNCTION(BLopassFilter, cutOffFunction, mCutOff, "Lopass", "cutOff")

QString
BLopassFilter::writeFilter()
{
  QString line;
  line.sprintf("Filter: %s - cutOff %3.2f", qPrintable(name()), mCutOff->get());
  return line;
}
