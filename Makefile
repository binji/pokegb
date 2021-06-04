pokegb: pokegb.cc
	$(CC) -O2 -Wall -Wno-return-type -Wno-misleading-indentation -o $@ $< -lSDL2 

clean:
	rm -f pokegb
