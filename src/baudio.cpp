#include "baudio.h"

#define BUFFER_SIZE 4096

BAudio::BAudio()
: mRecording(false)
, mRecDataSize(0)
, mRecDataTotal(0)
, mRecData(NULL)
, mAudioData(maxiSettings::channels, 0)
{
}

BAudio::~BAudio()
{
}

void
BAudio::terminate()
{
  try {
    mRta->stopStream();
  }
  catch (RtError& e) {
    e.printMessage();
  }

  if (mRta->isStreamOpen()) {
    mRta->closeStream();
  }
}

bool
BAudio::init()
{
  mRta = new RtAudio(RtAudio::WINDOWS_DS);

  if (mRta->getDeviceCount() < 1) {
    std::cout << "\nNo audio devices found!\n";
    return false;
  }

  RtAudio::StreamParameters outParams;
  outParams.deviceId = mRta->getDefaultOutputDevice();
  outParams.nChannels = maxiSettings::channels;
  outParams.firstChannel = 0;
  unsigned int sampleRate = maxiSettings::sampleRate;
  unsigned int bufferFrames = maxiSettings::bufferSize;

  RtAudio::StreamParameters inParams;
  inParams.deviceId = mRta->getDefaultOutputDevice();
  inParams.nChannels = maxiSettings::channels;
  inParams.firstChannel = 0;

  try {
    mRta->openStream(&outParams, &inParams, RTAUDIO_FLOAT64,
                      sampleRate, &bufferFrames, &BAudio::audioRouting,
                      this);
  } catch (RtError& e) {
    e.printMessage();
    return false;
  }

  mRta->startStream();
  return true;
}

void
BAudio::recStart()
{
  mRecording = true;
  recClear();
}

double*
BAudio::recStop(size_t* aSize)
{
  mRecording = false;

  double* data = mRecData;
  mRecData = NULL;

  *aSize = mRecDataSize;
  return data;
}

void
BAudio::recClear()
{
  free(mRecData);
  mRecData = NULL;
  mRecDataTotal = mRecDataSize = 0;
}

QString
BAudio::recordingStr()
{
  int size = mRecDataSize;

  size /= 1024;
  int k = size % 1024;

  size /= 1024;
  int m = size % 1024;

  QString value;

  if (m) {
    value.sprintf("%dM", m);
  }

  if (k) {
    value.sprintf("%dK", k);
  }

  return value;
}

int
BAudio::audioRouting(void* outputBuffer, void* inputBuffer,
                     unsigned int nBufferFrames, double /*streamTime*/,
                     RtAudioStreamStatus status, void* userData)
{
  BAudio* audio = (BAudio*) userData;
  double* values = (double*) &audio->mAudioData;
  double* outBuffer = (double*) outputBuffer;
  double* inBuffer = (double*) inputBuffer;

  //  double currentTime = (double) streamTime; Might come in handy for control
  if (status) {
    std::cout << "Stream underflow detected!" << std::endl;
  }

  if (audio->mRecording) {
    int total = nBufferFrames * maxiSettings::channels;

    while ((audio->mRecDataTotal - audio->mRecDataSize) < total) {
      if (!audio->mRecData) {
        audio->mRecDataTotal += BUFFER_SIZE;
        audio->mRecData = (double*)malloc(sizeof(double) * audio->mRecDataTotal);
      } else {
        audio->mRecDataTotal += BUFFER_SIZE;
        audio->mRecData = (double*)realloc(audio->mRecData, sizeof(double) * audio->mRecDataTotal);
      }
    }
  }

  // Write interleaved audio data.
  for (unsigned int i=0; i < nBufferFrames; ++i) {
    // TODO play(data->mAudioData);
    for (int j=0; j < maxiSettings::channels; ++j) {
      *outBuffer++ = values[j];
    }

    if (audio->mRecording) {
      audio->mRecData[audio->mRecDataSize++] = *inBuffer++;
    }
  }

  return 0;
}


