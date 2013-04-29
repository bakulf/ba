TEMPLATE = subdirs
CONFIG  += ordered
SUBDIRS  = generators filters core

core.depends = generators filters
