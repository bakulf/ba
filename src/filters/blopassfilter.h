#ifndef _BA_LOPASSFILTER_H_
#define _BA_LOPASSFILTER_H_

#include "bengine.h"
#include "bgenerator.h"
#include "maximilian.h"

class BLopassFilter : public BEngineFilter
{
  Q_OBJECT
  Q_DISABLE_COPY(BLopassFilter)

public:
  BLopassFilter(QObject* aParent,
                BGenerator* aCutOff);
  virtual ~BLopassFilter();

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
