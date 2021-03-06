#include "bgenerator.h"
#include "bscriptengine.h"

#include "bnumbergenerator.h"
#include "bnoisegenerator.h"
#include "bsinewavegenerator.h"
#include "bcoswavegenerator.h"
#include "bphasorgenerator.h"
#include "bsawgenerator.h"
#include "btrianglegenerator.h"
#include "bsquaregenerator.h"
#include "bpulsegenerator.h"
#include "bsinebufgenerator.h"
#include "bsinebuf4generator.h"
#include "bsawngenerator.h"
#include "brectgenerator.h"
#include "bopgenerator.h"

BGenerator::BGenerator(QString aName)
: mObjGenerator(QScriptValue::UndefinedValue)
, mName(aName) 
, mToken(0)
{
}

BGenerator::~BGenerator()
{
}

void
BGenerator::generate(quint64 aToken)
{
  if (mToken == aToken) {
    return;
  }

  mToken = aToken;
  generateInternal(aToken);
}

void
BGenerator::generatorFactory(QScriptEngine* aEngine)
{
  // Noise
  aEngine->globalObject().setProperty("Noise",
    aEngine->newFunction(BNoiseGenerator::engineFunction));

  // Sinewave
  aEngine->globalObject().setProperty("Sinewave",
    aEngine->newFunction(BSinewaveGenerator::engineFunction));

  // Coswave
  aEngine->globalObject().setProperty("Coswave",
    aEngine->newFunction(BCoswaveGenerator::engineFunction));

  // Phasor
  aEngine->globalObject().setProperty("Phasor",
    aEngine->newFunction(BPhasorGenerator::engineFunction));

  // Saw
  aEngine->globalObject().setProperty("Saw",
    aEngine->newFunction(BSawGenerator::engineFunction));

  // Triangle
  aEngine->globalObject().setProperty("Triangle",
    aEngine->newFunction(BTriangleGenerator::engineFunction));

  // Square
  aEngine->globalObject().setProperty("Square",
    aEngine->newFunction(BSquareGenerator::engineFunction));

  // Pulse
  aEngine->globalObject().setProperty("Pulse",
    aEngine->newFunction(BPulseGenerator::engineFunction));

  // Sinebuf
  aEngine->globalObject().setProperty("Sinebuf",
    aEngine->newFunction(BSinebufGenerator::engineFunction));

  // Sinebuf4
  aEngine->globalObject().setProperty("Sinebuf4",
    aEngine->newFunction(BSinebuf4Generator::engineFunction));

  // Sawn
  aEngine->globalObject().setProperty("Sawn",
    aEngine->newFunction(BSawnGenerator::engineFunction));

  // Rect
  aEngine->globalObject().setProperty("Rect",
    aEngine->newFunction(BRectGenerator::engineFunction));

  // Op
  QScriptValue op = aEngine->newFunction(BOpGenerator::engineFunction);
  BOpGenerator::createPrototype(aEngine, op);
  aEngine->globalObject().setProperty("Op", op);
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
