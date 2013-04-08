#ifndef _BA_BUFFER_H_
#define _BA_BUFFER_H_

#include <QObject>

class BApplication;
class QSettings;

class BBuffer : public QObject
{
  Q_OBJECT

public:
  BBuffer(QObject* aParent);
  virtual ~BBuffer();

  QChar key() const { return mKey; }
  void setKey(QChar aKey) { mKey = aKey; }

  double* data(size_t* aSize) const { *aSize = mDataSize; return mData; }
  void setData(double* aData, size_t aSize);

  void play();
  void stop();
  bool playing() { return mPlaying; }

  static BBuffer* create(BApplication* aApplication,
                         QSettings& aSettings, int id);

private:
  QChar mKey;

  double* mData;
  size_t mDataSize;

  bool mPlaying;
};

#endif

