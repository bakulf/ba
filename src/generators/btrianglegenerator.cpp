#include "btrianglegenerator.h"
#include "bscriptengine.h"
#include "bapplication.h"

BTriangleGenerator::BTriangleGenerator(BGenerator* aFrequency)
: BGenerator("triangle")
, mFrequency(aFrequency)
{
}

BTriangleGenerator::~BTriangleGenerator()
{
}

void
BTriangleGenerator::generateInternal(quint64 aToken)
{
  mFrequency->generate(aToken);
  mValue = mMaxi.triangle(mFrequency->get());
}

double
BTriangleGenerator::get()
{
  return mValue;
}

QScriptValue
BTriangleGenerator::engineFunction(QScriptContext* aContext,
                                   QScriptEngine* aEngine)
{
  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);

  if (aContext->argumentCount() < 1) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Triangle(frequency) used wrongly.");
  }

  BGeneratorRef frequency = BGenerator::numberToGenerator(aContext->argument(0));
  if (!frequency) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Triangle(frequency) used wrongly.");
  }

  BTriangleGenerator* generator = new BTriangleGenerator(frequency);
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
BTriangleGenerator::engineProperties(QScriptEngine* aEngine,
                                     QScriptValue aValue)
{
  aValue.setProperty("frequency", aEngine->newFunction(frequencyFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);
}

METHOD_FUNCTION(BTriangleGenerator, BGeneratorShell, frequencyFunction,
                mFrequency, "Triangle", "frequency")
