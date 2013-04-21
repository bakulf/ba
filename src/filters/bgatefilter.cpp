#include "bgatefilter.h"
#include "bscriptengine.h"
#include "bnumbergenerator.h"
#include "bapplication.h"

BGateFilter::BGateFilter(QObject* aParent,
                         BGenerator* aThreshold,
                         BGenerator* aHoldtime,
                         BGenerator* aAttach,
                         BGenerator* aRelease)
: BEngineFilter(aParent, QString("gate"))
, mThreshold(aThreshold)
, mHoldtime(aHoldtime)
, mAttach(aAttach)
, mRelease(aRelease)
{
}

BGateFilter::~BGateFilter()
{
}

void
BGateFilter::output(double* aOutput)
{
  for (int j=0; j < maxiSettings::channels; ++j) {
    aOutput[j] = mMaxi.gate(aOutput[j], mThreshold->get(),
                            mHoldtime->get(), mAttach->get(),
                            mRelease->get());
  }
}

QScriptValue
BGateFilter::updateFunction(QScriptContext* aContext,
                            QScriptEngine*)
{
  if (aContext->argumentCount() < 1) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Gate.update(threshold, [holdtime=1, attach=1, release=0.9995]) used wrongly.");
  }

  BGeneratorRef threshold = BGenerator::numberToGenerator(aContext->argument(0));

  BGeneratorRef holdtime;
  if (aContext->argumentCount() > 1) {
    holdtime = BGenerator::numberToGenerator(aContext->argument(1));
  } else {
    holdtime = new BNumberGenerator(1);
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

  if (!threshold || !holdtime || !attach || !release) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Gate.update(threshold, [holdtime=1, attach=1, release=0.9995]) used wrongly.");
  }

  BGateFilter* filter = static_cast<BGateFilter*>(aContext->thisObject().toQObject());

  filter->mThreshold = threshold;
  filter->mHoldtime = holdtime;
  filter->mAttach = attach;
  filter->mRelease = release;

  return QScriptValue();
}

QScriptValue
BGateFilter::engineFunction(QScriptContext* aContext,
                            QScriptEngine* aEngine)
{
  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);

  BGeneratorRef threshold = BGenerator::numberToGenerator(aContext->argument(0));
  if (aContext->argumentCount() > 0) {
    threshold = BGenerator::numberToGenerator(aContext->argument(0));
  } else {
    threshold = new BNumberGenerator(0.9);
  }

  BGeneratorRef holdtime;
  if (aContext->argumentCount() > 1) {
    holdtime = BGenerator::numberToGenerator(aContext->argument(1));
  } else {
    holdtime = new BNumberGenerator(1);
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

  if (!threshold || !holdtime || !attach || !release) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Gate(threshold, [holdtime=1, attach=1, release=0.9995]) used wrongly.");
  }

  BGateFilter* filter = new BGateFilter(engine->app(), threshold,
                                        holdtime, attach, release);

  QScriptValue object = filter->objFilter(engine);

  if (aContext->isCalledAsConstructor()) {
    aContext->setThisObject(object);
    return QScriptValue();
  }

  object.setPrototype(aContext->callee().property("prototype"));
  return object;
}

void
BGateFilter::engineProperties(QScriptEngine* aEngine, QScriptValue aValue)
{
  aValue.setProperty("threshold", aEngine->newFunction(thresholdFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);

  aValue.setProperty("holdtime", aEngine->newFunction(holdtimeFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);

  aValue.setProperty("attach", aEngine->newFunction(attachFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);

  aValue.setProperty("release", aEngine->newFunction(releaseFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);
}

METHOD_FUNCTION(BGateFilter, thresholdFunction, mThreshold, "Gate", "threshold")
METHOD_FUNCTION(BGateFilter, holdtimeFunction, mHoldtime, "Gate", "holdtime")
METHOD_FUNCTION(BGateFilter, attachFunction, mAttach, "Gate", "attach")
METHOD_FUNCTION(BGateFilter, releaseFunction, mRelease, "Gate", "release")

QString
BGateFilter::writeFilter()
{
  QString line;
  line.sprintf("Filter: %s - threshold %3.2f || holdtime %3.2f || attach %3.2f || release %3.2f",
               qPrintable(name()), mThreshold->get(), mHoldtime->get(),
               mAttach->get(), mRelease->get());
  return line;
}
