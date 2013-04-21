#include "bsinewavegenerator.h"
#include "bscriptengine.h"
#include "bapplication.h"

BSinewaveGenerator::BSinewaveGenerator(BGenerator* aFrequency)
: BGenerator("sinewave")
, mFrequency(aFrequency)
{
}

BSinewaveGenerator::~BSinewaveGenerator()
{
}

void
BSinewaveGenerator::generateInternal(quint64 aToken)
{
  mFrequency->generate(aToken);
  mValue = mMaxi.sinewave(mFrequency->get());
}

double
BSinewaveGenerator::get()
{
  return mValue;
}

QScriptValue
BSinewaveGenerator::engineFunction(QScriptContext* aContext,
                                   QScriptEngine* aEngine)
{
  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);

  if (aContext->argumentCount() < 1) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Sinewave(frequency) used wrongly.");
  }

  BGeneratorRef frequency = BGenerator::numberToGenerator(aContext->argument(0));
  if (!frequency) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Sinewave(frequency) used wrongly.");
  }

  BSinewaveGenerator* generator = new BSinewaveGenerator(frequency);
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
BSinewaveGenerator::engineProperties(QScriptEngine* aEngine,
                                     QScriptValue aValue)
{
  aValue.setProperty("frequency", aEngine->newFunction(frequencyFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);
}

METHOD_FUNCTION(BSinewaveGenerator, BGeneratorShell, frequencyFunction,
                mFrequency, "Sinewave", "frequency")
