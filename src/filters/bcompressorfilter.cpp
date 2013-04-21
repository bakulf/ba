#include "bcompressorfilter.h"
#include "bscriptengine.h"
#include "bapplication.h"
#include "bnumbergenerator.h"

BCompressorFilter::BCompressorFilter(QObject* aParent,
                                     BGenerator* aRatio,
                                     BGenerator* aThreshold,
                                     BGenerator* aAttach,
                                     BGenerator* aRelease)
: BEngineFilter(aParent, QString("compressor"))
, mRatio(aRatio)
, mThreshold(aThreshold)
, mAttach(aAttach)
, mRelease(aRelease)
{
}

BCompressorFilter::~BCompressorFilter()
{
}

void
BCompressorFilter::output(double* aOutput)
{
  for (int j=0; j < maxiSettings::channels; ++j) {
    aOutput[j] = mMaxi.compressor(aOutput[j], mRatio->get(), mThreshold->get(),
                                  mAttach->get(), mRelease->get());
  }
}

QScriptValue
BCompressorFilter::updateFunction(QScriptContext* aContext,
                                  QScriptEngine*)
{
  if (aContext->argumentCount() < 1) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Compressor.update(ratio, [threshold=0.9, attach=1, release=0.9995]) used wrongly.");
  }

  BGeneratorRef ratio = BGenerator::numberToGenerator(aContext->argument(0));

  BGeneratorRef threshold;
  if (aContext->argumentCount() > 1) {
    threshold = BGenerator::numberToGenerator(aContext->argument(1));
  } else {
    threshold = new BNumberGenerator(0.9);
  }

  BGeneratorRef attach;
  if (aContext->argumentCount() > 2) {
    attach = BGenerator::numberToGenerator(aContext->argument(2));
  } else {
    attach = new BNumberGenerator(1);
  }

  BGeneratorRef release;
  if (aContext->argumentCount() > 3) {
    release = BGenerator::numberToGenerator(aContext->argument(3));
  } else {
    release = new BNumberGenerator(0.9995);
  }

  if (!ratio || !threshold || !attach || !release) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Compressor.update(ratio, [threshold=0.9, attach=1, release=0.9995]) used wrongly.");
  }

  BEngineFilterShell* shell = static_cast<BEngineFilterShell*>(aContext->thisObject().toQObject());
  BCompressorFilterRef filter = static_cast<BCompressorFilter*>(shell->get());

  filter->mRatio = ratio;
  filter->mThreshold = threshold;
  filter->mAttach = attach;
  filter->mRelease = release;

  return QScriptValue();
}

QScriptValue
BCompressorFilter::engineFunction(QScriptContext* aContext,
                                  QScriptEngine* aEngine)
{
  if (aContext->argumentCount() < 1) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Compressor(ratio, [threshold=0.9, attach=1, release=0.9995]) used wrongly.");
  }

  BGeneratorRef ratio = BGenerator::numberToGenerator(aContext->argument(0));

  BGeneratorRef threshold;
  if (aContext->argumentCount() > 1) {
    threshold = BGenerator::numberToGenerator(aContext->argument(1));
  } else {
    threshold = new BNumberGenerator(0.9);
  }

  BGeneratorRef attach;
  if (aContext->argumentCount() > 2) {
    attach = BGenerator::numberToGenerator(aContext->argument(2));
  } else {
    attach = new BNumberGenerator(1);
  }

  BGeneratorRef release;
  if (aContext->argumentCount() > 3) {
    release = BGenerator::numberToGenerator(aContext->argument(3));
  } else {
    release = new BNumberGenerator(0.9995);
  }

  if (!ratio || !threshold || !attach || !release) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Compressor(ratio, [threshold=0.9, attach=1, release=0.9995]) used wrongly.");
  }

  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);
  BCompressorFilterRef filter = new BCompressorFilter(engine->app(),
                                                      ratio, threshold, attach,
                                                      release);

  QScriptValue object = filter->objFilter(engine);

  if (aContext->isCalledAsConstructor()) {
    aContext->setThisObject(object);
    return QScriptValue();
  }

  object.setPrototype(aContext->callee().property("prototype"));
  return object;
}

void
BCompressorFilter::engineProperties(QScriptEngine* aEngine, QScriptValue aValue)
{
  aValue.setProperty("ratio", aEngine->newFunction(ratioFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);

  aValue.setProperty("threshold", aEngine->newFunction(thresholdFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);

  aValue.setProperty("attach", aEngine->newFunction(attachFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);

  aValue.setProperty("release", aEngine->newFunction(releaseFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);
}

METHOD_FUNCTION(BCompressorFilter, ratioFunction, mRatio, "Bandpass", "ratio");
METHOD_FUNCTION(BCompressorFilter, thresholdFunction, mThreshold, "Bandpass", "threshold");
METHOD_FUNCTION(BCompressorFilter, attachFunction, mAttach, "Bandpass", "attach");
METHOD_FUNCTION(BCompressorFilter, releaseFunction, mRelease, "Bandpass", "release");

QString
BCompressorFilter::writeFilter()
{
  QString line;
  line.sprintf("Filter: %s - ratio %3.2f || threshold %3.2f || attach %3.2f || release %3.2f",
               qPrintable(name()), mRatio->get(), mThreshold->get(),
               mAttach->get(), mRelease->get());
  return line;
}
