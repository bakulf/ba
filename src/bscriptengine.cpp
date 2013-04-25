#include "bscriptengine.h"
#include "bapplication.h"
#include "bmutexlocker.h"
#include "bgenerator.h"
#include "btimer.h"

BScriptEngine::BScriptEngine(BApplication* aApp)
: QScriptEngine(aApp)
, mApp(aApp)
, mObjBuffers(QScriptValue::UndefinedValue)
{
  // void dump(<something>);
  globalObject().setProperty("dump", newFunction(funcDump));

  // buffers
  globalObject().setProperty("buffers", newFunction(funcBuffers),
    QScriptValue::PropertyGetter | QScriptValue::ReadOnly);

  // currentBuffer
  globalObject().setProperty("currentBuffer",
    newFunction(funcCurrentBuffer),
    QScriptValue::PropertyGetter | QScriptValue::ReadOnly);

  // void quit()
  globalObject().setProperty("quit", newFunction(funcQuit));

  // void recStart()
  globalObject().setProperty("recStart", newFunction(funcRecStart));

  // void recStop(buffer)
  globalObject().setProperty("recStop", newFunction(funcRecStop));

  // bool recording
  globalObject().setProperty("recording", newFunction(funcIsRecording),
    QScriptValue::PropertyGetter | QScriptValue::ReadOnly);

  // id setTimeout(function() {...}, timeout);
  globalObject().setProperty("setTimeout", newFunction(funcSetTimeout));

  // void clearTimeout(<id>);
  globalObject().setProperty("clearTimeout", newFunction(funcClearTimeout));

  // id setInterval(function() {...}, timeout);
  globalObject().setProperty("setInterval", newFunction(funcSetInterval));

  // void clearInterval(<id>);
  globalObject().setProperty("clearInterval", newFunction(funcClearInterval));

#ifdef BA_UNITTEST
  // void BAUT_is(true, true, "maybe...");
  globalObject().setProperty("BA_UT_is", newFunction(funcUnitTestIs));

  // void BAUT_ok(true, "maybe...");
  globalObject().setProperty("BA_UT_ok", newFunction(funcUnitTestOk));
#endif

  // Any filter constructors
  BEngine::filterFactory(this);

  // Any generator constructors
  BGenerator::generatorFactory(this);

  // effects, volume & C
  makeEngine(this, globalObject());
}

BScriptEngine::~BScriptEngine()
{
}

void
BScriptEngine::makeEngine(QScriptEngine* aEngine, QScriptValue aValue)
{
  aValue.setProperty("volume", aEngine->newFunction(funcVolume),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);

  aValue.setProperty("volumes", aEngine->newFunction(funcVolumes),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);

  aValue.setProperty("filters", aEngine->newFunction(funcFilters),
    QScriptValue::PropertyGetter | QScriptValue::ReadOnly);
}

// BBUFFER FUNCTIONS ----------------------------------------------------------

QScriptValue
BScriptEngine::funcDataType(QScriptContext* aContext,
                            QScriptEngine* aEngine)
{
  BBuffer* buffer = static_cast<BBuffer*>(aContext->thisObject().toQObject());
  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);

  return buffer->typeDataObject(engine);
}

QScriptValue
BScriptEngine::funcPlay(QScriptContext* aContext,
                        QScriptEngine*)
{
  BBuffer* buffer = static_cast<BBuffer*>(aContext->thisObject().toQObject());

  if (aContext->argumentCount()) {
    BGeneratorRef generator = BGenerator::numberToGenerator(aContext->argument(0));
    if (!generator) {
      return aContext->throwError(QScriptContext::SyntaxError,
                                  "Wrong usage of play([generator]).");
    }

    buffer->setGeneratorData(generator);
  }

  buffer->play();
  return QScriptValue();
}

QScriptValue
BScriptEngine::funcStop(QScriptContext* aContext,
                        QScriptEngine*)
{
  BBuffer* buffer = static_cast<BBuffer*>(aContext->thisObject().toQObject());
  buffer->stop();
  return QScriptValue();
}

QScriptValue
BScriptEngine::funcIsPlaying(QScriptContext* aContext,
                             QScriptEngine*)
{
  BBuffer* buffer = static_cast<BBuffer*>(aContext->thisObject().toQObject());
  return QScriptValue(buffer->playing());
}

