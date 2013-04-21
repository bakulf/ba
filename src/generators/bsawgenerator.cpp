#include "bsawgenerator.h"
#include "bscriptengine.h"
#include "bapplication.h"

BSawGenerator::BSawGenerator(BGenerator* aFrequency)
: BGenerator("saw")
, mFrequency(aFrequency)
{
}

BSawGenerator::~BSawGenerator()
{
}

void
BSawGenerator::generateInternal(quint64 aToken)
{
  mFrequency->generate(aToken);
  mValue = mMaxi.saw(mFrequency->get());
}

double
BSawGenerator::get()
{
  return mValue;
}

QScriptValue
BSawGenerator::engineFunction(QScriptContext* aContext,
                              QScriptEngine* aEngine)
{
  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);

  if (aContext->argumentCount() < 1) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Saw(frequency) used wrongly.");
  }

  BGeneratorRef frequency = BGenerator::numberToGenerator(aContext->argument(0));
  if (!frequency) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Saw(frequency) used wrongly.");
  }

  BSawGenerator* generator = new BSawGenerator(frequency);
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
BSawGenerator::engineProperties(QScriptEngine* aEngine,
                                QScriptValue aValue)
{
  aValue.setProperty("frequency", aEngine->newFunction(frequencyFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);
}

METHOD_FUNCTION(BSawGenerator, BGeneratorShell, frequencyFunction,
                mFrequency, "Saw", "frequency")
