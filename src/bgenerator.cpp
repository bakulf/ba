#include "bgenerator.h"
#include "bscriptengine.h"

#include "bnoisegenerator.h"
#include "bnumbergenerator.h"

BGenerator::BGenerator(QString aName)
: mObjGenerator(QScriptValue::UndefinedValue)
, mName(aName) 
{
}

BGenerator::~BGenerator()
{
}

void
BGenerator::generatorFactory(QScriptEngine* aEngine)
{
  // Noise
  QScriptValue noiseProto = aEngine->newObject();
  aEngine->globalObject().setProperty("Noise",
    aEngine->newFunction(BNoiseGenerator::engineFunction, noiseProto));
}

BGenerator*
BGenerator::numberToGenerator(QScriptValue aValue)
{
  if (aValue.isNumber()) {
    return new BNumberGenerator(aValue.toNumber());
  }

  BGeneratorShell* shell = static_cast<BGeneratorShell*>(aValue.toQObject());
  return shell ? shell->get() : NULL;
}

QScriptValue
BGenerator::objGenerator(BScriptEngine* aEngine)
{
  if (mObjGenerator.isUndefined()) {
    makeObjGenerator(aEngine);
  }

  return mObjGenerator;
}

void
BGenerator::makeObjGenerator(BScriptEngine* aEngine)
{
  if (!mObjGenerator.isUndefined()) {
    return;
  }

  BGeneratorShell* shell = new BGeneratorShell(this);
  QScriptValue obj = aEngine->newQObject(shell, QScriptEngine::ScriptOwnership);

  // name
  obj.setProperty("name", aEngine->newFunction(funcGeneratorName),
    QScriptValue::PropertyGetter | QScriptValue::ReadOnly);

  // Other properties
  engineProperties(aEngine, obj);

  mObjGenerator = obj;
}

QScriptValue
BGenerator::funcGeneratorName(QScriptContext* aContext,
                              QScriptEngine*)
{
  BGeneratorShell* shell = static_cast<BGeneratorShell*>(aContext->thisObject().toQObject());
  return QScriptValue(shell->get()->name());
}
