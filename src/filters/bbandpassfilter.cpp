#include "bbandpassfilter.h"
#include "bscriptengine.h"
#include "bapplication.h"
#include "bgenerator.h"

BBandpassFilter::BBandpassFilter(QObject* aParent,
                                 BGenerator* aCutOff,
                                 BGenerator* aResonance)
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
    aOutput[j] = mMaxi.bandpass(aOutput[j], mCutOff->get(), mResonance->get());
  }
}

QScriptValue
BBandpassFilter::updateFunction(QScriptContext* aContext,
                                QScriptEngine*)
{
  if (aContext->argumentCount() < 2) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Bandpass.update(cutOff, resonance) used wrongly.");
  }

  BGeneratorRef cutOff = BGenerator::numberToGenerator(aContext->argument(0));
  BGeneratorRef resonance = BGenerator::numberToGenerator(aContext->argument(1));

  if (!cutOff || !resonance) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Bandpass.update(cutOff, resonance) used wrongly.");
  }


  BBandpassFilter* filter = static_cast<BBandpassFilter*>(aContext->thisObject().toQObject());

  filter->mCutOff = cutOff;
  filter->mResonance = resonance;

  return QScriptValue();
}

QScriptValue
BBandpassFilter::engineFunction(QScriptContext* aContext,
                                QScriptEngine* aEngine)
{
  if (aContext->argumentCount() < 2) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Bandpass(cutOff, resonance) used wrongly.");
  }

  BGeneratorRef cutOff = BGenerator::numberToGenerator(aContext->argument(0));
  BGeneratorRef resonance = BGenerator::numberToGenerator(aContext->argument(1));

  if (!cutOff || !resonance) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Bandpass(cutOff, resonance) used wrongly.");
  }

  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);
  BBandpassFilter* filter = new BBandpassFilter(engine->app(),
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
BBandpassFilter::engineProperties(QScriptEngine* aEngine, QScriptValue aValue)
{
  aValue.setProperty("cutOff", aEngine->newFunction(cutOffFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);

  aValue.setProperty("resonance", aEngine->newFunction(resonanceFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);
}

METHOD_FUNCTION(BBandpassFilter, cutOffFunction, mCutOff, "Bandpass", "cutOff");
METHOD_FUNCTION(BBandpassFilter, resonanceFunction, mResonance, "Bandpass", "resonance");

QString
BBandpassFilter::writeFilter()
{
  QString line;
  line.sprintf("Filter: %s - cutOff %3.2f || resonance %3.2f", qPrintable(name()),
              mCutOff->get(), mResonance->get());
  return line;
}
