#ifndef _BA_BUFFER_H_
#define _BA_BUFFER_H_

#include "bengine.h"

#include "maximilian.h"

class BGenerator;
class BApplication;
class BScriptEngine;
class QSettings;

class BBuffer : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(BBuffer)

public:
  BBuffer(BApplication* aApp);
  virtual ~BBuffer();

  enum TypeData {
    NoData,
    RecData,
    FileData,
    GeneratorData,
  };

  void setRecData(double* aData, size_t aSize);
  void setFileData(QString aFilename);
  void setGeneratorData(BGenerator* aGenerator);

  void setTypeData(TypeData aType) { mType = aType; }
  QScriptValue typeDataObject(BScriptEngine* aEngine) const;

  bool loop() const { return mLoop; }
  void setLoop(bool aLoop) { mLoop = aLoop; }

  BGenerator* speed() const { return mSpeed; }
  void setSpeed(BGenerator* aSpeed) { mSpeed = aSpeed; }

  QString info() const;

  BEngine& engine() { return mEngine; }

  void play();
  void stop();
  bool playing() const { return mPlaying; }
  bool canPlay() const;

  void output(double* aOutput);

private:
  void recOutputLoop(double* aOutput);
  void recOutputNoLoop(double* aOutput);

  void fileOutputLoop(double* aOutput);
  void fileOutputNoLoop(double* aOutput);

private:
  BApplication* mApp;

  TypeData mType;

  double* mData;
  size_t mDataSize;
  double mDataPosition;

  QString mFilename;
  maxiSample mMaxi;

  BGeneratorRef mGenerator;

  BGeneratorRef mSpeed;

  bool mLoop;

  bool mPlaying;

  BEngine mEngine;
};

#endif
