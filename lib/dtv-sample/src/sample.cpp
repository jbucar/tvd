#include "sample.h"

#include <errno.h>
#include <stdio.h>

#include <stdlib.h>
#include <string.h>

/* The names of the input and output encodings. */
const char * INSET = "ISO_8859-1";
const char * OUTSET = "UTF-8";

namespace sample {

Sample::Sample( void ) {
	_conv_desc = iconv_open(OUTSET, INSET);
}

Sample::~Sample( void ) {
	if (_conv_desc != (iconv_t)-1) {
		iconv_close(_conv_desc);
	}
}

char* Sample::convert(char* in_string) {
	if(_conv_desc == (iconv_t)-1) {
		return NULL;
	}

	size_t in_size = strlen (in_string);
	size_t out_size = 2*in_size;
	char* out_string = (char*) calloc (out_size, 1);
	char* result = out_string;

	iconv (_conv_desc, &in_string, &in_size, &out_string, &out_size);

	return result;
}

}
