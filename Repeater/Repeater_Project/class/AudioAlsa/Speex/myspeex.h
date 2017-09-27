/*
* Speex.h
*
* Created: 2017/07/17
* Author: EDWARDS
*/
#ifndef Myspeex_h_
#define Myspeex_h_ 

#include <stdio.h>
#include <stdlib.h>
#include <speex/speex.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

#define	FRAME_SIZE	160

class Myspeex
{
public:
	Myspeex();
	~Myspeex();

	uint8_t encode_audio(short *buf, char*speex_data);
	void decode_audio(char *speex_data, short *out);

private:

	void *enc_state;
	SpeexBits enc_bits;

	void *dec_state;
	SpeexBits dec_bits;



};



#endif 