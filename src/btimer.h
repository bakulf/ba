#ifndef _BA_TIMER_H_
#define _BA_TIMER_H_

#include <QScriptValue>
#include <QList>

class BApplication;
class BTimerData;

class QTimer;
class QScriptContext;

class BTimer : public QObject
{
  Q_OBJECT

public:
  BTimer(BApplication* aApp);
  virtual ~BTimer();

  QScriptValue setTimeout(QScriptContext* aContext,
                          QScriptValue aValue,
                          int aTimeout);

  void clearTimeout(int aTimeout);

  QScriptValue setInterval(QScriptContext* aContext,
                           QScriptValue aValue,
                           int aInterval);

  void clearInterval(int aInterval);

private:
  BApplication* mApp;

  QList<BTimerData*> mTimeouts;
  QList<BTimerData*> mIntervals;

  int mTimeId;
};

class BTimerData : public QObject
{
  Q_OBJECT

public:
  BTimerData(BTimer* aTimer,
             QScriptValue aValue,
             int aTimeout,
             bool aSingleShot,
             int id);

  virtual ~BTimerData();

  int id() const { return mId; }

private Q_SLOTS:
  void timeout();

private:
  BTimer* mParent;

  QScriptValue mValue;
  QTimer* mTimer;

  int mId;
};

#endif
