#ifndef _BA_AUDIO_H_
#define _BA_AUDIO_H_

#include <QString>

#include <RtAudio.h>
#include "maximilian.h"

class BAudio
{
public:
  BAudio();
  virtual ~BAudio();

  void recStart();
  double* recStop(size_t* aSize);
  bool recording() const { return mRecording; }
  QString recordingStr();

  bool init();
  void terminate();

private:
  static int audioRouting(void* outputBuffer, void* inputBuffer,
                          unsigned int nBufferFrames, double streamTime,
                          RtAudioStreamStatus status, void* userData);

  void recClear();

private:
  RtAudio* mRta;

  bool mRecording;
  size_t mRecDataSize;
  size_t mRecDataTotal;
  double* mRecData;

  vector<double> mAudioData;
};

#endif