QScriptValue
BScriptEngine::funcSelect(QScriptContext* aContext,
                          QScriptEngine* aEngine)
{
  BBuffer* buffer = static_cast<BBuffer*>(aContext->thisObject().toQObject());
  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);
  engine->mApp->selectBuffer(buffer);
  return QScriptValue();
}

QScriptValue
BScriptEngine::funcLoop(QScriptContext* aContext,
                        QScriptEngine*)
{
  BBuffer* buffer = static_cast<BBuffer*>(aContext->thisObject().toQObject());

  if (aContext->argumentCount()) {
    buffer->setLoop(aContext->argument(0).toBool());
  }

  return QScriptValue(buffer->loop());
}

QScriptValue
BScriptEngine::funcSpeed(QScriptContext* aContext,
                        QScriptEngine* aEngine)
{
  BBuffer* buffer = static_cast<BBuffer*>(aContext->thisObject().toQObject());

  if (aContext->argumentCount()) {
    BGeneratorRef generator = BGenerator::numberToGenerator(aContext->argument(0));
    if (!generator) {
      return aContext->throwError(QScriptContext::SyntaxError,
                                  "buffer.speed = value - value must be a number or a generator");
    }

    buffer->setSpeed(generator);
  }

  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);
  return QScriptValue(buffer->speed()->objGenerator(engine));
}

// GLOBAL FUNCTIONS -----------------------------------------------------------

QScriptValue
BScriptEngine::funcDump(QScriptContext* aContext,
                        QScriptEngine* aEngine)
{
  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);
  engine->mApp->printMessage(aContext->argument(0).toString());
  return QScriptValue();
}

#ifdef BA_UNITTEST
QScriptValue
BScriptEngine::funcUnitTestIs(QScriptContext* aContext,
                              QScriptEngine*)
{
  if (aContext->argumentCount() < 2) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "BA_UT_is(arg1, arg2, [msg]) wrongly used.");
  }

  bool ret = aContext->argument(0).equals(aContext->argument(1));
  QString msg = aContext->argumentCount() < 3
                ? "" : aContext->argument(2).toString();

  std::cerr << "BAUNITTEST - " << (ret ? "SUCCESS" : "FAILURE") << ": "
            << qPrintable(aContext->argument(0).toString())
            << " == " << qPrintable(aContext->argument(1).toString())
            << ": " << qPrintable(msg) << std::endl;

  return QScriptValue();
}

QScriptValue
BScriptEngine::funcUnitTestOk(QScriptContext* aContext,
                              QScriptEngine*)
{
  if (aContext->argumentCount() < 1) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "BA_UT_ok(arg, [msg]) wrongly used.");
  }

  bool ret = aContext->argument(0).toBool();
  QString msg = aContext->argumentCount() < 2
                ? "" : aContext->argument(1).toString();

  std::cerr << "BAUNITTEST - " << (ret ? "SUCCESS" : "FAILURE") << ": "
            << qPrintable(aContext->argument(0).toString())
            << ": " << qPrintable(msg) << std::endl;

  return QScriptValue();
}
#endif

QScriptValue
BScriptEngine::funcSetTimeout(QScriptContext* aContext,
                              QScriptEngine* aEngine)
{
  if (aContext->argumentCount() < 2) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "setTimeout(function() { ... }, <timeout>) wrongly used.");
  }

  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);
  return engine->mApp->timer()->setTimeout(aContext,
                                           aContext->argument(0),
                                           aContext->argument(1).toInt32());
}

QScriptValue
BScriptEngine::funcClearTimeout(QScriptContext* aContext,
                                QScriptEngine* aEngine)
{
  if (aContext->argumentCount() < 1) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "clearTimeout(<id>) wrongly used.");
  }

  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);
  engine->mApp->timer()->clearTimeout(aContext->argument(0).toInt32());
  return QScriptValue();
}

QScriptValue
BScriptEngine::funcSetInterval(QScriptContext* aContext,
                               QScriptEngine* aEngine)
{
  if (aContext->argumentCount() < 2) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "setInterval(function() { ... }, <timeout>) wrongly used.");
  }

  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);
  return engine->mApp->timer()->setInterval(aContext,
                                            aContext->argument(0),
                                            aContext->argument(1).toInt32());
}

QScriptValue
BScriptEngine::funcClearInterval(QScriptContext* aContext,
                                 QScriptEngine* aEngine)
{
  if (aContext->argumentCount() < 1) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "clearInterval(<id>) wrongly used.");
  }

  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);
  engine->mApp->timer()->clearInterval(aContext->argument(0).toInt32());
  return QScriptValue();
}

