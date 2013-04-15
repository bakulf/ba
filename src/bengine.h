#ifndef _BA_ENGINE_H_
#define _BA_ENGINE_H_

#include <QScriptEngine>

class BEngineFilter;
class BScriptEngine;

class BEngine : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(BEngine)

public:
  BEngine();
  virtual ~BEngine();

  double volume() const;
  void setVolume(double aVolume);

  double volume(int aChannel) const;
  void setVolume(double aVolume, int aChannel);

  QList<BEngineFilter*>& filters() { return mFilters; }

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
  double* mVolumes;

  QList<BEngineFilter*> mFilters;

  QScriptValue mObjFilters;
  int mOldFiltersLength;
};

// Base class for any filter
class BEngineFilter : public QObject
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

#endif
