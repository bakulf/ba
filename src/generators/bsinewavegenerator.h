#ifndef _BA_SINEWAVE_GENERATOR_H_
#define _BA_SINEWAVE_GENERATOR_H_

#include "bgenerator.h"

#include "maximilian.h"

#include <QScriptValue>

class QScriptContext;

class BSinewaveGenerator : public BGenerator
{
  Q_OBJECT

public:
  BSinewaveGenerator(BGenerator* aFrequency);
  virtual ~BSinewaveGenerator();

  virtual void generate();
  virtual double get();

  static QScriptValue engineFunction(QScriptContext* aContext,
                                     QScriptEngine* aEngine);

protected:
  virtual void engineProperties(QScriptEngine* aEngine,
                                QScriptValue aValue);

  static QScriptValue frequencyFunction(QScriptContext* aContext,
                                        QScriptEngine* aEngine);


private:
  maxiOsc mMaxi;

  double mValue;
  BGeneratorRef mFrequency;
};

#endif

