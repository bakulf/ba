TEMPLATE = subdirs

TESTS = \
  001_base.config \
  002_buffers.config \
  003_globals.config \
  004_timers.config \
  005_rec.config \
  006_filters.config \
  007_noise.config \
  008_bandpassfilter.config \
  009_chorusfilter.config \
  010_compressorfilter.config \
  011_delaylinefilter.config \
  012_atandistortionfilter.config \
  013_fastatandistortionfilter.config \
  014_flangerfilter.config \
  015_gatefilter.config \
  016_hipassfilter.config \
  017_hiresfilter.config \
  018_lopassfilter.config \
  019_loresfilter.config

for(test, TESTS) {
  system("./unittest.sh $$test")
}
