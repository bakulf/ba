#include "bdistortionfilter.h"
#include "bscriptengine.h"
#include "bapplication.h"
#include "bgenerator.h"

BDistortionFilter::BDistortionFilter(QObject* aParent, Type aType,
                                     BGenerator* aShape)
: BEngineFilter(aParent, aType == ATAN ? QString("atandistortion") : QString("fastatandistortion"))
, mShape(aShape)
, mType(aType)
{
}

BDistortionFilter::~BDistortionFilter()
{
}

void
BDistortionFilter::output(double* aOutput)
{
  if (mType == ATAN) {
    for (int j=0; j < maxiSettings::channels; ++j) {
      aOutput[j] = mMaxi.atanDist(aOutput[j], mShape->get());
    }
  } else {
    for (int j=0; j < maxiSettings::channels; ++j) {
      aOutput[j] = mMaxi.fastAtanDist(aOutput[j], mShape->get());
    }
  }
}

QScriptValue
BDistortionFilter::updateFunction(QScriptContext* aContext,
                                  QScriptEngine*)
{
  BDistortionFilter* filter = static_cast<BDistortionFilter*>(aContext->thisObject().toQObject());

  if (aContext->argumentCount() < 1) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Distortion..update(shape) used wrongly.");
  }

  BGeneratorRef shape = BGenerator::numberToGenerator(aContext->argument(0));
  if (!shape) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Distortion..update(shape) used wrongly.");
  }

  filter->mShape = shape;
  return QScriptValue();
}

QScriptValue
BDistortionFilter::engineAtanFunction(QScriptContext* aContext,
                                      QScriptEngine* aEngine)
{
  return engineFunction(ATAN, aContext, aEngine);
}

QScriptValue
BDistortionFilter::engineFastAtanFunction(QScriptContext* aContext,
                                          QScriptEngine* aEngine)
{
  return engineFunction(FASTATAN, aContext, aEngine);
}

QScriptValue
BDistortionFilter::engineFunction(Type aType,
                                  QScriptContext* aContext,
                                  QScriptEngine* aEngine)
{
  if (aContext->argumentCount() < 1) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Distortion..update(shape) used wrongly.");
  }

  BGeneratorRef shape = BGenerator::numberToGenerator(aContext->argument(0));
  if (!shape) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "Distortion..update(shape) used wrongly.");
  }

  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);
  BDistortionFilter* filter = new BDistortionFilter(engine->app(),
                                    aType, shape);

  QScriptValue object = filter->objFilter(engine);

  if (aContext->isCalledAsConstructor()) {
    aContext->setThisObject(object);
    return QScriptValue();
  }

  object.setPrototype(aContext->callee().property("prototype"));
  return object;
}

void
BDistortionFilter::engineProperties(QScriptEngine* aEngine, QScriptValue aValue)
{
  aValue.setProperty("shape", aEngine->newFunction(shapeFunction),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);
}

METHOD_FUNCTION(BDistortionFilter, shapeFunction, mShape, "Distortion", "shape");

QString
BDistortionFilter::writeFilter()
{
  QString line;
  line.sprintf("Filter: %s - shape %3.2f",
               qPrintable(name()), mShape->get());
  return line;
}
