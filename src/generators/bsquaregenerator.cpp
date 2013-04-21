#include "bsquaregenerator.h"
#include "bscriptengine.h"
#include "bapplication.h"

BSquareGenerator::BSquareGenerator(BGenerator* aFrequency)
: BGenerator("square")
, mFrequency(aFrequency)
{
}

BSquareGenerator::~BSquareGenerator()
{
}

void
BSquareGenerator::generateInternal(quint64 aToken)
{
  mFrequency->generate(aToken);
  mValue = mMaxi.square(mFrequency->get());
}

double
BSquareGenerator::get()
{
  return mValue;
}

QScriptValue
BSquareGenerator::engineFunction(QScriptContext* aContext,
                                 QScriptEngine* aEngine)
{
  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);

  if (aContext->argumentCount() < 1) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Square(frequency) used wrongly.");
  }

  BGeneratorRef frequency = BGenerator::numberToGenerator(aContext->argument(0));
  if (!frequency) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Square(frequency) used wrongly.");
  }

  BSquareGenerator* generator = new BSquareGenerator(frequency);
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
BSquareGenerator::engineProperties(QScriptEngine* aEngine,
                                   QScriptValue aValue)
{
  aValue.setProperty("frequency", aEngine->newFunction(frequencyFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);
}

METHOD_FUNCTION(BSquareGenerator, BGeneratorShell, frequencyFunction,
                mFrequency, "Square", "frequency")
