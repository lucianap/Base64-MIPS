#include <stdio.h>
#include <string.h>

#include "base64_stack.h"

void encodingTest(char* src, char* out);
void decodingTest(char* src, char* out);

//int main() {
//	char src[400];
//	char expected[400];
//	strcpy(src, "");
//	strcpy(expected, "");
//	encodingTest(src, expected);
//	decodingTest(expected, src);
//	strcpy(src, "f");
//	strcpy(expected, "Zg==");
//	encodingTest(src, expected);
//	decodingTest(expected, src);
//	strcpy(src, "fo");
//	strcpy(expected, "Zm8=");
//	encodingTest(src, expected);
//	decodingTest(expected, src);
//	strcpy(src, "foo");
//	strcpy(expected, "Zm9v");
//	encodingTest(src, expected);
//	decodingTest(expected, src);
//	strcpy(src, "foob");
//	strcpy(expected, "Zm9vYg==");
//	encodingTest(src, expected);
//	decodingTest(expected, src);
//	strcpy(src, "fooba");
//	strcpy(expected, "Zm9vYmE=");
//	encodingTest(src, expected);
//	decodingTest(expected, src);
//	strcpy(src, "foobar");
//	strcpy(expected, "Zm9vYmFy");
//	encodingTest(src, expected);
//	decodingTest(expected, src);
//	strcpy(src, "M");
//	strcpy(expected, "TQ==");
//	encodingTest(src, expected);
//	decodingTest(expected, src);
//	strcpy(src, "Ma");
//	strcpy(expected, "TWE=");
//	encodingTest(src, expected);
//	decodingTest(expected, src);
//	strcpy(src, "any carnal pleasure.");
//	strcpy(expected, "YW55IGNhcm5hbCBwbGVhc3VyZS4=");
//	encodingTest(src, expected);
//	decodingTest(expected, src);
//	strcpy(src, "any carnal pleasure");
//	strcpy(expected, "YW55IGNhcm5hbCBwbGVhc3VyZQ==");
//	encodingTest(src, expected);
//	decodingTest(expected, src);
//	strcpy(src, "pleasure.");
//	strcpy(expected, "cGxlYXN1cmUu");
//	encodingTest(src, expected);
//	decodingTest(expected, src);
//	strcpy(src, "leasure.");
//	strcpy(expected, "bGVhc3VyZS4=");
//	encodingTest(src, expected);
//	decodingTest(expected, src);
//	strcpy(src, "easure.");
//	strcpy(expected, "ZWFzdXJlLg==");
//	encodingTest(src, expected);
//	decodingTest(expected, src);
//	strcpy(src, "asure.");
//	strcpy(expected, "YXN1cmUu");
//	encodingTest(src, expected);
//	decodingTest(expected, src);
//	strcpy(src, "sure.");
//	strcpy(expected, "c3VyZS4=");
//	encodingTest(src, expected);
//	decodingTest(expected, src);
//
//	return 0;
//}

void encodingTest(char* src, char* out) {
    size_t outputsize;
	char* encoded = base64_encode_2(src, strlen(src), &outputsize);
	int result = strcmp(out, encoded);
	char* boolRes = "";
	if(result == 0)
		boolRes = "OK";
	else
		boolRes = encoded;
	printf("TEST: encoding %s, %s: %s\n", src, out, boolRes);
}

void decodingTest(char* src, char* out) {
    size_t outputsize;
	char* decoded = base64_decode_2(src, strlen(src), &outputsize);
	int result = strcmp(out, decoded);
	char* boolRes = "";
	if(result == 0)
		boolRes = "OK";
	else
		boolRes = decoded;
	printf("TEST: decoding %s, %s: %s\n", src, out, boolRes);
}
