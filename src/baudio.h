#ifndef _BA_AUDIO_H_
#define _BA_AUDIO_H_

#include "bengine.h"

#include <QString>

#include <RtAudio.h>
#include "maximilian.h"

class BApplication;

class BAudio
{
  Q_DISABLE_COPY(BAudio)

public:
  BAudio();
  virtual ~BAudio();

  void recStart();
  double* recStop(size_t* aSize);
  bool recording() const { return mRecording; }
  QString recordingStr() { return byte2str(mRecDataSize); }

  bool init(BApplication* aApp);
  void terminate();

  BEngine& engine() { return mEngine; }

  static QString byte2str(size_t size);

private:
  static int audioRouting(void* outputBuffer, void* inputBuffer,
                          unsigned int nBufferFrames, double streamTime,
                          RtAudioStreamStatus status, void* userData);

  void recClear();

  void play(double* aOutput);

  bool deviceId(QString aDevice, RtAudio::StreamParameters* aParams);

private:
  BApplication *mApp;

  RtAudio* mRta;

  bool mRecording;
  size_t mRecDataSize;
  size_t mRecDataTotal;
  double* mRecData;

  BEngine mEngine;
};

#endif
