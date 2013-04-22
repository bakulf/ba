TEMPLATE = subdirs

TESTS = \
  001_base.config \
  002_buffers.config \
  003_globals.config \
  004_timers.config \
  005_rec.config \
  006_filters.config \
  007_noise.config \
  008_deadlock.config \
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
  202_phasorgenerator.config \
  203_sawgenerator.config \
  204_trianglegenerator.config \
  205_squaregenerator.config \
  206_pulsegenerator.config \
  207_sinebufgenerator.config \
  208_sinebuf4generator.config \
  209_sawngenerator.config \
  210_rectgenerator.config \
  211_opgenerator.config

for(test, TESTS) {
  system("./unittest.sh $$test")
}
