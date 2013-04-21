#ifndef _BA_COMPRESSORFILTER_H_
#define _BA_COMPRESSORFILTER_H_

#include "bengine.h"
#include "maximilian.h"

class BCompressorFilter : public BEngineFilter
{
  Q_OBJECT
  Q_DISABLE_COPY(BCompressorFilter)

public:
  BCompressorFilter(QObject* aParent,
                    BGenerator* aRatio,
                    BGenerator* aThreshold,
                    BGenerator* aAttach,
                    BGenerator* aRelease);
  virtual ~BCompressorFilter();

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

  static QScriptValue ratioFunction(QScriptContext* aContext,
                                    QScriptEngine* aEngine);

  static QScriptValue thresholdFunction(QScriptContext* aContext,
                                        QScriptEngine* aEngine);

  static QScriptValue attachFunction(QScriptContext* aContext,
                                     QScriptEngine* aEngine);

  static QScriptValue releaseFunction(QScriptContext* aContext,
                                      QScriptEngine* aEngine);

private:
  BGeneratorRef mRatio;
  BGeneratorRef mThreshold;
  BGeneratorRef mAttach;
  BGeneratorRef mRelease;

  maxiDyn mMaxi;
};

#endif
