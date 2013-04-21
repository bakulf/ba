#ifndef _BA_NUMBER_GENERATOR_H_
#define _BA_NUMBER_GENERATOR_H_

#include "bgenerator.h"

#include <QScriptValue>

class QScriptContext;

class BNumberGenerator : public BGenerator
{
  Q_OBJECT

public:
  BNumberGenerator(double aNumber);
  virtual ~BNumberGenerator();

  virtual void generate() {}
  virtual double get() { return mNumber; }

  static QScriptValue engineFunction(QScriptContext* aContext,
                                     QScriptEngine* aEngine);

private:
  virtual void makeObjGenerator(BScriptEngine* aEngine);

private:
  double mNumber;
};

#endif

