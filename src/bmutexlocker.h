#ifndef _BA_MUTEX_LOCKER_H_
#define _BA_MUTEX_LOCKER_H_

#include <QMutexLocker>

class BMutexLocker : public QMutexLocker
{
public:
  BMutexLocker();
  virtual ~BMutexLocker();
};

#define BMUTEXLOCKER \
  BMutexLocker __locker;

#endif
