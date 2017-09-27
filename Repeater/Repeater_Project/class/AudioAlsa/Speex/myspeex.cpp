/*
* Myspeex.cpp
*
* Created: 2017/07/17
* Author: EDWARDS
*/

#include "myspeex.h"

Myspeex::Myspeex()
{
	speex_bits_init(&enc_bits);
	int rate = 4;//8khz
	enc_state = speex_encoder_init(&speex_nb_mode);
	speex_encoder_ctl(enc_state, SPEEX_SET_QUALITY, &rate);


	dec_state = speex_decoder_init(&speex_nb_mode);
	int tmp = 1;
	/*Set the perceptual enhancement on*/
	speex_decoder_ctl(dec_state, SPEEX_SET_ENH, &tmp);
	speex_bits_init(&dec_bits);


}

Myspeex::~Myspeex()
{
	speex_decoder_destroy(dec_state);
	speex_bits_destroy(&dec_bits);

	speex_encoder_destroy(enc_state);
	speex_bits_destroy(&enc_bits);
	fprintf(stderr, "delete class myspeex\n");
}


uint8_t Myspeex::encode_audio(short *buf, char *speex_data)
{

	char cbits[200];
	int nbBytes;

	/*Flush all the bits in the struct so we can encode a new frame*/
	speex_bits_reset(&enc_bits);
	/*Encode the frame*/
	//speex_encode(enc_state, input, &enc_bits);
	speex_encode_int(enc_state, buf, &enc_bits);
	/*Copy the bits to an array of char that can be written*/
	nbBytes = speex_bits_write(&enc_bits, cbits, 200);
	//fprintf(stderr, "speex_bits_write nbytes is : %d\n", nbBytes);
	memcpy(speex_data, cbits, nbBytes);

	return nbBytes;

}

void Myspeex::decode_audio(char *speex_data, short *out)
{

	int nbBytes = 20;

	speex_bits_read_from(&dec_bits, speex_data, nbBytes);
	//speex_decode(dec_state, &dec_bits, output);
	speex_decode_int(dec_state, &dec_bits, out);

}

