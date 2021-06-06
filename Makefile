pokegb: pokegb.cc
	$(CC) -O2 -Wall -Wno-return-type -Wno-misleading-indentation -Wno-parentheses -o $@ $< -lSDL2

rom.sav: empty.sav
	cp $< $@

clean:
	rm -f pokegb rom.sav
