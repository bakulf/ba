#ifndef _BA_FLANGERFILTER_H_
#define _BA_FLANGERFILTER_H_

#include "bengine.h"
#include "bgenerator.h"
#include "maximilian.h"

class BFlangerFilter : public BEngineFilter
{
  Q_OBJECT
  Q_DISABLE_COPY(BFlangerFilter)

public:
  BFlangerFilter(QObject* aParent,
                 BGenerator* aDelay,
                 BGenerator* aFeedback,
                 BGenerator* aSpeed,
                 BGenerator* aDepth);
  virtual ~BFlangerFilter();

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

  static QScriptValue delayFunction(QScriptContext* aContext,
                                    QScriptEngine* aEngine);

  static QScriptValue feedbackFunction(QScriptContext* aContext,
                                       QScriptEngine* aEngine);

  static QScriptValue speedFunction(QScriptContext* aContext,
                                    QScriptEngine* aEngine);

  static QScriptValue depthFunction(QScriptContext* aContext,
                                    QScriptEngine* aEngine);

private:
  BGeneratorRef mDelay;
  BGeneratorRef mFeedback;
  BGeneratorRef mSpeed;
  BGeneratorRef mDepth;

  maxiFlanger mMaxi;
};

#endif
