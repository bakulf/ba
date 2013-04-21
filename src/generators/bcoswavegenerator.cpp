#include "bcoswavegenerator.h"
#include "bscriptengine.h"
#include "bapplication.h"

BCoswaveGenerator::BCoswaveGenerator(BGenerator* aFrequency)
: BGenerator("coswave")
, mFrequency(aFrequency)
{
}

BCoswaveGenerator::~BCoswaveGenerator()
{
}

void
BCoswaveGenerator::generate()
{
  mValue = mMaxi.coswave(mFrequency->get());
}

double
BCoswaveGenerator::get()
{
  return mValue;
}

QScriptValue
BCoswaveGenerator::engineFunction(QScriptContext* aContext,
                                 QScriptEngine* aEngine)
{
  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);

  if (aContext->argumentCount() < 1) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Coswave(frequency) used wrongly.");
  }

  BGeneratorRef frequency = BGenerator::numberToGenerator(aContext->argument(0));
  if (!frequency) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Coswave(frequency) used wrongly.");
  }

  BCoswaveGenerator* generator = new BCoswaveGenerator(frequency);
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
BCoswaveGenerator::engineProperties(QScriptEngine* aEngine,
                                    QScriptValue aValue)
{
  aValue.setProperty("frequency", aEngine->newFunction(frequencyFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);
}

METHOD_FUNCTION(BCoswaveGenerator, BGeneratorShell, frequencyFunction,
                mFrequency, "Coswave", "frequency")
