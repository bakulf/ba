#include "bnoisegenerator.h"
#include "bscriptengine.h"
#include "bapplication.h"

#include <QTime>

bool BNoiseGenerator::gInitialized = false;

BNoiseGenerator::BNoiseGenerator()
: BGenerator("noise")
{
}

BNoiseGenerator::~BNoiseGenerator()
{
}

void
BNoiseGenerator::generate()
{
  if (!gInitialized) {
    gInitialized = true;

    QTime midnight(0, 0, 0);
    qsrand(midnight.secsTo(QTime::currentTime()));
  }
}

double
BNoiseGenerator::get()
{
  return (double)qrand() / (double)RAND_MAX;
}

QScriptValue
BNoiseGenerator::engineFunction(QScriptContext* aContext,
                                QScriptEngine* aEngine)
{
  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);

  BNoiseGenerator* generator = new BNoiseGenerator();
  engine->app()->registerGenerator(generator);

  QScriptValue object = generator->objGenerator(engine);

  if (aContext->isCalledAsConstructor()) {
    aContext->setThisObject(object);
    return QScriptValue();
  }

  object.setPrototype(aContext->callee().property("prototype"));
  return object;
}
