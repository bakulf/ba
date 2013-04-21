#include "bsinebufgenerator.h"
#include "bscriptengine.h"
#include "bapplication.h"

BSinebufGenerator::BSinebufGenerator(BGenerator* aFrequency)
: BGenerator("sinebuf")
, mFrequency(aFrequency)
{
}

BSinebufGenerator::~BSinebufGenerator()
{
}

void
BSinebufGenerator::generateInternal(quint64 aToken)
{
  mFrequency->generate(aToken);
  mValue = mMaxi.sinebuf(mFrequency->get());
}

double
BSinebufGenerator::get()
{
  return mValue;
}

QScriptValue
BSinebufGenerator::engineFunction(QScriptContext* aContext,
                                  QScriptEngine* aEngine)
{
  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);

  if (aContext->argumentCount() < 1) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Sinebuf(frequency) used wrongly.");
  }

  BGeneratorRef frequency = BGenerator::numberToGenerator(aContext->argument(0));
  if (!frequency) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Sinebuf(frequency) used wrongly.");
  }

  BSinebufGenerator* generator = new BSinebufGenerator(frequency);
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
BSinebufGenerator::engineProperties(QScriptEngine* aEngine,
                                    QScriptValue aValue)
{
  aValue.setProperty("frequency", aEngine->newFunction(frequencyFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);
}

METHOD_FUNCTION(BSinebufGenerator, BGeneratorShell, frequencyFunction,
                mFrequency, "Sinebuf", "frequency")
