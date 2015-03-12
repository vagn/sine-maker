#ifndef ADSR_H_INCLUDED
#define ADSR_H_INCLUDED

#include	<math.h>
#include	<assert.h>

class ADSR_state {
    public:
	int count; // lookup to get weight
	int state;
};

class ADSR_unit {
    public:
	int lo;
	int step;
	int hi;

	void init(const int a_lo, const int a_step,
		  const int a_hi) {

		lo = a_lo;
		step = a_step;
		hi = a_hi;
	}

	bool lt_hi(const int n) { return n < hi; }
	bool gt_lo(const int n) { return n > lo; }
	bool gt_hi(const int n) { return n > hi; }
	bool lt_lo(const int n) { return n < lo; }
	bool ge_hi(const int n) { return n >= hi; }
	bool le_lo(const int n) { return n <= lo; }
	int inc(const int n) { return (ge_hi(n+step))?hi:n+step; }
	int dec(const int n) { return (lt_lo(n-step))?lo:n-step; }
};

class ADSR {
	int sample_rate;
	int ramp_time_ns;
	int ramp_max_count;
	float	*ramp;

	ADSR_unit adsr_a;
	ADSR_unit adsr_d;
	ADSR_unit adsr_s;
	ADSR_unit adsr_r;

    public:

	ADSR(int a_sample_rate = 44100, int a_ramp_time_ns = 15000) {
		sample_rate = a_sample_rate;
		ramp_time_ns = a_ramp_time_ns;
		ramp_max_count = (sample_rate/1000000.0)*a_ramp_time_ns;
		ramp = new float[ramp_max_count];

		ramp_munged_half_cos();
		//ramp_quarter_sine();
		//ramp_mirror_cosine();
		//ramp_linear();
		//ramp_zero();

		//          lo, step, hi
		int p4 = ramp_max_count - 1;
		int p3 = ((p4 / 4) * 3);
		int p2 = ((p4 / 4) * 2);
		int p1 = ((p4 / 4) * 1);
		int p0 = 0;

#if 1
		adsr_a.init(  0, 12, p4);
		adsr_d.init( p3,  8, p4);
		adsr_s.init( p3,  8, p3);
		adsr_r.init(  0,  1, p3);
#else
		adsr_a.init(  0, 6, p4);
		adsr_d.init( p4, 4, p4);
		adsr_s.init( p4, 4, p4);
		adsr_r.init(  0, 1, p4);
#endif
	}

	~ADSR(){ delete[] ramp; }

	void ramp_zero() {
		for(int count = 0; count < ramp_max_count; count++) {
			float a = count;
			a /= ramp_max_count;
			ramp[count] = 0.0;
		}
	}

	void ramp_linear() {
		for(int count = 0; count < ramp_max_count; count++) {
			float a = count;
			a /= ramp_max_count;
			ramp[count] = a;
		}
	}

	void ramp_munged_half_cos() {
		for(int count = 0; count < ramp_max_count; count++) {
			float a = count;
			a /= ramp_max_count;
			a *= M_PI;
			a += M_PI;
			ramp[count] = (1.0 + cos(a))/2;
		}

		ramp[0] = 0.0;
		ramp[ramp_max_count - 1] = 1.0;
	}

	void ramp_mirror_cosine() {
		for(int count = 0; count < ramp_max_count; count++) {
			float a = count;
			a /= ramp_max_count;
			a *= M_PI/2;
			ramp[count] = 1.0 - cos(a);
		}

		for(int count = 0; count < ramp_max_count/2; count++) {
			ramp[ramp_max_count - count] = 1.0 - ramp[count];
		}

		ramp[0] = 0.0;
	}

	void ramp_quarter_sine() {
		for(int count = 0; count < ramp_max_count; count++) {
			float a = count;
			a /= ramp_max_count;
			a *= M_PI/2;
			ramp[count] = sin(a);
		}
	}

	float lookup(ADSR_state &s) {
		return lookup(s.count);
	}

	float lookup(const int count) {
		if (count < 1) return ramp[0];
		if (count < ramp_max_count) return ramp[count];
		return ramp[ramp_max_count - 1];
	}

	int get_sample_rate() { return sample_rate; }
	int get_ramp_time_ns() { return ramp_time_ns; }
	int get_ramp_max_count() { return ramp_max_count; }

	void update(ADSR_state& s) {
		int n = s.count;
		switch(s.state) {
		case 'A':
			if(adsr_a.lt_hi(n)) { s.count = adsr_a.inc(n); }
			else { s.state = 'D'; }
			break;
		case 'D':
			if(adsr_d.gt_lo(n)) { s.count = adsr_d.dec(n); }
			else { s.state = 'S'; }
			break;
		case 'S':
			if(adsr_s.gt_hi(n)) { s.count = adsr_s.dec(n); }
			if(adsr_s.lt_lo(n)) { s.count = adsr_s.inc(n); }
			break;
		case 'R':
			if(adsr_r.gt_lo(n)) { s.count = adsr_r.dec(n); }
			else { s.state = 'Q'; }
			break;
		case 'Q':
			if(n > 0) { s.count--; }
			break;
		default:
			s.state = 'Q';
			break;
		}
	}

	void key_down(ADSR_state& s) { s.state = 'A'; update(s); }
	void key_up(ADSR_state& s) { s.state = 'R'; update(s); }
	void key_init(ADSR_state& s) { s.state = 'Q'; update(s); }
};

#endif // ADSR_H_INCLUDED
