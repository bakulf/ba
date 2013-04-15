#ifndef _BA_LORESFILTER_H_
#define _BA_LORESFILTER_H_

#include "bengine.h"
#include "maximilian.h"

class BLoresFilter : public BEngineFilter
{
  Q_OBJECT
  Q_DISABLE_COPY(BLoresFilter)

public:
  BLoresFilter(QObject* aParent,
                double aCutOff,
                double aResonance);
  virtual ~BLoresFilter();

  virtual QString writeFilter();

  virtual void output(double* aOutput);

  virtual QScriptEngine::FunctionSignature engineUpdateFunction()
  {
    return updateFunction;
  }

  virtual void engineProperties(QScriptEngine* aEngine,
                                QScriptValue aValue);

  static QScriptValue engineFunction(QScriptContext* aContext,
                                     QScriptEngine* aEngine);

private:
  static QScriptValue updateFunction(QScriptContext* aContext,
                                     QScriptEngine* aEngine);

  static QScriptValue cutOffFunction(QScriptContext* aContext,
                                     QScriptEngine* aEngine);

  static QScriptValue resonanceFunction(QScriptContext* aContext,
                                        QScriptEngine* aEngine);

private:
  double mCutOff;
  double mResonance;

  maxiFilter mMaxi;
};

#endif
