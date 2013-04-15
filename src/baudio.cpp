#include "baudio.h"
#include "bapplication.h"
#include "bbuffer.h"
#include "bmutexlocker.h"

#define BUFFER_SIZE 4096

BAudio::BAudio()
: mRta(NULL)
, mRecording(false)
, mRecDataSize(0)
, mRecDataTotal(0)
, mRecData(NULL)
{
}

BAudio::~BAudio()
{
  if (mRta) {
    delete mRta;
  }
}

void
BAudio::terminate()
{
  if (!mRta) {
    return;
  }

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
BAudio::init(BApplication* aApp)
{
  mRta = new RtAudio(RtAudio::WINDOWS_DS);

  if (mRta->getDeviceCount() < 1) {
    std::cout << "\nNo audio devices found!\n";
    mRta = NULL;
    return false;
  }

  RtAudio::StreamParameters params;
  params.deviceId = mRta->getDefaultOutputDevice();
  params.nChannels = maxiSettings::channels;
  params.firstChannel = 0;
  unsigned int bufferFrames = maxiSettings::bufferSize;

  try {
    mRta->openStream(&params, &params, RTAUDIO_FLOAT64,
                     maxiSettings::sampleRate, &bufferFrames,
                     &BAudio::audioRouting, this);
  } catch (RtError& e) {
    e.printMessage();
    mRta = NULL;
    return false;
  }

  mRta->startStream();
  mApp = aApp;

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
  if (mRecData) {
    free(mRecData);
  }

  mRecData = NULL;
  mRecDataTotal = mRecDataSize = 0;
}

QString
BAudio::byte2str(size_t size)
{
  size *= sizeof(double);

  size /= 1024;
  int k = size % 1024;

  size /= 1024;
  int m = size % 1024;

  QString value;

  if (m > 0) {
    QString tmp;
    tmp.sprintf("%dM", m);
    value.append(tmp);
  }

  if (k > 0) {
    QString tmp;
    tmp.sprintf("%dK", k);
    value.append(tmp);
  }

  return value;
}

void
BAudio::play(double* aOutput)
{
  memset(aOutput, 0, sizeof(double) * maxiSettings::channels);

  QList<BBuffer*> buffers = mApp->buffers();
  foreach (BBuffer* buffer, buffers) {

    if (buffer->canPlay()) {
      double tmp[maxiSettings::channels];
      memset(tmp, 0, sizeof(double) * maxiSettings::channels);

      buffer->output(tmp);

      for (int j=0; j < maxiSettings::channels; ++j) {
        aOutput[j] = (aOutput[j] + tmp[j]) / 2;
      }
    }
  }

  mEngine.output(aOutput);
}

int
BAudio::audioRouting(void* outputBuffer, void* inputBuffer,
                     unsigned int nBufferFrames, double /*streamTime*/,
                     RtAudioStreamStatus status, void* userData)
{
  BAudio* audio = static_cast<BAudio*>(userData);
  double* outBuffer = (double*) outputBuffer;
  double* inBuffer = (double*) inputBuffer;

  //  double currentTime = (double) streamTime; Might come in handy for control
  if (status == RTAUDIO_OUTPUT_UNDERFLOW) {
    std::cout << "Stream underflow detected!" << std::endl;
  }

  BMUTEXLOCKER

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

      if (!audio->mRecData) {
        std::cout << "No memory?!? - Ready for a crash?" << std::endl;
        audio->mRecording = false;
        break;
      }
    }
  }

  // Write interleaved audio data.
  for (unsigned int i=0; i < nBufferFrames; ++i) {
    double values[maxiSettings::channels];
    audio->play(values);

    for (int j=0; j < maxiSettings::channels; ++j) {
      *outBuffer++ = values[j];
    }

    if (audio->mRecording) {
      for (int j=0; j < maxiSettings::channels; ++j) {
        audio->mRecData[audio->mRecDataSize++] = *inBuffer++;
      }
    }
  }

  return 0;
}
