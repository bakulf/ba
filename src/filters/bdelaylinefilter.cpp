#include "bdelaylinefilter.h"
#include "bscriptengine.h"
#include "bapplication.h"
#include "bnumbergenerator.h"

BDelaylineFilter::BDelaylineFilter(QObject* aParent,
                                   BGenerator* aSize,
                                   BGenerator* aFeedback,
                                   BGenerator* aPosition)
: BEngineFilter(aParent, QString("delayline"))
, mSize(aSize)
, mFeedback(aFeedback)
, mPosition(aPosition)
{
}

BDelaylineFilter::~BDelaylineFilter()
{
}

void
BDelaylineFilter::output(double* aOutput)
{
  for (int j=0; j < maxiSettings::channels; ++j) {
    aOutput[j] = mMaxi.dl(aOutput[j], mSize->get(), mFeedback->get(),
                          mPosition->get());
  }
}

QScriptValue
BDelaylineFilter::updateFunction(QScriptContext* aContext,
                                 QScriptEngine*)
{
  if (aContext->argumentCount() < 2) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Delayline.update(size, feedback[, position]) used wrongly.");
  }

  BGeneratorRef size = BGenerator::numberToGenerator(aContext->argument(0));
  BGeneratorRef feedback = BGenerator::numberToGenerator(aContext->argument(1));

  BGeneratorRef position;
  if (aContext->argumentCount() > 2) {
    position = BGenerator::numberToGenerator(aContext->argument(2));
  } else {
    position = new BNumberGenerator(0);
  }


  if (!size || !feedback || !position) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Delayline.update(size, feedback[, position]) used wrongly.");
  }

  BDelaylineFilter* filter = static_cast<BDelaylineFilter*>(aContext->thisObject().toQObject());

  filter->mSize = size;
  filter->mFeedback = feedback;
  filter->mPosition = position;

  return QScriptValue();
}

QScriptValue
BDelaylineFilter::engineFunction(QScriptContext* aContext,
                                 QScriptEngine* aEngine)
{
  if (aContext->argumentCount() < 2) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Delayline(size, feedback[, position]) used wrongly.");
  }

  BGeneratorRef size = BGenerator::numberToGenerator(aContext->argument(0));
  BGeneratorRef feedback = BGenerator::numberToGenerator(aContext->argument(1));

  BGeneratorRef position;
  if (aContext->argumentCount() > 2) {
    position = BGenerator::numberToGenerator(aContext->argument(2));
  } else {
    position = new BNumberGenerator(0);
  }


  if (!size || !feedback || !position) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Delayline(size, feedback[, position]) used wrongly.");
  }

  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);
  BDelaylineFilter* filter = new BDelaylineFilter(engine->app(), size,
                                                  feedback, position);

  QScriptValue object = filter->objFilter(engine);

  if (aContext->isCalledAsConstructor()) {
    aContext->setThisObject(object);
    return QScriptValue();
  }

  object.setPrototype(aContext->callee().property("prototype"));
  return object;
}

void
BDelaylineFilter::engineProperties(QScriptEngine* aEngine, QScriptValue aValue)
{
  aValue.setProperty("size", aEngine->newFunction(sizeFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);

  aValue.setProperty("feedback", aEngine->newFunction(feedbackFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);

  aValue.setProperty("position", aEngine->newFunction(positionFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);
}

METHOD_FUNCTION(BDelaylineFilter, sizeFunction, mSize, "Delayline", "size");
METHOD_FUNCTION(BDelaylineFilter, feedbackFunction, mFeedback, "Delayline", "feedback");
METHOD_FUNCTION(BDelaylineFilter, positionFunction, mPosition, "Delayline", "position");

QString
BDelaylineFilter::writeFilter()
{
  QString line;
  line.sprintf("Filter: %s - size %3.2f || feedback %3.2f || position %3.2f",
               qPrintable(name()), mSize->get(), mFeedback->get(), mPosition->get());
  return line;
}
