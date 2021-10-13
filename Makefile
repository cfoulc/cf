SOURCES += $(wildcard src/*.cpp)

DISTRIBUTABLES += $(wildcard LICENSE*) res
DISTRIBUTABLES += $(wildcard LICENSE*) playeroscs

RACK_DIR ?= ../..
include $(RACK_DIR)/plugin.mk
