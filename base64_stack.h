#ifndef BASE64_STACK_H_INCLUDED
#define BASE64_STACK_H_INCLUDED

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdint.h>

unsigned char *base64_decode_2(const char *data,
                             size_t input_length,
                             size_t *output_length);

char *base64_encode_2(const unsigned char *data,
                    size_t input_length,
                    size_t *output_length);

void base64_cleanup();
void build_decoding_table();

#endif // BASE64_H_INCLUDED
