#ifndef _BA_GATEFILTER_H_
#define _BA_GATEFILTER_H_

#include "bengine.h"
#include "bgenerator.h"
#include "maximilian.h"

class BGateFilter : public BEngineFilter
{
  Q_OBJECT
  Q_DISABLE_COPY(BGateFilter)

public:
  BGateFilter(QObject* aParent,
              BGenerator* aThreshold,
              BGenerator* aHoldTime,
              BGenerator* aAttach,
              BGenerator* aRelease);
  virtual ~BGateFilter();

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

  static QScriptValue thresholdFunction(QScriptContext* aContext,
                                        QScriptEngine* aEngine);

  static QScriptValue holdtimeFunction(QScriptContext* aContext,
                                       QScriptEngine* aEngine);

  static QScriptValue attachFunction(QScriptContext* aContext,
                                     QScriptEngine* aEngine);

  static QScriptValue releaseFunction(QScriptContext* aContext,
                                      QScriptEngine* aEngine);

private:
  BGeneratorRef mThreshold;
  BGeneratorRef mHoldtime;
  BGeneratorRef mAttach;
  BGeneratorRef mRelease;

  maxiDyn mMaxi;
};

#endif
