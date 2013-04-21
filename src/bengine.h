#ifndef _BA_ENGINE_H_
#define _BA_ENGINE_H_

#include "bref.h"
#include "bgenerator.h"

#include <QScriptEngine>

class BEngineFilter;
class BScriptEngine;
class BNumberGenerator;

typedef BRef<BEngineFilter> BEngineFilterRef;

class BEngine : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(BEngine)

public:
  BEngine();
  virtual ~BEngine();

  BGenerator* volume();
  void setVolume(BGenerator* aVolume);

  BGenerator* volume(int aChannel) const;
  void setVolume(BGenerator* aVolume, int aChannel);

  QList<BEngineFilterRef>& filters() { return mFilters; }

  void pushFilter(BEngineFilter* aFilter);
  BEngineFilter* popFilter();
  BEngineFilter* shiftFilter();
  void addFilter(BEngineFilter* aFilter);
  void replaceFilter(int aId, BEngineFilter* aFilter);

  BEngineFilter* findFilter(QString& aName);
  void reverseFilters();

  uint lengthFilters();

  QScriptValue objFilters() const { return mObjFilters; }
  void setObjFilters(QScriptValue aFilters) { mObjFilters = aFilters; }
  void refreshObjFilters(QScriptEngine* aEngine);

  void output(double *aOutput);

  static void filterFactory(QScriptEngine* aEngine);

  static QString writeVolume(BEngine& aEngine);
  static QStringList writeFilters(BEngine& aEngine);

private:
  QList<BGeneratorRef> mVolumes;

  QList<BEngineFilterRef> mFilters;

  QScriptValue mObjFilters;
  int mOldFiltersLength;
};

// Base class for any filter
class BEngineFilter : public QObject,
                      public BRefObj
{
  Q_OBJECT

public:
  BEngineFilter(QObject* aParent, QString aName);
  virtual ~BEngineFilter();

  QString name() const { return mName; }

  bool enabled() const { return mEnabled; }
  void setEnabled(bool aEnabled) { mEnabled = aEnabled; }

  virtual QString writeFilter();

  virtual void output(double *aOutput) = 0;

  QScriptValue objFilter(BScriptEngine* aEngine);

  virtual QScriptEngine::FunctionSignature engineUpdateFunction() = 0;

  virtual void engineProperties(QScriptEngine*,
                                QScriptValue) {}

  static QScriptValue funcFilterEnabled(QScriptContext* aContext,
                                        QScriptEngine* aEngine);

  static QScriptValue funcFilterName(QScriptContext* aContext,
                                     QScriptEngine* aEngine);

private:
  void makeObjFilter(BScriptEngine* aEngine);

  QString mName;
  bool mEnabled;

  QScriptValue mObjFilter;
};

class BEngineFilterShell : public QObject
{
  Q_OBJECT

public:
  BEngineFilterShell(BEngineFilter* aObj)
  : mObj(aObj)
  {
  }

  virtual ~BEngineFilterShell()
  {
  }

  BEngineFilter*
  get()
  {
    return mObj;
  }

private:
  BEngineFilterRef mObj;
};

#define METHOD_FUNCTION( _class , _shell , _method , _prop , _cname, _mname ) \
QScriptValue                                                                  \
_class::_method(QScriptContext* aContext, QScriptEngine* aEngine)             \
{                                                                             \
  _shell* shell = static_cast<_shell*>(aContext->thisObject().toQObject());   \
  _class* filter = static_cast<_class*>(shell->get());                        \
                                                                              \
  if (aContext->argumentCount()) {                                            \
    BGeneratorRef x = BGenerator::numberToGenerator(aContext->argument(0));   \
    if (!x) {                                                                 \
      return aContext->throwError(QScriptContext::SyntaxError,                \
                                  "#_cname.#_cname used wrongly.");           \
    }                                                                         \
                                                                              \
    filter->_prop = x;                                                        \
  }                                                                           \
                                                                              \
  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);               \
  return QScriptValue(filter->_prop ? filter->_prop->objGenerator(engine)     \
                                    : QScriptValue::UndefinedValue);          \
}

#endif
