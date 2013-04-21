#ifndef _BA_PULSE_GENERATOR_H_
#define _BA_PULSE_GENERATOR_H_

#include "bgenerator.h"

#include "maximilian.h"

#include <QScriptValue>

class QScriptContext;

class BPulseGenerator : public BGenerator
{
  Q_OBJECT

public:
  BPulseGenerator(BGenerator* aFrequency, BGenerator* aDuty);
  virtual ~BPulseGenerator();

  virtual void generateInternal(quint64 aToken);
  virtual double get();

  static QScriptValue engineFunction(QScriptContext* aContext,
                                     QScriptEngine* aEngine);

protected:
  virtual void engineProperties(QScriptEngine* aEngine,
                                QScriptValue aValue);

  static QScriptValue frequencyFunction(QScriptContext* aContext,
                                        QScriptEngine* aEngine);

  static QScriptValue dutyFunction(QScriptContext* aContext,
                                   QScriptEngine* aEngine);

private:
  maxiOsc mMaxi;

  double mValue;
  BGeneratorRef mFrequency;
  BGeneratorRef mDuty;
};

#endif

