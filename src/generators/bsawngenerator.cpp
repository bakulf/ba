#include "bsawngenerator.h"
#include "bscriptengine.h"
#include "bapplication.h"

BSawnGenerator::BSawnGenerator(BGenerator* aFrequency)
: BGenerator("sawn")
, mFrequency(aFrequency)
{
}

BSawnGenerator::~BSawnGenerator()
{
}

void
BSawnGenerator::generateInternal(quint64 aToken)
{
  mFrequency->generate(aToken);
  mValue = mMaxi.sawn(mFrequency->get());
}

double
BSawnGenerator::get()
{
  return mValue;
}

QScriptValue
BSawnGenerator::engineFunction(QScriptContext* aContext,
                               QScriptEngine* aEngine)
{
  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);

  if (aContext->argumentCount() < 1) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Sawn(frequency) used wrongly.");
  }

  BGeneratorRef frequency = BGenerator::numberToGenerator(aContext->argument(0));
  if (!frequency) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Sawn(frequency) used wrongly.");
  }

  BSawnGenerator* generator = new BSawnGenerator(frequency);
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
BSawnGenerator::engineProperties(QScriptEngine* aEngine,
                                 QScriptValue aValue)
{
  aValue.setProperty("frequency", aEngine->newFunction(frequencyFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);
}

METHOD_FUNCTION(BSawnGenerator, BGeneratorShell, frequencyFunction,
                mFrequency, "Sawn", "frequency")
