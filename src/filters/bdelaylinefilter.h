#ifndef _BA_DELAYLINEFILTER_H_
#define _BA_DELAYLINEFILTER_H_

#include "bengine.h"
#include "maximilian.h"

class BDelaylineFilter : public BEngineFilter
{
  Q_OBJECT
  Q_DISABLE_COPY(BDelaylineFilter)

public:
  BDelaylineFilter(QObject* aParent,
                   BGenerator* aSize,
                   BGenerator* aFeedback,
                   BGenerator* aPosition);
  virtual ~BDelaylineFilter();

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

  static QScriptValue sizeFunction(QScriptContext* aContext,
                                   QScriptEngine* aEngine);

  static QScriptValue feedbackFunction(QScriptContext* aContext,
                                       QScriptEngine* aEngine);

  static QScriptValue positionFunction(QScriptContext* aContext,
                                       QScriptEngine* aEngine);

private:
  BGeneratorRef mSize;
  BGeneratorRef mFeedback;
  BGeneratorRef mPosition;

  maxiDelayline mMaxi;
};

typedef BRef<BDelaylineFilter> BDelaylineFilterRef;

#endif
