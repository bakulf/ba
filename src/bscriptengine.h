#ifndef _BA_SCRIPTENGINE_H_
#define _BA_SCRIPTENGINE_H_

#include <QScriptEngine>

class BApplication;
class BBuffer;
class BEngine;
class BEngineFilter;

class BScriptEngine : public QScriptEngine
{
  Q_OBJECT
  Q_DISABLE_COPY(BScriptEngine)

public:
  BScriptEngine(BApplication* aApplication);
  ~BScriptEngine();

  BApplication* app() { return mApp; }

  static QScriptValue funcFilter(QScriptContext* aContext,
                                 QScriptEngine* aEngine);

private:
  static QScriptValue funcDump(QScriptContext* aContext,
                               QScriptEngine* aEngine);

#ifdef BA_UNITTEST
  static QScriptValue funcUnitTestIs(QScriptContext* aContext,
                                     QScriptEngine* aEngine);

  static QScriptValue funcUnitTestOk(QScriptContext* aContext,
                                     QScriptEngine* aEngine);
#endif

  static QScriptValue funcSetTimeout(QScriptContext* aContext,
                                     QScriptEngine* aEngine);

  static QScriptValue funcClearTimeout(QScriptContext* aContext,
                                       QScriptEngine* aEngine);

  static QScriptValue funcSetInterval(QScriptContext* aContext,
                                       QScriptEngine* aEngine);

  static QScriptValue funcClearInterval(QScriptContext* aContext,
                                        QScriptEngine* aEngine);

  static QScriptValue funcBuffers(QScriptContext* aContext,
                                  QScriptEngine* aEngine);

  static QScriptValue makeBufferObject(BScriptEngine* aEngine,
                                       BBuffer* aBuffer);

  static QScriptValue funcQuit(QScriptContext* aContext,
                               QScriptEngine* aEngine);

  static QScriptValue funcSelect(QScriptContext* aContext,
                                 QScriptEngine* aEngine);

  static QScriptValue funcCurrentBuffer(QScriptContext* aContext,
                                        QScriptEngine* aEngine);

  static QScriptValue funcLoop(QScriptContext* aContext,
                               QScriptEngine* aEngine);

  static QScriptValue funcSpeed(QScriptContext* aContext,
                               QScriptEngine* aEngine);

  static QScriptValue funcDataType(QScriptContext* aContext,
                                   QScriptEngine* aEngine);

  static QScriptValue funcPlay(QScriptContext* aContext,
                               QScriptEngine* aEngine);

  static QScriptValue funcStop(QScriptContext* aContext,
                               QScriptEngine* aEngine);

  static QScriptValue funcIsPlaying(QScriptContext* aContext,
                                    QScriptEngine* aEngine);

  static QScriptValue funcRecStart(QScriptContext* aContext,
                                   QScriptEngine* aEngine);

  static QScriptValue funcRecStop(QScriptContext* aContext,
                                  QScriptEngine* aEngine);

  static QScriptValue funcIsRecording(QScriptContext* aContext,
                                      QScriptEngine* aEngine);

  static QScriptValue funcVolume(QScriptContext* aContext,
                                 QScriptEngine* aEngine);

  static QScriptValue funcVolumes(QScriptContext* aContext,
                                  QScriptEngine* aEngine);

  static QScriptValue funcChannelVolume(QScriptContext* aContext,
                                        QScriptEngine* aEngine);

  static QScriptValue funcVolumeLength(QScriptContext* aContext,
                                       QScriptEngine* aEngine);

  static QScriptValue funcFilters(QScriptContext* aContext,
                                  QScriptEngine* aEngine);

  static void makeEngine(QScriptEngine* aEngine, QScriptValue aValue);

  static QScriptValue funcFilterLength(QScriptContext* aContext,
                                       QScriptEngine* aEngine);

  static QScriptValue funcFilterFind(QScriptContext* aContext,
                                     QScriptEngine* aEngine);

  static QScriptValue funcFilterPush(QScriptContext* aContext,
                                     QScriptEngine* aEngine);

  static QScriptValue funcFilterPop(QScriptContext* aContext,
                                    QScriptEngine* aEngine);

  static QScriptValue funcFilterReverse(QScriptContext* aContext,
                                        QScriptEngine* aEngine);

  static QScriptValue funcFilterShift(QScriptContext* aContext,
                                      QScriptEngine* aEngine);

  static QScriptValue funcFilterNotImplemented(QScriptContext* aContext,
                                               QScriptEngine* aEngine);

  static QScriptValue funcFilterUnshift(QScriptContext* aContext,
                                        QScriptEngine* aEngine);

private:
  BApplication* mApp;

  QScriptValue mObjBuffers;
};

#endif
