
LIBS   := $(shell pkg-config --libs sndfile) -lm
CFLAGS := $(shell pkg-config --cflags sndfile)
SRC    := main.cc
HDR    := tonewheel.h

target := app

redo: realclean $(target) sine.wav play

play: sine.wav
	play $<
	
sine.wav: $(target)
	./$(target)

$(target): $(SRC) $(HDR)
	g++ $(CFLAGS) -o $@ $(LIBS) $(SRC)


clean:
	rm -f $(target) sine.wav

realclean: clean
	rm -f core
