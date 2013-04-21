#include "bgenerator.h"
#include "bscriptengine.h"

#include "bnoisegenerator.h"
#include "bnumbergenerator.h"

BGenerator::BGenerator(QString aName)
: mObjGenerator(QScriptValue::UndefinedValue)
, mName(aName) 
, mRef(0)
{
}

BGenerator::~BGenerator()
{
}

void
BGenerator::addRef()
{
  ++mRef;
}

void
BGenerator::release()
{
  --mRef;
  if (!mRef) {
    deleteLater();
  }
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

  return static_cast<BGenerator*>(aValue.toQObject());
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

  QScriptValue obj = aEngine->newQObject(this);

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
  BGenerator* generator = static_cast<BGenerator*>(aContext->thisObject().toQObject());
  return QScriptValue(generator->name());
}
