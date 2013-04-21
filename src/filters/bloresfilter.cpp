#include "bloresfilter.h"
#include "bscriptengine.h"
#include "bapplication.h"

BLoresFilter::BLoresFilter(QObject* aParent,
                           BGenerator* aCutOff,
                           BGenerator* aResonance)
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
    aOutput[j] = mMaxi.lores(aOutput[j], mCutOff->get(), mResonance->get());
  }
}

QScriptValue
BLoresFilter::updateFunction(QScriptContext* aContext,
                             QScriptEngine*)
{
  if (aContext->argumentCount() < 2) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Lores.update(cutOff, resonance) used wrongly.");
  }

  BGeneratorRef cutOff = BGenerator::numberToGenerator(aContext->argument(0));
  BGeneratorRef resonance = BGenerator::numberToGenerator(aContext->argument(1));

  if (!cutOff || !resonance) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Lores.update(cutOff, resonance) used wrongly.");
  }

  BLoresFilter* filter = static_cast<BLoresFilter*>(aContext->thisObject().toQObject());

  filter->mCutOff = cutOff;
  filter->mResonance = resonance;

  return QScriptValue();
}

QScriptValue
BLoresFilter::engineFunction(QScriptContext* aContext,
                             QScriptEngine* aEngine)
{
  if (aContext->argumentCount() < 2) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Lores(cutOff, resonance) used wrongly.");
  }

  BGeneratorRef cutOff = BGenerator::numberToGenerator(aContext->argument(0));
  BGeneratorRef resonance = BGenerator::numberToGenerator(aContext->argument(1));

  if (!cutOff || !resonance) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Lores(cutOff, resonance) used wrongly.");
  }

  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);
  BLoresFilter* filter = new BLoresFilter(engine->app(),
                                          cutOff, resonance);

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

METHOD_FUNCTION(BLoresFilter, cutOffFunction, mCutOff, "Lores", "cutOff")
METHOD_FUNCTION(BLoresFilter, resonanceFunction, mResonance, "Lores", "resonance")

QString
BLoresFilter::writeFilter()
{
  QString line;
  line.sprintf("Filter: %s - cutOff %3.2f || resonance %3.2f", qPrintable(name()),
               mCutOff->get(), mResonance->get());
  return line;
}
