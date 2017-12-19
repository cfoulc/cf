SLUG = cf
VERSION = 0.5.10

FLAGS += -D v050

SOURCES = $(wildcard src/*.cpp)

include ../../plugin.mk

dist: all
	mkdir -p dist/cf
	cp LICENSE* dist/cf/
	cp plugin.* dist/cf/
	cp -R res dist/cf/
