#include "bpulsegenerator.h"
#include "bscriptengine.h"
#include "bapplication.h"

BPulseGenerator::BPulseGenerator(BGenerator* aFrequency,
                                 BGenerator* aDuty)
: BGenerator("pulse")
, mFrequency(aFrequency)
, mDuty(aDuty)
{
}

BPulseGenerator::~BPulseGenerator()
{
}

void
BPulseGenerator::generateInternal(quint64 aToken)
{
  mFrequency->generate(aToken);
  mDuty->generate(aToken);
  mValue = mMaxi.pulse(mFrequency->get(), mDuty->get());
}

double
BPulseGenerator::get()
{
  return mValue;
}

QScriptValue
BPulseGenerator::engineFunction(QScriptContext* aContext,
                                 QScriptEngine* aEngine)
{
  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);

  if (aContext->argumentCount() < 2) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Pulse(frequency, duty) used wrongly.");
  }

  BGeneratorRef frequency = BGenerator::numberToGenerator(aContext->argument(0));
  BGeneratorRef duty = BGenerator::numberToGenerator(aContext->argument(1));
  if (!frequency || !duty) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Pulse(frequency, duty) used wrongly.");
  }

  BPulseGenerator* generator = new BPulseGenerator(frequency, duty);
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
BPulseGenerator::engineProperties(QScriptEngine* aEngine,
                                   QScriptValue aValue)
{
  aValue.setProperty("frequency", aEngine->newFunction(frequencyFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);

  aValue.setProperty("duty", aEngine->newFunction(dutyFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);
}

METHOD_FUNCTION(BPulseGenerator, BGeneratorShell, frequencyFunction,
                mFrequency, "Pulse", "frequency")

METHOD_FUNCTION(BPulseGenerator, BGeneratorShell, dutyFunction,
                mDuty, "Pulse", "duty")
