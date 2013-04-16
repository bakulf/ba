TEMPLATE = subdirs

TESTS = \
  001_base.config \
  002_buffers.config \
  003_globals.config \
  004_timers.config \
  005_rec.config \
  006_filters.config \
  007_noise.config

for(test, TESTS) {
  system("./unittest.sh $$test")
}
