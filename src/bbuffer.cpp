#include "bbuffer.h"
#include "bapplication.h"

#include <QSettings>
#include <iostream>
#include <stdio.h>

BBuffer::BBuffer(QObject *aParent)
: QObject(aParent)
, mType(NoData)
, mData(NULL)
, mDataSize(0)
, mDataPosition(0.0)
, mSpeed(1.0)
, mLoop(false)
, mPlaying(false)
{
}

BBuffer::~BBuffer()
{
}

void
BBuffer::setRecData(double* aData, size_t aSize)
{
  if (mData) {
    free(mData);
  }

  mData = aData;
  mDataSize = aSize;
  mDataPosition = 0.0;

  mType = mData ? RecData : NoData;
}

void
BBuffer::setNoiseData()
{
  mType = NoiseData;
}

void
BBuffer::setTypeDataString(QString& aType)
{
  if (aType == "rec") {
    setTypeData(mData ? RecData : NoData);
    return;
  }

  if(aType == "noise") {
    setTypeData(NoiseData);
    return;
  }

  setTypeData(NoData);
}

void
BBuffer::play()
{
  mPlaying = true;
  mDataPosition = 0.0;
}

void
BBuffer::stop()
{
  mPlaying = false;
}

QString
BBuffer::info() const
{
  QString msg;

  switch (mType) {
    case NoData:
      msg.append("No Data");
      break;

    case RecData:
      msg.append("Data: ");
      msg.append(BAudio::byte2str(mDataSize));
      break;

    case NoiseData:
      msg.append("Noise Data");
      break;
  }

  return msg;
}

bool
BBuffer::canPlay() const
{
  if (!mPlaying) {
    return false;
  }

  switch (mType) {
    case NoData:
      return false;

    case RecData:
      if (!mLoop && (size_t)mDataPosition >= mDataSize) {
        return false;
      }
      break;

    case NoiseData:
      break;
  }

  return true;
}

void
BBuffer::output(double *aOutput)
{
  if (!canPlay()) {
    return;
  }

  switch (mType) {
    case NoData:
      break;

    case RecData:
      if (mLoop) {
        outputLoop(aOutput);
      } else {
        outputNoLoop(aOutput);
      }

      break;

    case NoiseData:
      for (int j=0; j < maxiSettings::channels; ++j) {
        aOutput[j] = (double)qrand() / (double)RAND_MAX;
      }
      break;
  }


  mEngine.output(aOutput);
}

void
BBuffer::outputLoop(double* aOutput)
{
  double remainder;
  size_t a,b;


  if (mSpeed >=0) {
    for (int j=0; j < maxiSettings::channels; ++j) {
      mDataPosition += mSpeed;

      if ((size_t)mDataPosition >= mDataSize - 1) {
        mDataPosition = 1;
      }

      remainder = mDataPosition - floor(mDataPosition);
      if ((size_t)mDataPosition + 1 < mDataSize) {
        a = mDataPosition + 1;
      } else {
        a = mDataSize - 1;
      }

      if ((size_t)mDataPosition + 2 < mDataSize) {
        b = mDataPosition + 2;
      } else {
        b = mDataSize - 1;
      }

      //linear interpolation
      aOutput[j] = ((1 - remainder) * mData[a] + remainder * mData[b]);
    }
  } else {
    for (int j=0; j < maxiSettings::channels; ++j) {
      mDataPosition += mSpeed;

      if (mDataPosition < 0) {
        mDataPosition = mDataSize;
      }

      remainder = mDataPosition - floor(mDataPosition);
      if (mDataPosition - 1 >= 0) {
        a = mDataPosition - 1;
      } else {
        a = 0;
      }

      if (mDataPosition - 2 >= 0) {
        b = mDataPosition - 2;
      } else {
        b = 0;
      }

      //linear interpolation
      aOutput[j] = (double) ((-1-remainder) * mData[a] + remainder * mData[b]);
    }
  }
}

void
BBuffer::outputNoLoop(double* aOutput)
{
  for (int j=0; j < maxiSettings::channels; ++j) {
    mDataPosition = mDataPosition + mSpeed;
    double remainder = mDataPosition - floor(mDataPosition);

    if ((size_t)mDataPosition < mDataSize) {
      aOutput[j] = ((1 - remainder) * mData[1 + (size_t) mDataPosition] +
                    remainder * mData[2 + (size_t)mDataPosition]);
    } else {
      stop();
    }
  }
}
