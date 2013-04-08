#include "bbuffer.h"
#include "bapplication.h"
#include "beventselectbuffer.h"

#include <QSettings>
#include <iostream>
#include <stdio.h>

BBuffer::BBuffer(QObject *aParent)
: QObject(aParent)
, mData(NULL)
, mDataSize(0)
, mPlaying(false)
{
}

BBuffer::~BBuffer()
{
}

BBuffer*
BBuffer::create(BApplication* aApplication,
                QSettings& aSettings,
                int id)
{
  char key[1024];

  BBuffer* buffer = new BBuffer(aApplication);

  snprintf(key, sizeof(key), "buffer_%d/key", id);
  if (!aSettings.contains(key)) {
    std::cerr << "The key must be set (buffer_" << id << ")." << std::endl;
    buffer->deleteLater();
    return NULL;
  }

  QString value = aSettings.value(key).toString();
  if (value.length() < 0 || value.length() > 1) {
    std::cerr << "The key must be 1 single char (buffer_" << id << ")." << std::endl;
    buffer->deleteLater();
    return NULL;
  }

  buffer->setKey(value[0]);

  aApplication->eventManager().add(value[0],
    new BEventSelectBuffer(aApplication, buffer));

  return buffer;
}

void
BBuffer::setData(double* aData, size_t aSize)
{
  if (mData) {
    free(mData);
  }

  mData = aData;
  mDataSize = aSize;
}

void
BBuffer::play()
{
  mPlaying = true;
}

void
BBuffer::stop()
{
  mPlaying = false;
}
