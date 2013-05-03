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

  RtAudio::StreamParameters outParams;
  RtAudio::StreamParameters inParams;

  if (!deviceId(aApp->outputDevice(), &outParams) ||
      !deviceId(aApp->inputDevice(), &inParams)) {
    mRta = NULL;
    return false;
  }

  outParams.nChannels = maxiSettings::channels;
  inParams.nChannels = aApp->inputChannels();

  outParams.firstChannel = 0;
  inParams.firstChannel = 0;

  unsigned int bufferFrames = maxiSettings::bufferSize;

  try {
    mRta->openStream(&outParams, &inParams, RTAUDIO_FLOAT64,
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

bool
BAudio::deviceId(QString aDevice, RtAudio::StreamParameters* aParams)
{
  if (aDevice.isEmpty()) {
    aParams->deviceId = mRta->getDefaultOutputDevice();
    return true;
  }

  for (int i = 0, len = mRta->getDeviceCount(); i < len; ++i) {
    if (aDevice.toStdString() == mRta->getDeviceInfo(i).name) {
      aParams->deviceId = i;
      return true;
    }
  }

  std::cout << "No audio device found with name: `"
            << qPrintable(aDevice) << "'" << std::endl;
  std::cout << "The list of available devices is:" << std::endl;

  for (int i = 0, len = mRta->getDeviceCount(); i < len; ++i) {
    std::cout << mRta->getDeviceInfo(i).name << std::endl;
  }

  return false;
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
    value.append(QString("%1M").arg(m));
  }

  if (k > 0) {
    value.append(QString("%1K").arg(k));
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

    audio->mApp->updateGenerators();

    double values[maxiSettings::channels];
    audio->play(values);

    for (int j=0; j < maxiSettings::channels; ++j) {
      *outBuffer++ = values[j];
    }

    if (audio->mRecording) {
      double lastValue = 0;
      for (int j=0; j < audio->mApp->inputChannels(); ++j) {
        lastValue = *inBuffer++;
        audio->mRecData[audio->mRecDataSize++] = lastValue;
      }

      // from inputChannels to outputChannels
      for (int k=audio->mApp->inputChannels(); k < maxiSettings::channels; ++k) {
        audio->mRecData[audio->mRecDataSize++] = lastValue;
      }
    }
  }

  return 0;
}