QScriptValue
BScriptEngine::funcBuffers(QScriptContext*,
                           QScriptEngine* aEngine)
{
  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);

  if (engine->mObjBuffers.isUndefined()) {
    QList<BBuffer*>& buffers = engine->mApp->buffers();

    engine->mObjBuffers = engine->newArray();
    for (int i = 0; i < buffers.length(); ++i) {
      QScriptValue obj = makeBufferObject(engine, buffers[i]);
      engine->mObjBuffers.setProperty(i, obj);
    }
  }

  return engine->mObjBuffers;
}

QScriptValue
BScriptEngine::funcQuit(QScriptContext*,
                        QScriptEngine* aEngine)
{
  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);
  engine->mApp->quit();
  return QScriptValue();
}

QScriptValue
BScriptEngine::funcCurrentBuffer(QScriptContext* aContext,
                                 QScriptEngine* aEngine)
{
  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);

  // Initialize the buffer obj
  if (engine->mObjBuffers.isUndefined()) {
    funcBuffers(aContext, aEngine);
  }

  QList<BBuffer*>& buffers = engine->mApp->buffers();
  for (int i = 0; i < buffers.length(); ++i) {
    if (buffers[i] == engine->mApp->currentBuffer()) {
      return engine->mObjBuffers.property(i);
    }
  }

  return QScriptValue();
}

QScriptValue
BScriptEngine::makeBufferObject(BScriptEngine* aEngine,
                                BBuffer* aBuffer)
{
  QScriptValue obj = aEngine->newQObject(aBuffer);

  // void play()
  obj.setProperty("play", aEngine->newFunction(funcPlay));

  // dataType
  obj.setProperty("dataType", aEngine->newFunction(funcDataType),
    QScriptValue::PropertyGetter | QScriptValue::ReadOnly);

  // void stop()
  obj.setProperty("stop", aEngine->newFunction(funcStop));

  // bool playing
  obj.setProperty("playing", aEngine->newFunction(funcIsPlaying),
    QScriptValue::PropertyGetter | QScriptValue::ReadOnly);

  // void select()
  obj.setProperty("select", aEngine->newFunction(funcSelect));

  // void loop
  obj.setProperty("loop", aEngine->newFunction(funcLoop),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);

  // void speed
  obj.setProperty("speed", aEngine->newFunction(funcSpeed),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);

  // effects, volume & C
  makeEngine(aEngine, obj);

  return obj;
}

QScriptValue
BScriptEngine::funcRecStart(QScriptContext*,
                            QScriptEngine* aEngine)
{
  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);
  engine->mApp->audio().recStart();
  return QScriptValue();
}

QScriptValue
BScriptEngine::funcRecStop(QScriptContext* aContext,
                           QScriptEngine* aEngine)
{
  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);

  size_t size;
  double* data = engine->mApp->audio().recStop(&size);

  if (!aContext->argumentCount()) {
    free(data);
  } else {
    BBuffer* buffer = static_cast<BBuffer*>(aContext->argument(0).toQObject());

    if (!buffer) {
      return aContext->throwError(QScriptContext::SyntaxError,
                                  "recStop(buffer) wrongly used.");
    }

    buffer->setRecData(data, size);
    buffer->setTypeData(BBuffer::RecData);
  }

  return QScriptValue();
}

QScriptValue
BScriptEngine::funcIsRecording(QScriptContext*,
                               QScriptEngine* aEngine)
{
  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);
  return QScriptValue(engine->mApp->audio().recording());
}

// ENGINE FUNCTIONS -----------------------------------------------------------

QScriptValue
BScriptEngine::funcVolume(QScriptContext* aContext,
                          QScriptEngine* aEngine)
{
  BBuffer* buffer = static_cast<BBuffer*>(aContext->thisObject().toQObject());
  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);

  BEngine& audioEngine = buffer ? buffer->engine()
                                : engine->mApp->audio().engine();

  if (aContext->argumentCount()) {
    BGeneratorRef generator = BGenerator::numberToGenerator(aContext->argument(0));
    if (!generator) {
      return aContext->throwError(QScriptContext::SyntaxError,
                                  "volume = value - value must be a number or a generator");
    }

    audioEngine.setVolume(generator);
  }

  BGeneratorRef obj = audioEngine.volume();
  return QScriptValue(obj->objGenerator(engine));
}

