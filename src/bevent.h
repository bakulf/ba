#ifndef _BA_EVENT_H_
#define _BA_EVENT_H_

#include <QObject>
#include <QMap>

// Base class for events
class BEvent : public QObject
{
  Q_OBJECT

public:
  BEvent(QObject* aParent);
  virtual ~BEvent();

  virtual void run();
};

class BEventManager : public QObject
{
  Q_OBJECT

public:
  BEventManager();
  virtual ~BEventManager();

  void add(QChar aCh, BEvent* aEvent);

  void handle(QChar aCh);

private:
  QMap<QChar, BEvent*> mMap;
};

#endif
