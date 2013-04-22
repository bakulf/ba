#ifndef _BA_OP_GENERATOR_H_
#define _BA_OP_GENERATOR_H_

#include "bgenerator.h"

#include <QScriptValue>

class QScriptContext;

class BOpGenerator : public BGenerator
{
  Q_OBJECT

  struct OpType {
    int mId;
    const char *mName;
    double (*generate)(QList<BGeneratorRef>& aInput);
  };

public:
  BOpGenerator(OpType& aType, QList<BGeneratorRef>& aInputs);
  virtual ~BOpGenerator();

  virtual void generateInternal(quint64 aToken);
  virtual double get();

  static void createPrototype(QScriptEngine* aEngine,
                              QScriptValue aValue);

  static QScriptValue engineFunction(QScriptContext* aContext,
                                     QScriptEngine* aEngine);

protected:
  virtual void engineProperties(QScriptEngine* aEngine,
                                QScriptValue aValue);

  static QScriptValue typeFunction(QScriptContext* aContext,
                                   QScriptEngine* aEngine);

  static QScriptValue typesFunction(QScriptContext* aContext,
                                    QScriptEngine* aEngine);

  static QScriptValue inputFunction(QScriptContext* aContext,
                                    QScriptEngine* aEngine);

  static double sumOp(QList<BGeneratorRef>& aInputs);
  static double diffOp(QList<BGeneratorRef>& aInputs);
  static double prodOp(QList<BGeneratorRef>& aInputs);
  static double divOp(QList<BGeneratorRef>& aInputs);
  static double modOp(QList<BGeneratorRef>& aInputs);
  static double avgOp(QList<BGeneratorRef>& aInputs);
  static double powOp(QList<BGeneratorRef>& aInputs);
  static double sqrtOp(QList<BGeneratorRef>& aInputs);
  static double logOp(QList<BGeneratorRef>& aInputs);
  static double log10Op(QList<BGeneratorRef>& aInputs);
  static double expOp(QList<BGeneratorRef>& aInputs);

private:
  double mValue;

  QList<BGeneratorRef> mInputs;
  OpType mType;

  static OpType OpTypes[];
};

#endif

