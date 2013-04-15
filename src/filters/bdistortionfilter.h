#ifndef _BA_DISTORTIONFILTER_H_
#define _BA_DISTORTIONFILTER_H_

#include "bengine.h"
#include "maximilian.h"

class BDistortionFilter : public BEngineFilter
{
  Q_OBJECT
  Q_DISABLE_COPY(BDistortionFilter)

  enum Type {
    ATAN,
    FASTATAN
  };

public:
  BDistortionFilter(QObject* aParent,
                    Type aType,
                    double aShape);
  virtual ~BDistortionFilter();

  virtual QString writeFilter();

  virtual void output(double* aOutput);

  virtual QScriptEngine::FunctionSignature engineUpdateFunction()
  {
    return updateFunction;
  }

  virtual void engineProperties(QScriptEngine* aEngine,
                                QScriptValue aValue);

  static QScriptValue engineAtanFunction(QScriptContext* aContext,
                                         QScriptEngine* aEngine);
  static QScriptValue engineFastAtanFunction(QScriptContext* aContext,
                                             QScriptEngine* aEngine);

private:
  static QScriptValue engineFunction(Type aType,
                                     QScriptContext* aContext,
                                     QScriptEngine* aEngine);

  static QScriptValue updateFunction(QScriptContext* aContext,
                                     QScriptEngine* aEngine);

  static QScriptValue shapeFunction(QScriptContext* aContext,
                                    QScriptEngine* aEngine);

private:
  double mShape;
  Type mType;

  maxiDistortion mMaxi;
};

#endif