QScriptValue
BScriptEngine::funcVolumes(QScriptContext* aContext,
                           QScriptEngine* aEngine)
{
  BBuffer* buffer = static_cast<BBuffer*>(aContext->thisObject().toQObject());
  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);

  QScriptValue array;

  if (buffer) {
    array = engine->newQObject(buffer);
  } else {
    array = engine->newObject();
  }

  // Array of volumes
  for (int i = 0; i < maxiSettings::channels; ++i) {
    QScriptValue value = aEngine->newFunction(funcChannelVolume);
    value.setData(i);

    array.setProperty(i, value,
                      QScriptValue::PropertyGetter | QScriptValue::PropertySetter);
  }

  // methods
  array.setProperty("length", aEngine->newFunction(funcVolumeLength),
                    QScriptValue::PropertyGetter | QScriptValue::ReadOnly);

  return array;
}

QScriptValue
BScriptEngine::funcChannelVolume(QScriptContext* aContext,
                                 QScriptEngine* aEngine)
{
  QScriptValue value = aContext->callee();
  int id = value.data().toInt32();

  BBuffer* buffer = static_cast<BBuffer*>(aContext->thisObject().toQObject());
  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);

  BEngine& audioEngine = buffer ? buffer->engine()
                                : engine->mApp->audio().engine();

  if (aContext->argumentCount() > 0) {
    BGeneratorRef generator = BGenerator::numberToGenerator(aContext->argument(0));
    if (!generator) {
      return aContext->throwError(QScriptContext::SyntaxError,
                                  "volume[id] = value - value must be a number or a generator");
    }

    audioEngine.setVolume(generator, id);
  }

  BGeneratorRef obj = audioEngine.volume(id);
  return QScriptValue(obj->objGenerator(engine));
}

QScriptValue
BScriptEngine::funcVolumeLength(QScriptContext*,
                                QScriptEngine*)
{
  return QScriptValue(maxiSettings::channels);
}

QScriptValue
BScriptEngine::funcFilters(QScriptContext* aContext,
                           QScriptEngine* aEngine)
{
  BBuffer* buffer = static_cast<BBuffer*>(aContext->thisObject().toQObject());
  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);

  BEngine& audioEngine = buffer ? buffer->engine()
                                : engine->mApp->audio().engine();

  QScriptValue array = audioEngine.objFilters();

  if (audioEngine.objFilters().isUndefined()) {
    if (buffer) {
      array = engine->newQObject(buffer);
    } else {
      array = engine->newObject();
    }

    // methods
    array.setProperty("length", aEngine->newFunction(funcFilterLength),
                      QScriptValue::PropertyGetter | QScriptValue::ReadOnly);
    array.setProperty("find", aEngine->newFunction(funcFilterFind));
    array.setProperty("push", aEngine->newFunction(funcFilterPush));
    array.setProperty("pop", aEngine->newFunction(funcFilterPop));
    array.setProperty("reverse", aEngine->newFunction(funcFilterReverse));
    array.setProperty("shift", aEngine->newFunction(funcFilterShift));
    array.setProperty("sort", aEngine->newFunction(funcFilterNotImplemented));
    array.setProperty("splice", aEngine->newFunction(funcFilterNotImplemented));
    array.setProperty("unshift", aEngine->newFunction(funcFilterUnshift));

    audioEngine.setObjFilters(array);
  }

  audioEngine.refreshObjFilters(engine);
  return array;
}

QScriptValue
BScriptEngine::funcFilter(QScriptContext* aContext,
                          QScriptEngine* aEngine)
{
  QScriptValue value = aContext->callee();
  int id = value.data().toInt32();

  BBuffer* buffer = static_cast<BBuffer*>(aContext->thisObject().toQObject());
  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);

  BEngine& audioEngine = buffer ? buffer->engine()
                                : engine->mApp->audio().engine();
  if (id < 0 || id >= audioEngine.filters().length()) {
    return QScriptValue();
  }

  BEngineFilterShell* filter = static_cast<BEngineFilterShell*>(aContext->argument(0).toQObject());

  if (aContext->argumentCount() > 0) {
    if (!filter) {
      return aContext->throwError(QScriptContext::SyntaxError,
                                  "wrong access to filter object.");
    }

    audioEngine.replaceFilter(id, filter->get());
  }

  return audioEngine.filters()[id]->objFilter(engine);
}


