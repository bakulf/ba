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

  virtual double get() { return mNumber; }

  static QScriptValue engineFunction(QScriptContext* aContext,
                                     QScriptEngine* aEngine);

protected:
  virtual void generateInternal(quint64) {}

private:
  virtual void makeObjGenerator(BScriptEngine* aEngine);

private:
  double mNumber;
};

#endif

