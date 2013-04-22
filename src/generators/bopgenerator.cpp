#include "bopgenerator.h"
#include "bnumbergenerator.h"
#include "bscriptengine.h"
#include "bapplication.h"

BOpGenerator::OpType BOpGenerator::OpTypes[] = {
{ 0, "SUM", BOpGenerator::sumOp },
{ 1, "DIFF", BOpGenerator::diffOp },
{ 2, "PROD", BOpGenerator::prodOp },
{ 3, "DIV", BOpGenerator::divOp },
{ 4, "MOD", BOpGenerator::modOp },
{ 5, "AVG", BOpGenerator::avgOp },
{ -1, NULL, NULL }
};

BOpGenerator::BOpGenerator(OpType& aType, QList<BGeneratorRef>& aInputs)
: BGenerator("op")
, mInputs(aInputs)
, mType(aType)
{
  mValue = mType.generate(mInputs);
}

BOpGenerator::~BOpGenerator()
{
}

void
BOpGenerator::generateInternal(quint64 aToken)
{
  foreach(const BGeneratorRef& input, mInputs) {
    input->generate(aToken);
  }

  mValue = mType.generate(mInputs);
}

double
BOpGenerator::get()
{
  return mValue;
}

QScriptValue
BOpGenerator::engineFunction(QScriptContext* aContext,
                             QScriptEngine* aEngine)
{
  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);

  if (aContext->argumentCount() < 1) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Op(type [, input, ... inputs]) used wrongly.");
  }

  qint32 type = aContext->argument(0).toInt32();

  OpType *opType = NULL;

  for (int i = 0; OpTypes[i].mId != -1; ++i) {
    if (type == OpTypes[i].mId) {
      opType = &OpTypes[i];
      break;
    }
  }

  if (!opType) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Op(type [, input, ... inputs]) used wrongly.");
  }

  QList<BGeneratorRef> inputs;

  for (int i = 1; i < aContext->argumentCount(); ++i) {
    BGeneratorRef generator = BGenerator::numberToGenerator(aContext->argument(i));
    if (!generator) {
      return aContext->throwError(QScriptContext::SyntaxError,
                                  "Op(type [, input, ... inputs]) used wrongly.");
    }

    inputs << generator;
  }

  BOpGenerator* generator = new BOpGenerator(*opType, inputs);
  engine->app()->registerGenerator(generator);

  QScriptValue object = generator->objGenerator(engine);

  if (aContext->isCalledAsConstructor()) {
    aContext->setThisObject(object);
    return QScriptValue();
  }

  return object;
}

void
BOpGenerator::createPrototype(QScriptEngine* aEngine,
                              QScriptValue aValue)
{
  for (int i = 0; OpTypes[i].mId != -1; ++i) {
    QScriptValue obj = aEngine->newFunction(typesFunction);
    obj.setData(OpTypes[i].mId);

    aValue.setProperty(OpTypes[i].mName, obj,
                       QScriptValue::PropertyGetter | QScriptValue::ReadOnly);
    aValue.setData(i);
  }
}

void
BOpGenerator::engineProperties(QScriptEngine* aEngine,
                               QScriptValue aValue)
{
  aValue.setProperty("type", aEngine->newFunction(typeFunction),
    QScriptValue::PropertyGetter | QScriptValue::ReadOnly);

  for (int i = 0, len = mInputs.length(); i < len; ++i) {
    QScriptValue obj = aEngine->newFunction(inputFunction);
    obj.setData(i);

    aValue.setProperty(i, obj,
                       QScriptValue::PropertyGetter | QScriptValue::PropertySetter);
  }
}

QScriptValue
BOpGenerator::typeFunction(QScriptContext* aContext,
                           QScriptEngine*)
{
  BGeneratorShell* shell =
    static_cast<BGeneratorShell*>(aContext->thisObject().toQObject());
  BOpGenerator* generator = static_cast<BOpGenerator*>(shell->get());

  return QScriptValue(generator->mType.mId);
}

QScriptValue
BOpGenerator::typesFunction(QScriptContext* aContext,
                            QScriptEngine*)
{
  QScriptValue value = aContext->callee();
  int id = value.data().toInt32();
  return QScriptValue(id);
}

QScriptValue
BOpGenerator::inputFunction(QScriptContext* aContext,
                            QScriptEngine* aEngine)
{
  QScriptValue value = aContext->callee();
  int id = value.data().toInt32();

  BGeneratorShell* shell = static_cast<BGeneratorShell*>(aContext->thisObject().toQObject());
  BGeneratorRef generator = static_cast<BGenerator*>(shell->get());
  BOpGenerator* op = static_cast<BOpGenerator*>(generator.get());

  if (id < 0 || id > op->mInputs.length()) {
    return QScriptValue();
  }

  if (aContext->argumentCount() > 0) {
    BGeneratorRef generator = BGenerator::numberToGenerator(aContext->argument(0));
    if (!generator) {
      return aContext->throwError(QScriptContext::SyntaxError,
                                  "Wrong access to Op object.");
    }

    op->mInputs.replace(id, generator);
    op->mValue = op->mType.generate(op->mInputs);
  }

  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);
  return QScriptValue(op->mInputs[id]->objGenerator(engine));
}

double
BOpGenerator::sumOp(QList<BGeneratorRef>& aInputs)
{
  double value = 0;
  
  foreach(const BGeneratorRef& input, aInputs) {
    value += input->get();
  }

  return value;
}

double
BOpGenerator::diffOp(QList<BGeneratorRef>& aInputs)
{
  if (aInputs.length() == 0) {
    return 0;
  }

  double value = aInputs[0]->get();
  for (int i = 1; i < aInputs.length(); ++i) {
    value -= aInputs[i]->get();
  }

  return value;
}

double
BOpGenerator::prodOp(QList<BGeneratorRef>& aInputs)
{
  if (aInputs.length() == 0) {
    return 0;
  }

  double value = aInputs[0]->get();
  for (int i = 1; i < aInputs.length(); ++i) {
    value *= aInputs[i]->get();
  }

  return value;
}

double
BOpGenerator::divOp(QList<BGeneratorRef>& aInputs)
{
  if (aInputs.length() == 0) {
    return 0;
  }

  double value = aInputs[0]->get();
  for (int i = 1; i < aInputs.length(); ++i) {
    double tmp = aInputs[i]->get();
    if (!tmp) {
      value = 0;
      break;
    }

    value /= tmp;
  }

  return value;
}

double
BOpGenerator::modOp(QList<BGeneratorRef>& aInputs)
{
  if (aInputs.length() == 0) {
    return 0;
  }

  int value = (int)aInputs[0]->get();
  for (int i = 1; i < aInputs.length(); ++i) {
    int tmp = (int)aInputs[i]->get();
    if (!tmp) {
      value = 0;
      break;
    }

    value %= tmp;
  }

  return value;
}
double
BOpGenerator::avgOp(QList<BGeneratorRef>& aInputs)
{
  double value = 0;
  
  foreach(const BGeneratorRef& input, aInputs) {
    value += input->get();
  }

  return value / aInputs.length();
}