QScriptValue
BScriptEngine::funcFilterLength(QScriptContext* aContext,
                                QScriptEngine* aEngine)
{
  BBuffer* buffer = static_cast<BBuffer*>(aContext->thisObject().toQObject());
  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);

  BEngine& audioEngine = buffer ? buffer->engine()
                                : engine->mApp->audio().engine();

  return QScriptValue(audioEngine.lengthFilters());
}

QScriptValue
BScriptEngine::funcFilterFind(QScriptContext* aContext,
                              QScriptEngine* aEngine)
{
  BBuffer* buffer = static_cast<BBuffer*>(aContext->thisObject().toQObject());
  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);

  BEngine& audioEngine = buffer ? buffer->engine()
                                : engine->mApp->audio().engine();

  if (aContext->argumentCount() < 1) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "find(string) wrongly used.");
  }

  QString name = aContext->argument(0).toString();

  BEngineFilterRef filter = audioEngine.findFilter(name);
  if (filter) {
    return filter->objFilter(engine);
  }

  return QScriptValue(QScriptValue::NullValue);
}

QScriptValue
BScriptEngine::funcFilterPush(QScriptContext* aContext,
                              QScriptEngine* aEngine)
{
  if (aContext->argumentCount() < 1) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "push(filter) wrongly used.");
  }

  BBuffer* buffer = static_cast<BBuffer*>(aContext->thisObject().toQObject());
  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);

  BEngine& audioEngine = buffer ? buffer->engine()
                                : engine->mApp->audio().engine();

  BEngineFilterShell* filter = static_cast<BEngineFilterShell*>(aContext->argument(0).toQObject());
  if (!filter) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "push(filter) wrongly used.");
  }

  audioEngine.pushFilter(filter->get());
  return QScriptValue(QScriptValue::NullValue);
}

QScriptValue
BScriptEngine::funcFilterPop(QScriptContext* aContext,
                             QScriptEngine* aEngine)
{
  BBuffer* buffer = static_cast<BBuffer*>(aContext->thisObject().toQObject());
  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);

  BEngine& audioEngine = buffer ? buffer->engine()
                                : engine->mApp->audio().engine();

  BEngineFilterRef filter = audioEngine.popFilter();
  if (!filter) {
    return QScriptValue(QScriptValue::NullValue);
  }

  return filter->objFilter(engine);
}

QScriptValue
BScriptEngine::funcFilterReverse(QScriptContext* aContext,
                                 QScriptEngine* aEngine)
{
  BBuffer* buffer = static_cast<BBuffer*>(aContext->thisObject().toQObject());
  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);

  BEngine& audioEngine = buffer ? buffer->engine()
                                : engine->mApp->audio().engine();

  audioEngine.reverseFilters();
  return QScriptValue(QScriptValue::NullValue);
}

QScriptValue
BScriptEngine::funcFilterShift(QScriptContext* aContext,
                               QScriptEngine* aEngine)
{
  BBuffer* buffer = static_cast<BBuffer*>(aContext->thisObject().toQObject());
  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);

  BEngine& audioEngine = buffer ? buffer->engine()
                                : engine->mApp->audio().engine();

  BEngineFilterRef filter = audioEngine.shiftFilter();
  if (!filter) {
    return QScriptValue(QScriptValue::NullValue);
  }

  return filter->objFilter(engine);
}

QScriptValue
BScriptEngine::funcFilterNotImplemented(QScriptContext* aContext,
                                        QScriptEngine*)
{
  return aContext->throwError(QScriptContext::SyntaxError,
                              "This method is not implemented.");
}

QScriptValue
BScriptEngine::funcFilterUnshift(QScriptContext* aContext,
                                 QScriptEngine* aEngine)
{
  if (aContext->argumentCount() < 1) {
    return aContext->throwError(QScriptContext::SyntaxError,
                                "unshift(filter, [..filters]) wrongly used.");
  }

  BBuffer* buffer = static_cast<BBuffer*>(aContext->thisObject().toQObject());
  BScriptEngine* engine = static_cast<BScriptEngine*>(aEngine);

  BEngine& audioEngine = buffer ? buffer->engine()
                                : engine->mApp->audio().engine();

  for (int i = 0; i < aContext->argumentCount(); ++i) {
    BEngineFilterShell* filter = static_cast<BEngineFilterShell*>(aContext->argument(i).toQObject());
    if (!filter) {
      return aContext->throwError(QScriptContext::SyntaxError,
                                  "unshift(filter, [..filters]) wrongly used.");
    }

    audioEngine.addFilter(filter->get());
  }

  return QScriptValue(QScriptValue::NullValue);
}
