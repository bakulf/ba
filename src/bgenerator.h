#ifndef _BA_GENERATOR_H_
#define _BA_GENERATOR_H_

#include "bref.h"

#include <QScriptValue>

class BScriptEngine;
class QScriptContext;

class BGenerator : public QObject,
                   public BRefObj
{
  Q_OBJECT

public:
  BGenerator(QString aName);
  virtual ~BGenerator();

  QString name() const { return mName; }

  QScriptValue objGenerator(BScriptEngine* aEngine);

  virtual void generate() = 0;
  virtual double get() = 0;

  static void generatorFactory(QScriptEngine* aEngine);

  static BGenerator* numberToGenerator(QScriptValue aValue);

protected:

  virtual void engineProperties(QScriptEngine*,
                                QScriptValue) {}

  virtual void makeObjGenerator(BScriptEngine* aEngine);

  static QScriptValue funcGeneratorName(QScriptContext* aContext,
                                        QScriptEngine* aEngine);

  static QScriptValue funcGeneratorGet(QScriptContext* aContext,
                                       QScriptEngine* aEngine);

protected:
  QScriptValue mObjGenerator;
  QString mName;
};

typedef BRef<BGenerator> BGeneratorRef;

class BGeneratorShell : public QObject
{
  Q_OBJECT

public:
  BGeneratorShell(BGenerator* aObj)
  : mObj(aObj)
  {
  }

  virtual ~BGeneratorShell()
  {
  }

  BGenerator*
  get()
  {
    return mObj;
  }

private:
  BGeneratorRef mObj;
};

#endif
