#ifndef OLD_ENCODER_H
#define OLD_ENCODER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

unsigned char get6bitsFrom(char* src, int from, int length);
char* encode(char* input, int input_len);
char decodeCharFromTable(char input);
char* decode3chars(char* chars);
char* decode(char* input, int input_lenght);

#endif //MAIN_H
