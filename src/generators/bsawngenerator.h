#ifndef _BA_SAWN_GENERATOR_H_
#define _BA_SAWN_GENERATOR_H_

#include "bgenerator.h"

#include "maximilian.h"

#include <QScriptValue>

class QScriptContext;

class BSawnGenerator : public BGenerator
{
  Q_OBJECT

public:
  BSawnGenerator(BGenerator* aFrequency);
  virtual ~BSawnGenerator();

  virtual void generateInternal(quint64 aToken);
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

