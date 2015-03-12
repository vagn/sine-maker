

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<math.h>
#include	<assert.h>

#include	<sndfile.h>

#include	<string>
#include	<vector>
#include	<algorithm>

#include	"adsr.h"
#include	"sample.h"
#include	"tonewheel.h"

#define		SAMPLE_RATE		44100
#define		CLICK_GATE		32000
#define		SAMPLE_COUNT		(SAMPLE_RATE * 12)	/* 6 seconds */
#define		AMPLITUDE		(1.0 * 0x7F000000)

Tonewheel tonewheel(SAMPLE_RATE);
// fixme: adsr should be given in millisec
ADSR adsr(SAMPLE_RATE, CLICK_GATE);

int song[] = {
	0, 1, 2, 3, 4, 12, 13, 16, 22, 8, 16, 22, 8, 9, 10, 11, 9, 10,
	11, 14, 15, 16, 22, 8, 9, 10, 11, 23, 24, 5, 6, 7, 8, 9, 10,
	11, 8, 9, 10, 11, 17, 18, 19, 20, 21, 3, 4, 12, 13, 3, 4, 12, 13,

};

class keyx {
    public:
	int note;
	int flag;
	ADSR_state ramp;
	
	keyx(){
		adsr.key_init(ramp);
	}

};

keyx key[61];
std::vector<keyx *> kv;
typedef std::vector<keyx *>::iterator kvi;

keyx&
get_melody(int k)
{
	static keyx kx;
	static int beat = SAMPLE_RATE / 8;
	static int index = 0;
	static int size = sizeof(song)/sizeof(song[0]);

	kx.flag = 0;

	if(0 == (k % beat)) {
		index = ( index + 3) % size;
		kx.flag = 1;
	}
	kx.note = song[index];
	return kx;
}

int
get_sample(int note)
{
	float left_freq = tonewheel.get_phase2(note);

	return AMPLITUDE * ( sin (left_freq * 1)) / 10 ;
}

int
get_upper(int note)
{
	return get_sample(note)
	     + get_sample(note+7)
	     + get_sample(note+10)
	     + get_sample(note+14);
}

int
get_lower(int note)
{
	return get_sample(note)
	     + get_sample(note+3)
	     + get_sample(note+10);
}

float
beep_boop()
{
	tonewheel.update();

	float value = 0;
	for(kvi i = kv.begin(); i != kv.end(); i++) {
		adsr.update((*i)->ramp);
		float a = adsr.lookup((*i)->ramp);
		value += get_upper(20 + (*i)->note) * a;
	}

	for(kvi i = kv.begin(); i != kv.end(); i++) {
		if((*i)->ramp.count == 0) {
			kv.erase(i);
			break;
		}
	}

	return value;
}

int
main(int argc, char **argv)
{
	Sample sample("sine.wav");

	for(int i = 0; i < 61; i++) {
		key[i].note = i + 13;
	}

	int last_note = 0;

	for (int k = 0 ; k < SAMPLE_COUNT ; k++) {

		keyx melody = get_melody(k);
		//printf("note is: %d\n", melody.note);

		if(melody.flag) {
			adsr.key_up(key[last_note].ramp);
			adsr.key_down(key[melody.note].ramp);
			// fixme: just check if it is already sounding
			if(melody.note != last_note) {
				kv.push_back(&key[melody.note]);
			}
			last_note = melody.note;
			//printf("last_note is: %d\n", last_note);
		}

		float value = beep_boop();
		sample.write(value, value);
	}


	for(kvi i = kv.begin(); i != kv.end(); i++) {
		adsr.key_up((*i)->ramp);
	}

	while (kv.size() > 0) {
		float value = beep_boop();
		sample.write(value, value);
	}

	return	 0 ;
}

