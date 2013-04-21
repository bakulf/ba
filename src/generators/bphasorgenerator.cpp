#include "bphasorgenerator.h"
#include "bscriptengine.h"
#include "bapplication.h"

BPhasorGenerator::BPhasorGenerator(BGenerator* aFrequency,
                                   BGenerator* aStartphase,
                                   BGenerator* aEndphase)
: BGenerator("phasor")
, mFrequency(aFrequency)
, mStartphase(aStartphase)
, mEndphase(aEndphase)
{
}

BPhasorGenerator::~BPhasorGenerator()
{
}

void
BPhasorGenerator::generate()
{
  if (!mStartphase || !mEndphase) {
    mValue = mMaxi.phasor(mFrequency->get());
  } else {
    mValue = mMaxi.phasor(mFrequency->get(), mStartphase->get(), mEndphase->get());
  }
}

double
BPhasorGenerator::get()
{
  return mValue;
}

QScriptValue
BPhasorGenerator::engineFunction(QScriptContext* aContext,
                                 QScriptEngine* aEngine)
{
  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);

  if (aContext->argumentCount() < 1) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Phasor(frequency, [startphase, endphase]) used wrongly.");
  }

  BGeneratorRef frequency = BGenerator::numberToGenerator(aContext->argument(0));
  if (!frequency) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Phasor(frequency, [startphase, endphase]) used wrongly.");
  }

  BGeneratorRef startphase;
  BGeneratorRef endphase;

  if (aContext->argumentCount() > 1) {
    startphase = BGenerator::numberToGenerator(aContext->argument(1));
    endphase = BGenerator::numberToGenerator(aContext->argument(2));

    if (aContext->argumentCount() < 2 || !startphase || !endphase) {
      return aContext->throwError(QScriptContext::SyntaxError,
                                  "Phasor(frequency, [startphase, endphase]) used wrongly.");
    }
  }

  BPhasorGenerator* generator = new BPhasorGenerator(frequency, startphase, endphase);
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
BPhasorGenerator::engineProperties(QScriptEngine* aEngine,
                                    QScriptValue aValue)
{
  aValue.setProperty("frequency", aEngine->newFunction(frequencyFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);

  aValue.setProperty("startphase", aEngine->newFunction(startphaseFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);

  aValue.setProperty("endphase", aEngine->newFunction(endphaseFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);
}

METHOD_FUNCTION(BPhasorGenerator, BGeneratorShell, frequencyFunction,
                mFrequency, "Phasor", "frequency")

METHOD_FUNCTION(BPhasorGenerator, BGeneratorShell, startphaseFunction,
                mStartphase, "Phasor", "startphase")

METHOD_FUNCTION(BPhasorGenerator, BGeneratorShell, endphaseFunction,
                mEndphase, "Phasor", "endphase")
