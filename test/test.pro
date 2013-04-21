TEMPLATE = subdirs

TESTS = \
  001_base.config \
  002_buffers.config \
  003_globals.config \
  004_timers.config \
  005_rec.config \
  006_filters.config \
  007_noise.config \
  100_bandpassfilter.config \
  101_chorusfilter.config \
  102_compressorfilter.config \
  103_delaylinefilter.config \
  104_atondistortionfilter.config \
  105_fastatandistortionfilter.config \
  106_flangerfilter.config \
  107_gatefilter.config \
  108_hipassfilter.config \
  109_hiresfilter.config \
  110_lopassfilter.config \
  111_loresfilter.config \
  200_sinewavegenerator.config \
  201_coswavegenerator.config \
  202_phasorgenerator.config

for(test, TESTS) {
  system("./unittest.sh $$test")
}
