#ifndef _BA_HIPASSFILTER_H_
#define _BA_HIPASSFILTER_H_

#include "bengine.h"
#include "bgenerator.h"
#include "maximilian.h"

class BHipassFilter : public BEngineFilter
{
  Q_OBJECT
  Q_DISABLE_COPY(BHipassFilter)

public:
  BHipassFilter(QObject* aParent,
                BGenerator* aCutOff);
  virtual ~BHipassFilter();

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

private:
  BGeneratorRef mCutOff;

  maxiFilter mMaxi;
};

#endif
