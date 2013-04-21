#ifndef _BA_GENERATOR_H_
#define _BA_GENERATOR_H_

#include <QScriptValue>
#include <iostream>

class BScriptEngine;
class QScriptContext;

class BGenerator : public QObject
{
  Q_OBJECT

public:
  BGenerator(QString aName);
  virtual ~BGenerator();

  void addRef();
  void release();

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

protected:
  QScriptValue mObjGenerator;
  QString mName;

  quint64 mRef;
};

class BGeneratorRef
{
public:
  BGeneratorRef()
  : mRawPtr(0)
  {
  }

  BGeneratorRef(const BGeneratorRef& aCopy)
  : mRawPtr(aCopy.mRawPtr)
  {
    if (mRawPtr) {
      mRawPtr->addRef();
    }
  }

  BGeneratorRef(BGenerator* aRawPtr)
  : mRawPtr(aRawPtr)
  {
    if (mRawPtr) {
      mRawPtr->addRef();
    }
  }

  virtual ~BGeneratorRef()
  {
    if (mRawPtr) {
      mRawPtr->release();
    }
  }

  BGeneratorRef&
  operator=(const BGeneratorRef& aCopy)
  {
    assignWithAddRef(aCopy.mRawPtr);
    return *this;
  }

  BGeneratorRef&
  operator=(BGenerator* aRawPtr)
  {
    assignWithAddRef(aRawPtr);
    return *this;
  }

  BGenerator*
  get() const
  {
    return mRawPtr;
  }

  operator BGenerator*() const
  {
    return get();
  }

  BGenerator*
  operator->() const
  {
    if (!mRawPtr) {
      std::cerr << "You can't dereference a NULL nsRefPtr with operator->()." << std::endl;
    }
    return get();
  }

  BGenerator&
  operator*() const
  {
    if (!mRawPtr) {
      std::cerr << "You can't dereference a NULL nsRefPtr with operator->()." << std::endl;
    }
    return *get();
  }

private:
  void
  assignWithAddRef(BGenerator* aRawPtr)
  {
    if (aRawPtr) {
      aRawPtr->addRef();
    }
    assignAssumingAddRef(aRawPtr);
  }

  void
  assignAssumingAddRef(BGenerator* aRawPtr)
  {
    BGenerator* oldPtr = mRawPtr;
    mRawPtr = aRawPtr;
    if (oldPtr) {
      oldPtr->release();
    }
  }

private:
  BGenerator* mRawPtr;
};

#endif
