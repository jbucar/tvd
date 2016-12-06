#include "../../src/sample.h"
#include <gtest/gtest.h>
#include <string.h>

TEST( Sample, greet ) {
	sample::Sample sample;
	unsigned char in_string[] = { 0xE1, 0xE9, 0xED, 0xF3, 0xFA, 0x00 }; // áéióú
	unsigned char out_string_correct[] = { 0xC3, 0xA1, 0xC3, 0xA9, 0xC3, 0xAD, 0xC3, 0xB3, 0xC3, 0xBA, 0x00 };
	char* out_string = sample.convert((char*) in_string);
	ASSERT_FALSE( out_string == 0 );
	ASSERT_TRUE( strcmp(out_string, (char*) out_string_correct) == 0 );
	if(out_string) {
		free(out_string);
	}
}
