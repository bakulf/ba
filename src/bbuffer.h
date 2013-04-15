#ifndef _BA_BUFFER_H_
#define _BA_BUFFER_H_

#include "bengine.h"

class BApplication;
class QSettings;

class BBuffer : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(BBuffer)

public:
  BBuffer(QObject* aParent);
  virtual ~BBuffer();

  void setData(double* aData, size_t aSize);

  bool loop() const { return mLoop; }
  void setLoop(bool aLoop) { mLoop = aLoop; }

  double speed() const { return mSpeed; }
  void setSpeed(double aSpeed) { mSpeed = aSpeed; }

  QString info() const;

  BEngine& engine() { return mEngine; }

  void play();
  void stop();
  bool playing() const { return mPlaying; }
  bool canPlay() const;

  void output(double* aOutput);

private:
  void outputLoop(double* aOutput);
  void outputNoLoop(double* aOutput);

private:
  double* mData;
  size_t mDataSize;
  double mDataPosition;

  double mSpeed;

  bool mLoop;

  bool mPlaying;

  BEngine mEngine;
};

#endif
