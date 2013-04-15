#ifndef _BA_FLANGERFILTER_H_
#define _BA_FLANGERFILTER_H_

#include "bengine.h"
#include "maximilian.h"

class BFlangerFilter : public BEngineFilter
{
  Q_OBJECT
  Q_DISABLE_COPY(BFlangerFilter)

public:
  BFlangerFilter(QObject* aParent,
                 int aDelay,
                 double aFeedback,
                 double aSpeed,
                 double aDepth);
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
  int mDelay;
  double mFeedback;
  double mSpeed;
  double mDepth;

  maxiFlanger mMaxi;
};

#endif
