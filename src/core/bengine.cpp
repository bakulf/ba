#include "bengine.h"
#include "bapplication.h"
#include "bmutexlocker.h"
#include "bscriptengine.h"
#include "bnumbergenerator.h"

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
  for (int i = 0; i < maxiSettings::channels; ++i) {
    mVolumes << new BNumberGenerator(1.0);
  }
}

BEngine::~BEngine()
{
}

BGenerator*
BEngine::volume()
{
  double volume = 0.0;

  for (int i = 0; i < maxiSettings::channels; ++i) {
    volume += mVolumes[i]->get();
  }

  return new BNumberGenerator(volume / maxiSettings::channels);
}

void
BEngine::setVolume(BGenerator* aVolume)
{
  for (int i = 0; i < maxiSettings::channels; ++i) {
    mVolumes[i] = aVolume;
  }
}

BGenerator*
BEngine::volume(int aChannel) const
{
  if (aChannel < 0 || aChannel >= maxiSettings::channels) {
    return 0;
  }

  return mVolumes[aChannel];
}

void
BEngine::setVolume(BGenerator* aVolume, int aChannel)
{
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

  BEngineFilterRef filter = mFilters.last();
  mFilters.removeLast();
  return filter;
}

BEngineFilter*
BEngine::shiftFilter()
{
  if (mFilters.isEmpty()) {
    return NULL;
  }

  BEngineFilterRef filter = mFilters.first();
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
  foreach (const BEngineFilterRef& filter, mFilters) {
    if (filter->name() == aName) {
      return filter;
    }
  }

  return NULL;
}

void
BEngine::reverseFilters()
{
  QList<BEngineFilterRef> filters;
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
  foreach (const BEngineFilterRef& filter, mFilters) {
    if (filter->enabled()) {
      filter->output(aOutput);
    }
  }

  for (int i = 0; i < maxiSettings::channels; ++i) {
    double volume = mVolumes[i]->get();
    if (volume < 0.0) {
      volume = 0.0;
    } else if(volume > 1.0) {
      volume = 1.0;
    }

    aOutput[i] *= volume;
  }
}

void
BEngine::filterFactory(QScriptEngine* aEngine)
{
  // Chorus
  aEngine->globalObject().setProperty("Chorus",
    aEngine->newFunction(BChorusFilter::engineFunction));

  // Lores
  aEngine->globalObject().setProperty("Lores",
    aEngine->newFunction(BLoresFilter::engineFunction));

  // Hires
  aEngine->globalObject().setProperty("Hires",
    aEngine->newFunction(BHiresFilter::engineFunction));

  // Bandpass
  aEngine->globalObject().setProperty("Bandpass",
    aEngine->newFunction(BBandpassFilter::engineFunction));

  // Hipass
  aEngine->globalObject().setProperty("Hipass",
    aEngine->newFunction(BHipassFilter::engineFunction));

  // Lopass
  aEngine->globalObject().setProperty("Lopass",
    aEngine->newFunction(BLopassFilter::engineFunction));

  // Delayline
  aEngine->globalObject().setProperty("Delayline",
    aEngine->newFunction(BDelaylineFilter::engineFunction));

  // Gate
  aEngine->globalObject().setProperty("Gate",
    aEngine->newFunction(BGateFilter::engineFunction));

  // Compressor
  aEngine->globalObject().setProperty("Compressor",
    aEngine->newFunction(BCompressorFilter::engineFunction));

  // AtanDistortion
  aEngine->globalObject().setProperty("AtanDistortion",
    aEngine->newFunction(BDistortionFilter::engineAtanFunction));

  // FastAtanDistortion
  aEngine->globalObject().setProperty("FastAtanDistortion",
    aEngine->newFunction(BDistortionFilter::engineFastAtanFunction));

  // Flanger
  aEngine->globalObject().setProperty("Flanger",
    aEngine->newFunction(BFlangerFilter::engineFunction));

  // Other effects FIXME
  // . maxiEnv
  // . maxiEnvelope
}

QString
BEngine::writeVolume(BEngine& aEngine)
{
  QString str;

  str.sprintf("%1.2f [", aEngine.volume()->get());

  for (int i = 0; i < maxiSettings::channels; ++i) {
    char tmp[1024];
    snprintf(tmp, sizeof(tmp), "%s%1.2f", (i ? ", " : ""),
             aEngine.volume(i)->get());
    str.append(tmp);
  }

  str.append("]");
  return str;
}

QStringList
BEngine::writeFilters(BEngine& aEngine)
{
  QStringList lines;

  foreach (const BEngineFilterRef& filter, aEngine.filters()) {
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

  BEngineFilterShell* shell = new BEngineFilterShell(this);
  QScriptValue obj = aEngine->newQObject(shell, QScriptEngine::ScriptOwnership);

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
  BEngineFilterShell* shell = static_cast<BEngineFilterShell*>(aContext->thisObject().toQObject());
  return QScriptValue(shell->get()->name());
}

QScriptValue
BEngineFilter::funcFilterEnabled(QScriptContext* aContext,
                                 QScriptEngine*)
{
  BEngineFilterShell* shell = static_cast<BEngineFilterShell*>(aContext->thisObject().toQObject());
  BEngineFilterRef filter = shell->get();

  if (aContext->argumentCount()) {
    filter->setEnabled(aContext->argument(0).toBool());
  }

  return QScriptValue(filter->enabled());
}

QString
BEngineFilter::writeFilter()
{
  return QString("Filter: %1").arg(mName);
}
