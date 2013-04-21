#include "brectgenerator.h"
#include "bnumbergenerator.h"
#include "bscriptengine.h"
#include "bapplication.h"

BRectGenerator::BRectGenerator(BGenerator* aFrequency, BGenerator* aDuty)
: BGenerator("rect")
, mFrequency(aFrequency)
, mDuty(aDuty)
{
}

BRectGenerator::~BRectGenerator()
{
}

void
BRectGenerator::generateInternal(quint64 aToken)
{
  mFrequency->generate(aToken);
  mDuty->generate(aToken);
  mValue = mMaxi.rect(mFrequency->get());
}

double
BRectGenerator::get()
{
  return mValue;
}

QScriptValue
BRectGenerator::engineFunction(QScriptContext* aContext,
                               QScriptEngine* aEngine)
{
  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);

  if (aContext->argumentCount() < 1) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Rect(frequency [, duty=0.5]) used wrongly.");
  }

  BGeneratorRef frequency = BGenerator::numberToGenerator(aContext->argument(0));
  BGeneratorRef duty;
  if (aContext->argumentCount() > 1) {
    duty = BGenerator::numberToGenerator(aContext->argument(1));
  } else {
    duty = new BNumberGenerator(0.5);
  }

  if (!frequency || !duty) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Rect(frequency [, duty=0.5]) used wrongly.");
  }

  BRectGenerator* generator = new BRectGenerator(frequency, duty);
  engine->app()->registerGenerator(generator);

  QScriptValue object = generator->objGenerator(engine);

  if (aContext->isCalledAsConstructor()) {
    aContext->setThisObject(object);
    return QScriptValue();
  }

  object.setPrototype(aContext->callee().property("prototype"));
  return object;
}

void
BRectGenerator::engineProperties(QScriptEngine* aEngine,
                                 QScriptValue aValue)
{
  aValue.setProperty("frequency", aEngine->newFunction(frequencyFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);

  aValue.setProperty("duty", aEngine->newFunction(dutyFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);
}

METHOD_FUNCTION(BRectGenerator, BGeneratorShell, frequencyFunction,
                mFrequency, "Rect", "frequency")

METHOD_FUNCTION(BRectGenerator, BGeneratorShell, dutyFunction,
                mDuty, "Rect", "duty")
