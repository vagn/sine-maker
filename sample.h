#ifndef SAMPLE_H_INCLUDED
#define SAMPLE_H_INCLUDED

//#include	<stdio.h>
//#include	<stdlib.h>
//#include	<string.h>
//#include	<math.h>
//#include	<assert.h>

#include	<iostream>
#include	<string>
#include	<sndfile.h>

class Sample {
    public:
	SNDFILE	*file ;
	SF_INFO	sfinfo ;
	int	*buffer ;
	size_t buffer_size;
	size_t buffer_index;

	int samplerate;
	int channels;
	int format;

	Sample(const char *filename,
		const int a_sample_rate = 44100,
		const int a_channels = 2,
		const int a_format = (SF_FORMAT_WAV | SF_FORMAT_PCM_24))
	{
		init(filename, a_sample_rate, a_channels, a_format);
	}

	Sample(const std::string& filename,
		const int a_sample_rate = 44100,
		const int a_channels = 2,
		const int a_format = (SF_FORMAT_WAV | SF_FORMAT_PCM_24))
	{
		init(filename.c_str(), a_sample_rate, a_channels, a_format);
	}

	void init(const char *filename,
		const int a_sample_rate = 44100,
		const int a_channels = 2,
		const int a_format = (SF_FORMAT_WAV | SF_FORMAT_PCM_24))
	{

		buffer_size = 2048;
		samplerate = a_sample_rate;
		channels = a_channels;
		format = a_format;

		sfinfo.samplerate = samplerate;
		sfinfo.channels = channels;
		sfinfo.format = format;

		if (! (file = sf_open ("sine.wav", SFM_WRITE, &sfinfo)))
		{
			printf ("Error : Not able to open output file.\n") ;
			exit(1);
		}

		dump();

		buffer_size = buffer_size * sizeof(int) * channels;
		buffer = new int[buffer_size];
		buffer_index = 0;

		printf("done with init\n");
	}

	~Sample(){
		flush_buffer();
		sf_close (file) ;
		delete[] buffer;
	}

	void flush_buffer() {
		if (sf_write_int (file, buffer, buffer_index) != buffer_index) {
			puts (sf_strerror (file)) ;
			exit(1);
		}
		buffer_index = 0;
	}

	void dump() {
		printf("sfinfo.samplerate %d\n", sfinfo.samplerate);
		printf("    sfinfo.frames %d\n", sfinfo.frames);
		printf("  sfinfo.channels %d\n", sfinfo.channels);
		printf("    sfinfo.format %d\n", sfinfo.format);
	}

	void write(const int left, const int right = 0) {

		if (channels == 1) {	
			if(buffer_index >= buffer_size) flush_buffer();
			buffer [buffer_index] = left;
			buffer_index++;
		}
		else if (channels == 2) {	
			if(buffer_index >= buffer_size) flush_buffer();
			buffer [buffer_index] = left;
			buffer_index++;

			if(buffer_index >= buffer_size) flush_buffer();
			buffer [buffer_index] = right;
			buffer_index++;
		}
		else {
			dump();
			printf ("can only generate mono or stereo files.\n") ;
			exit (1) ;
		}
	}
};

#endif // SAMPLE_H_INCLUDED
