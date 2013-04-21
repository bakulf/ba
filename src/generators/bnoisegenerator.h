#ifndef _BA_NOISE_GENERATOR_H_
#define _BA_NOISE_GENERATOR_H_

#include "bgenerator.h"

#include <QScriptValue>

class QScriptContext;

class BNoiseGenerator : public BGenerator
{
  Q_OBJECT

public:
  BNoiseGenerator();
  virtual ~BNoiseGenerator();

  virtual void generate();
  virtual double get();

  static QScriptValue engineFunction(QScriptContext* aContext,
                                     QScriptEngine* aEngine);

private:
  static bool gInitialized;
};

#endif

