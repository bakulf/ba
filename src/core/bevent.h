#ifndef _BA_EVENT_H_
#define _BA_EVENT_H_

#include <QObject>
#include <QMap>

class BApplication;

class BEvent : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(BEvent)

public:
  BEvent(BApplication* aParent, QString aCode);
  virtual ~BEvent();

  void run();

private:
  BApplication* mApp;
  QString mCode;
};

class BEventManager : public QObject
{
  Q_OBJECT

public:
  BEventManager();
  virtual ~BEventManager();

  void add(char aCh, BEvent* aEvent);

  void handle(char aCh);

private:
  QMap<char, BEvent*> mMap;
};

#endif
