#ifndef _BA_PHASOR_GENERATOR_H_
#define _BA_PHASOR_GENERATOR_H_

#include "bgenerator.h"

#include "maximilian.h"

#include <QScriptValue>

class QScriptContext;

class BPhasorGenerator : public BGenerator
{
  Q_OBJECT

public:
  BPhasorGenerator(BGenerator* aFrequency, BGenerator* aStarphase,
                   BGenerator* aEndphase);
  virtual ~BPhasorGenerator();

  virtual void generate();
  virtual double get();

  static QScriptValue engineFunction(QScriptContext* aContext,
                                     QScriptEngine* aEngine);

protected:
  virtual void engineProperties(QScriptEngine* aEngine,
                                QScriptValue aValue);

  static QScriptValue frequencyFunction(QScriptContext* aContext,
                                        QScriptEngine* aEngine);

  static QScriptValue startphaseFunction(QScriptContext* aContext,
                                         QScriptEngine* aEngine);

  static QScriptValue endphaseFunction(QScriptContext* aContext,
                                        QScriptEngine* aEngine);

private:
  maxiOsc mMaxi;

  double mValue;
  BGeneratorRef mFrequency;
  BGeneratorRef mStartphase;
  BGeneratorRef mEndphase;
};

#endif

