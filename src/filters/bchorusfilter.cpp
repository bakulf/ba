#include "bchorusfilter.h"
#include "bscriptengine.h"
#include "bapplication.h"

BChorusFilter::BChorusFilter(QObject* aParent,
                             BGenerator* aDelay,
                             BGenerator* aFeedback,
                             BGenerator* aSpeed,
                             BGenerator* aDepth)
: BEngineFilter(aParent, QString("chorus"))
, mDelay(aDelay)
, mFeedback(aFeedback)
, mSpeed(aSpeed)
, mDepth(aDepth)
{
}

BChorusFilter::~BChorusFilter()
{
}

void
BChorusFilter::output(double* aOutput)
{
  for (int j=0; j < maxiSettings::channels; ++j) {
    aOutput[j] = mMaxi.chorus(aOutput[j], mDelay->get(),
                              mFeedback->get(), mSpeed->get(),
                              mDepth->get());
  }
}

QScriptValue
BChorusFilter::updateFunction(QScriptContext* aContext,
                              QScriptEngine*)
{
  if (aContext->argumentCount() < 4) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Chorus.update(delay, feedback, speed, depth) used wrongly.");
  }

  BGeneratorRef delay = BGenerator::numberToGenerator(aContext->argument(0));
  BGeneratorRef feedback = BGenerator::numberToGenerator(aContext->argument(1));
  BGeneratorRef speed = BGenerator::numberToGenerator(aContext->argument(2));
  BGeneratorRef depth = BGenerator::numberToGenerator(aContext->argument(3));

  if (!delay || !feedback || !speed || !depth) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Chorus.update(delay, feedback, speed, depth) used wrongly.");
  }

  BEngineFilterShell* shell = static_cast<BEngineFilterShell*>(aContext->thisObject().toQObject());
  BChorusFilterRef filter = static_cast<BChorusFilter*>(shell->get());

  filter->mDelay = delay;
  filter->mFeedback = feedback;
  filter->mSpeed = speed;
  filter->mDepth = depth;

  return QScriptValue();
}

QScriptValue
BChorusFilter::engineFunction(QScriptContext* aContext,
                              QScriptEngine* aEngine)
{
  if (aContext->argumentCount() < 4) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Chorus(delay, feedback, speed, depth) used wrongly.");
  }

  BGeneratorRef delay = BGenerator::numberToGenerator(aContext->argument(0));
  BGeneratorRef feedback = BGenerator::numberToGenerator(aContext->argument(1));
  BGeneratorRef speed = BGenerator::numberToGenerator(aContext->argument(2));
  BGeneratorRef depth = BGenerator::numberToGenerator(aContext->argument(3));

  if (!delay || !feedback || !speed || !depth) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Chorus(delay, feedback, speed, depth) used wrongly.");
  }

  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);
  BChorusFilterRef filter = new BChorusFilter(engine->app(), delay, feedback,
                                              speed, depth);

  QScriptValue object = filter->objFilter(engine);

  if (aContext->isCalledAsConstructor()) {
    aContext->setThisObject(object);
    return QScriptValue();
  }

  object.setPrototype(aContext->callee().property("prototype"));
  return object;
}

void
BChorusFilter::engineProperties(QScriptEngine* aEngine, QScriptValue aValue)
{
  aValue.setProperty("delay", aEngine->newFunction(delayFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);

  aValue.setProperty("feedback", aEngine->newFunction(feedbackFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);

  aValue.setProperty("speed", aEngine->newFunction(speedFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);

  aValue.setProperty("depth", aEngine->newFunction(depthFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);
}

METHOD_FUNCTION(BChorusFilter, BEngineFilterShell, delayFunction,
                mDelay, "Chorus", "delay")
METHOD_FUNCTION(BChorusFilter, BEngineFilterShell, feedbackFunction,
                mFeedback, "Chorus", "feedback")
METHOD_FUNCTION(BChorusFilter, BEngineFilterShell, speedFunction,
                mSpeed, "Chorus", "speed")
METHOD_FUNCTION(BChorusFilter, BEngineFilterShell, depthFunction,
                mDepth, "Chorus", "depth")

QString
BChorusFilter::writeFilter()
{
  QString line;
  line.sprintf("Filter: %s - delay %3.2f || feedback %3.2f || speed %3.2f || depth %3.2f",
               qPrintable(name()), mDelay->get(), mFeedback->get(),
               mSpeed->get(), mDepth->get());
  return line;
}
