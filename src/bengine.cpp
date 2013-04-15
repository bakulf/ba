#include "bengine.h"
#include "bapplication.h"
#include "bmutexlocker.h"
#include "bscriptengine.h"

#include "bchorusfilter.h"
#include "bloresfilter.h"
#include "bhiresfilter.h"
#include "bbandpassfilter.h"
#include "bhipassfilter.h"
#include "blopassfilter.h"
#include "bdelaylinefilter.h"
#include "bgatefilter.h"
#include "bcompressorfilter.h"
#include "bdistortionfilter.h"
#include "bflangerfilter.h"

#include "maximilian.h"

#include <QStringList>

#include <assert.h>

BEngine::BEngine()
: mObjFilters(QScriptValue::UndefinedValue)
, mOldFiltersLength(0)
{
  mVolumes = new double[maxiSettings::channels];

  for (int i = 0; i < maxiSettings::channels; ++i) {
    mVolumes[i] = 1.0;
  }
}

BEngine::~BEngine()
{
  delete[] mVolumes;
}

double
BEngine::volume() const
{
  double volume = 0.0;

  for (int i = 0; i < maxiSettings::channels; ++i) {
    volume += mVolumes[i];
  }

  return volume / maxiSettings::channels;
}

void
BEngine::setVolume(double aVolume)
{
  if (aVolume < 0 || aVolume > 1) {
    return;
  }

  for (int i = 0; i < maxiSettings::channels; ++i) {
    mVolumes[i] = aVolume;
  }
}

double
BEngine::volume(int aChannel) const
{
  if (aChannel < 0 || aChannel >= maxiSettings::channels) {
    return 0;
  }

  return mVolumes[aChannel];
}

void
BEngine::setVolume(double aVolume, int aChannel)
{
  if (aVolume < 0 || aVolume > 1) {
    return;
  }

  if (aChannel < 0 || aChannel >= maxiSettings::channels) {
    return;
  }

  mVolumes[aChannel] = aVolume;
}

void
BEngine::pushFilter(BEngineFilter* aFilter)
{
  mFilters << aFilter;
}

BEngineFilter*
BEngine::popFilter()
{
  if (mFilters.isEmpty()) {
    return NULL;
  }

  BEngineFilter* filter = mFilters.last();
  mFilters.removeLast();
  return filter;
}

BEngineFilter*
BEngine::shiftFilter()
{
  if (mFilters.isEmpty()) {
    return NULL;
  }

  BEngineFilter* filter = mFilters.first();
  mFilters.removeFirst();
  return filter;
}

void
BEngine::addFilter(BEngineFilter* aFilter)
{
  mFilters.prepend(aFilter);
}

void
BEngine::replaceFilter(int aId, BEngineFilter* aFilter)
{
  if (aId < 0 || aId >= mFilters.length()) {
    return;
  }

  mFilters.replace(aId, aFilter);
}

BEngineFilter*
BEngine::findFilter(QString& aName)
{
  foreach (BEngineFilter* filter, mFilters) {
    if (filter->name() == aName) {
      return filter;
    }
  }

  return NULL;
}

void
BEngine::reverseFilters()
{
  QList<BEngineFilter*> filters;
  for (int i = mFilters.length() - 1; i >= 0; --i) {
    filters.append(mFilters[i]);
  }

  mFilters = filters;
}

uint
BEngine::lengthFilters()
{
  return mFilters.length();
}

void
BEngine::refreshObjFilters(QScriptEngine* aEngine)
{
  int id;
  for (id = 0; id < mFilters.length(); ++id) {
    QScriptValue obj = aEngine->newFunction(BScriptEngine::funcFilter);
    obj.setData(id);

    mObjFilters.setProperty(id, obj,
                            QScriptValue::PropertyGetter | QScriptValue::PropertySetter);
  }

  for (; id < mOldFiltersLength; ++id) {
    QScriptValue value(QScriptValue::UndefinedValue);
    mObjFilters.setProperty(id, value);
  }

  // Backup
  mOldFiltersLength = mFilters.length();
}

void
BEngine::output(double *aOutput)
{
  foreach (BEngineFilter* filter, mFilters) {
    if (filter->enabled()) {
      filter->output(aOutput);
    }
  }

  for (int i = 0; i < maxiSettings::channels; ++i) {
    aOutput[i] *= mVolumes[i];
  }
}

