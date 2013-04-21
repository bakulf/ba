#include "bgenerator.h"
#include "bscriptengine.h"

#include "bnumbergenerator.h"
#include "bnoisegenerator.h"
#include "bsinewavegenerator.h"
#include "bcoswavegenerator.h"
#include "bphasorgenerator.h"

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

  // Sinewave
  QScriptValue sinewaveProto = aEngine->newObject();
  aEngine->globalObject().setProperty("Sinewave",
    aEngine->newFunction(BSinewaveGenerator::engineFunction, sinewaveProto));

  // Coswave
  QScriptValue coswaveProto = aEngine->newObject();
  aEngine->globalObject().setProperty("Coswave",
    aEngine->newFunction(BCoswaveGenerator::engineFunction, coswaveProto));

  // Phasor
  QScriptValue phasorProto = aEngine->newObject();
  aEngine->globalObject().setProperty("Phasor",
    aEngine->newFunction(BPhasorGenerator::engineFunction, phasorProto));
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

  // get
  obj.setProperty("get", aEngine->newFunction(funcGeneratorGet));

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

QScriptValue
BGenerator::funcGeneratorGet(QScriptContext* aContext,
                             QScriptEngine*)
{
  BGeneratorShell* shell = static_cast<BGeneratorShell*>(aContext->thisObject().toQObject());
  return QScriptValue(shell->get()->get());
}
