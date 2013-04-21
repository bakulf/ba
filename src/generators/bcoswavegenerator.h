#ifndef _BA_COSWAVE_GENERATOR_H_
#define _BA_COSWAVE_GENERATOR_H_

#include "bgenerator.h"

#include "maximilian.h"

#include <QScriptValue>

class QScriptContext;

class BCoswaveGenerator : public BGenerator
{
  Q_OBJECT

public:
  BCoswaveGenerator(BGenerator* aFrequency);
  virtual ~BCoswaveGenerator();

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