void
BEngine::filterFactory(QScriptEngine* aEngine)
{
  // Chorus
  QScriptValue chorusProto = aEngine->newObject();
  aEngine->globalObject().setProperty("Chorus",
  aEngine->newFunction(BChorusFilter::engineFunction, chorusProto));

  // Lores
  QScriptValue loresProto = aEngine->newObject();
  aEngine->globalObject().setProperty("Lores",
    aEngine->newFunction(BLoresFilter::engineFunction, loresProto));

  // Hires
  QScriptValue hiresProto = aEngine->newObject();
  aEngine->globalObject().setProperty("Hires",
    aEngine->newFunction(BHiresFilter::engineFunction, hiresProto));

  // Bandpass
  QScriptValue bandpassProto = aEngine->newObject();
  aEngine->globalObject().setProperty("Bandpass",
    aEngine->newFunction(BBandpassFilter::engineFunction, bandpassProto));

  // HipasHis
  QScriptValue hipassProto = aEngine->newObject();
  aEngine->globalObject().setProperty("Hipass",
    aEngine->newFunction(BLopassFilter::engineFunction, hipassProto));

  // Lopass
  QScriptValue lopassProto = aEngine->newObject();
  aEngine->globalObject().setProperty("Lopass",
    aEngine->newFunction(BLopassFilter::engineFunction, lopassProto));

  // Delayline
  QScriptValue delaylineProto = aEngine->newObject();
  aEngine->globalObject().setProperty("Delayline",
    aEngine->newFunction(BDelaylineFilter::engineFunction, delaylineProto));

  // Gate
  QScriptValue gateProto = aEngine->newObject();
  aEngine->globalObject().setProperty("Gate",
    aEngine->newFunction(BGateFilter::engineFunction, gateProto));

  // Compressor
  QScriptValue compressorProto = aEngine->newObject();
  aEngine->globalObject().setProperty("Compressor",
    aEngine->newFunction(BCompressorFilter::engineFunction, compressorProto));

  // AtanDistortion
  QScriptValue atandistortionProto = aEngine->newObject();
  aEngine->globalObject().setProperty("AtanDistortion",
    aEngine->newFunction(BDistortionFilter::engineAtanFunction, atandistortionProto));

  // FastAtanDistortion
  QScriptValue fastatandistortionProto = aEngine->newObject();
  aEngine->globalObject().setProperty("FastAtanDistortion",
    aEngine->newFunction(BDistortionFilter::engineFastAtanFunction, fastatandistortionProto));

  // Flanger
  QScriptValue flangerProto = aEngine->newObject();
  aEngine->globalObject().setProperty("Flanger",
    aEngine->newFunction(BFlangerFilter::engineFunction, fastatandistortionProto));

  // Other effects TODO
  // . maxiEnv
  // . maxiEnvelope
}

QString
BEngine::writeVolume(BEngine& aEngine)
{
  QString str;

  str.sprintf("%1.2f [", aEngine.volume());

  for (int i = 0; i < maxiSettings::channels; ++i) {
    char tmp[1024];
    snprintf(tmp, sizeof(tmp), "%s%1.2f", (i ? ", " : ""),
             aEngine.volume(i));
    str.append(tmp);
  }

  str.append("]");
  return str;
}

QStringList
BEngine::writeFilters(BEngine& aEngine)
{
  QStringList lines;

  foreach (BEngineFilter* filter, aEngine.filters()) {
    lines << filter->writeFilter();
  }

  return lines;
}

BEngineFilter::BEngineFilter(QObject* aParent, QString aName)
: QObject(aParent)
, mName(aName)
, mEnabled(true)
, mObjFilter(QScriptValue::UndefinedValue)
{
}

BEngineFilter::~BEngineFilter()
{
}

QScriptValue
BEngineFilter::objFilter(BScriptEngine* aEngine)
{
  if (mObjFilter.isUndefined()) {
    makeObjFilter(aEngine);
  }

  return mObjFilter;
}

void
BEngineFilter::makeObjFilter(BScriptEngine* aEngine)
{
  if (!mObjFilter.isUndefined()) {
    return;
  }

  QScriptValue obj = aEngine->newQObject(this);

  // name
  obj.setProperty("name", aEngine->newFunction(funcFilterName),
    QScriptValue::PropertyGetter | QScriptValue::ReadOnly);

  // update
  obj.setProperty("update", aEngine->newFunction(engineUpdateFunction()));

  // enabled
  obj.setProperty("enabled", aEngine->newFunction(funcFilterEnabled),
    QScriptValue::PropertyGetter | QScriptValue::PropertySetter);

  // Other properties
  engineProperties(aEngine, obj);

  mObjFilter = obj;
}

QScriptValue
BEngineFilter::funcFilterName(QScriptContext* aContext,
                              QScriptEngine*)
{
  BEngineFilter* filter = static_cast<BEngineFilter*>(aContext->thisObject().toQObject());
  return QScriptValue(filter->name());
}

QScriptValue
BEngineFilter::funcFilterEnabled(QScriptContext* aContext,
                                 QScriptEngine*)
{
  BEngineFilter* filter = static_cast<BEngineFilter*>(aContext->thisObject().toQObject());

  if (aContext->argumentCount()) {
    filter->setEnabled(aContext->argument(0).toBool());
  }

  return QScriptValue(filter->enabled());
}

QString
BEngineFilter::writeFilter()
{
  QString line;
  line.sprintf("Filter: %s", qPrintable(mName));
  return line;
}
