#ifndef TONEWHEEL_H_INCLUDED
#define TONEWHEEL_H_INCLUDED

#include	<assert.h>
#include	<math.h>
#include	<iostream>

using namespace std;

class Tonewheel {

	static const int WHEEL_COUNT	= 91;
	static const long WHEEL_MASK	= 0x0000FFFFFFFFFFFFL; // 48 bits
	static const int WHEEL_SHIFT	= 30;
	static const long WHEEL_SCALE	= 0x03FFFF+1; // 18 bits

	long int bump[WHEEL_COUNT];
	long int phase[WHEEL_COUNT];
	float phase2[WHEEL_COUNT];

	int SAMPLE_RATE;

    public:
	Tonewheel(int sample_rate = 44100) : SAMPLE_RATE(sample_rate){
		const static float freq[] = {
			 32.6923, 34.6341, 36.7123, 38.8889, 41.2000,
			 43.6364, 46.2500, 49.0000, 51.8919, 55.0000,
			 58.2609, 61.7143, 65.3846, 69.2683, 73.4247,
			 77.7778, 82.4000, 87.2727, 92.5000, 98.0000,
			 103.7838, 110.0000, 116.5217, 123.4286, 130.7692,
			 138.5366, 146.8493, 155.5556, 164.8000, 174.5455,
			 185.0000, 196.0000, 207.5676, 220.0000, 233.0435,
			 246.8571, 261.5385, 277.0732, 293.6986, 311.1111,
			 329.6000, 349.0909, 370.0000, 392.0000, 415.1351,
			 440.0000, 466.0870, 493.7143, 523.0769, 554.1463,
			 587.3973, 622.2222, 659.2000, 698.1818, 740.0000,
			 784.0000, 830.2703, 880.0000, 932.1739, 987.4286,
			 1046.1538, 1108.2927, 1174.7945, 1244.4444,
			 1318.4000, 1396.3636, 1480.0000, 1568.0000,
			 1660.5405, 1760.0000, 1864.3478, 1974.8571,
			 2092.3077, 2216.5854, 2349.5890, 2488.8889,
			 2636.8000, 2792.7273, 2960.0000, 3136.0000,
			 3321.0811, 3520.0000, 3728.6957, 3949.7143,
			 4189.0909, 4440.0000, 4704.0000, 4981.6216,
			 5280.0000, 5593.0435, 5924.5714

		};

		const int WHEEL_COUNT_CHECK = (sizeof(freq)/sizeof(freq[0]));
		assert(WHEEL_COUNT_CHECK == WHEEL_COUNT);
		assert( ((WHEEL_MASK>>WHEEL_SHIFT)+1) == WHEEL_SCALE);

		//cout << "WHEEL_MASK " << WHEEL_MASK << endl;
		//cout << "WHEEL_MASK>>30 " << WHEEL_MASK>>30 ) << endl;
		//cout << "size of phase[0] is " << sizeof(phase[0]) << endl;

		for(int i = 0; i < WHEEL_COUNT; i++) {
			bump[i] = freq[i] * 2 * M_PI * WHEEL_SCALE
				  / SAMPLE_RATE;
			//cout << ("bump[%d] = 0x%lx.\n"<< i << bump[i] << endl;
		}
	}

	void update() {
		for(int i = 0; i < WHEEL_COUNT; i++) {
			phase[i] += bump[i];
			phase[i] &= WHEEL_MASK;
			phase2[i] = phase[i];
			phase2[i] /= WHEEL_SCALE;
		}
	}

	int get_wheel_count() { return WHEEL_COUNT; }
	int get_sample_rate() { return SAMPLE_RATE; }
	long get_bump(int i) { return bump[i]; }
	long get_phase(int i) { return phase[i]; }
	float get_phase2(int i) { return phase2[i]; }
};

extern Tonewheel tonewheel;

#endif // TONEWHEEL_H_INCLUDED
