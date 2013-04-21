#include "bnumbergenerator.h"
#include "bscriptengine.h"

#include <assert.h>

BNumberGenerator::BNumberGenerator(double aNumber)
: BGenerator("number")
, mNumber(aNumber)
{
}

BNumberGenerator::~BNumberGenerator()
{
}

QScriptValue
BNumberGenerator::engineFunction(QScriptContext*,
                                 QScriptEngine*)
{
  assert("This method should not be called");
  return QScriptValue();
}

void
BNumberGenerator::makeObjGenerator(BScriptEngine*)
{
  mObjGenerator = QScriptValue(mNumber);
}
