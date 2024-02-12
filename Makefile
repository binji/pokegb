CXXFLAGS := -Os -s -Wall -Wno-return-type -Wno-misleading-indentation -Wno-parentheses
override CXXFLAGS += $(shell pkg-config sdl2 --cflags)
override LDLIBS += $(shell pkg-config sdl2 --libs)

all: pokegb deobfuscated

rom.sav: empty.sav
	cp $< $@

clean:
	$(RM) pokegb rom.sav
