#include "bsinebuf4generator.h"
#include "bscriptengine.h"
#include "bapplication.h"

BSinebuf4Generator::BSinebuf4Generator(BGenerator* aFrequency)
: BGenerator("sinebuf4")
, mFrequency(aFrequency)
{
}

BSinebuf4Generator::~BSinebuf4Generator()
{
}

void
BSinebuf4Generator::generateInternal(quint64 aToken)
{
  mFrequency->generate(aToken);
  mValue = mMaxi.sinebuf4(mFrequency->get());
}

double
BSinebuf4Generator::get()
{
  return mValue;
}

QScriptValue
BSinebuf4Generator::engineFunction(QScriptContext* aContext,
                                   QScriptEngine* aEngine)
{
  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);

  if (aContext->argumentCount() < 1) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Sinebuf4(frequency) used wrongly.");
  }

  BGeneratorRef frequency = BGenerator::numberToGenerator(aContext->argument(0));
  if (!frequency) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Sinebuf4(frequency) used wrongly.");
  }

  BSinebuf4Generator* generator = new BSinebuf4Generator(frequency);
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
BSinebuf4Generator::engineProperties(QScriptEngine* aEngine,
                                     QScriptValue aValue)
{
  aValue.setProperty("frequency", aEngine->newFunction(frequencyFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);
}

METHOD_FUNCTION(BSinebuf4Generator, BGeneratorShell, frequencyFunction,
                mFrequency, "Sinebuf4", "frequency")
