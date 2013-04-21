#include "bhiresfilter.h"
#include "bscriptengine.h"
#include "bapplication.h"

BHiresFilter::BHiresFilter(QObject* aParent,
                           BGenerator* aCutOff,
                           BGenerator* aResonance)
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
    aOutput[j] = mMaxi.hires(aOutput[j], mCutOff->get(), mResonance->get());
  }
}

QScriptValue
BHiresFilter::updateFunction(QScriptContext* aContext,
                             QScriptEngine*)
{
  if (aContext->argumentCount() < 2) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Hires.update(cutOff, resonance) used wrongly.");
  }

  BGeneratorRef cutOff = BGenerator::numberToGenerator(aContext->argument(0));
  BGeneratorRef resonance = BGenerator::numberToGenerator(aContext->argument(1));

  if (!cutOff || !resonance) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Hires.update(cutOff, resonance) used wrongly.");
  }

  BHiresFilter* filter = static_cast<BHiresFilter*>(aContext->thisObject().toQObject());

  filter->mCutOff = cutOff;
  filter->mResonance = resonance;

  return QScriptValue();
}

QScriptValue
BHiresFilter::engineFunction(QScriptContext* aContext,
                             QScriptEngine* aEngine)
{
  if (aContext->argumentCount() < 2) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Hires(cutOff, resonance) used wrongly.");
  }

  BGeneratorRef cutOff = BGenerator::numberToGenerator(aContext->argument(0));
  BGeneratorRef resonance = BGenerator::numberToGenerator(aContext->argument(1));

  if (!cutOff || !resonance) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Hires(cutOff, resonance) used wrongly.");
  }

  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);
  BHiresFilter* filter = new BHiresFilter(engine->app(), cutOff, resonance);

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

METHOD_FUNCTION(BHiresFilter, cutOffFunction, mCutOff, "Hires", "cutOff")
METHOD_FUNCTION(BHiresFilter, resonanceFunction, mResonance, "Hires", "resonance")

QString
BHiresFilter::writeFilter()
{
  QString line;
  line.sprintf("Filter: %s - cutOff %3.2f || resonance %3.2f", qPrintable(name()),
               mCutOff->get(), mResonance->get());
  return line;
}
