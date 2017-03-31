#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


int isLittleEndian = 1;

static char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/'};

void checkEndian(void)
{
    union
    {
        short   inum;
        char c[sizeof(short)];
    } un;

    un.inum=0x0102;

    if(un.c[0]==1 && un.c[1]==2)
    {
        printf("big_endian.\n");
        isLittleEndian = 0;
    }
    else if(un.c[0]==2 && un.c[1]==1)
    {
        printf("little_endian.\n");
        isLittleEndian = 1;
    }

}

// Devuelve 6 bits contiguos a la posición from.
// En caso de que los bits pedidos no alcancen, se completan los 6 bits con ceros.
unsigned char get6bitsFrom(char* src, int from, int length)
{
    //Me posiciono en el byte de inicio
    unsigned char tmp1 = *(src + from/8);
    unsigned char tmp2 = *(src + from/8 + 1);

    int bitsDer = 8 - (from % 8);
    int startDer = from % 8;
    int bitsIzq = 0;

    unsigned char maskDer = 0x0;
    unsigned char maskIzq = 0x0;
    unsigned char result = 0x0;

    if((bitsDer < 6) && ((from + bitsDer) < length*8)) {
        bitsIzq = (6-bitsDer);
        maskDer = ((1 << bitsDer) - 1) << (0);
        maskIzq = ~((1 << (8 -bitsIzq)) - 1);

        unsigned char first = tmp1 & maskDer;
        unsigned char second = tmp2 & maskIzq;
        result = (first << (6 - bitsDer)) | (second >> (8 - bitsIzq));
    } else if ((from + 6) <= (length*8)) {
        bitsDer = 6;
        startDer = (from%8);
        maskDer = ((1 << 6) - 1) << (8 - startDer - 6);
        result = (maskDer & tmp1) >> (8 - 6 - startDer);
    } else if ((from + 6) > length*8) {
        //complete with 0
        unsigned char mask = 0x3;
        result = (mask & tmp1) << 4;
    }

    return result;

}


int getBit(char *src, int n)
{
    unsigned char tmp = *(src + n/8);
    unsigned char mask = (0x1 << (8 - n%8 - 1));
    int bit = 0;
    bit =  (tmp & mask) > 0;
    return bit;
}


void printbits(unsigned char c)
{
    int i = 0;
    for(i = 0; i< 8; i++) {
        printf("%d", getBit(&c, i));
    }
    printf("\n");
}


void setBit(char *src, int n, int bit)
{
    unsigned char * pTmp = src + n/8;
    unsigned char mask = (0x1 << (8 - n%8 - 1));

    if (bit)
    {
        *pTmp |= mask;
    }
    else
    {
        *pTmp &= ~mask;
    }
}


char* encode(char* input, int input_len)
{
    //length of the coded string
    char* output = malloc( 4 * ((input_len + 2) / 3) );

    int counterOutput;
    counterOutput = 0;
    int code = 0;
    int i;

    for(i = 0; i <= (input_len*8 - 2); i+=6) {
        code = get6bitsFrom(input, i, input_len);
        output[counterOutput] = encoding_table[code];
        counterOutput++;
    }

    //now calculate padding if necessary
    if((input_len % 3) != 0) {
        int restBytes = (input_len) % 3;
        if(restBytes == 2) {
            output[counterOutput] = '=';
        } else if (restBytes == 1) {
            output[counterOutput] = '=';
            counterOutput++;
            output[counterOutput] = '=';
        }
    }

    counterOutput++;
    output[counterOutput] = '\n';

    return output;
}

char decodeCharFromTable(char input) {
    int i;
    int len = sizeof(encoding_table) / sizeof(char);
    for (i = 0; i < len; i++) {
        if(encoding_table[i] == input) {
            return i;
        }
    }
    //si no encuentra en la tabla tira exception
}

//Entran 4 chars encodeados y salen 3 desencodeados.
char* decode3chars(char* chars) {
    char* output = malloc(sizeof(char) * 4);
    char char1 = decodeCharFromTable(chars[0]);
    char char2 = decodeCharFromTable(chars[1]);
    char char3 = decodeCharFromTable(chars[2]);
    if (char3 == '=') {
        char3 = 0x0;
    }
    char char4 = decodeCharFromTable(chars[3]);
    if(char4 == '=') {
        char4 = 0x0;
    }

    unsigned char mask1 = (0x3 << 4);
    unsigned char temp1 = char1;
    unsigned char temp2 = char2 & mask1;

    temp1 = temp1 << 2;
    temp2 = temp2 >> 4;
    output[0] = temp1 | temp2;

    unsigned char mask3 = 0xF;
    unsigned char mask4 = 0xF << 2;
    temp1 = (char2 & mask3) << 4;
    temp2 = (char3 & mask4) >> 2;
    output[1] = temp1 |temp2;

    unsigned char mask5 = 0x3;
    temp1 = (char3 & mask5) << 6;
    temp2 = char4;
    output[2] = temp1 | temp2;
    output[3] = '\0';

    return output;

}

char* decode(char* input, int input_lenght) {

    int i;
    if (input_lenght % 4 != 0) return NULL;
    if(input_lenght % 4 != 0){
        //TIRAR EXCEPTION, NO RESPETA STANDARD DE BASE64
    }

    int output_length = input_lenght / 4 * 3;
    if (input[input_lenght - 1] == '=') (output_length)--;
    if (input[input_lenght - 2] == '=') (output_length)--;

    unsigned char *decoded_data = malloc(output_length);
    if (decoded_data == NULL) return NULL;
    int outputCounter = 0;

    for(i = 0; i < input_lenght; i+=4){
        unsigned char* in = input + i;
        char* decodedChars = decode3chars(in);
        decoded_data[outputCounter] = decodedChars[0];
        outputCounter++;
        decoded_data[outputCounter] = decodedChars[1];
        outputCounter++;
        decoded_data[outputCounter] = decodedChars[2];
        outputCounter++;
    }

    return decoded_data;
}


void help_message() {

	char buffer[512];
	snprintf(buffer, sizeof buffer, "%s",
   			"\tUsage: \n"
   			"\ttp0 -h \n"
   			"\ttp0 -V \n"
   			"\ttp0 [options] \n"
   			"\tOptions: \n"
   			"\t\t-V, --version Print version and quit.\n"
   			"\t\t-h, --help Print this information.\n"
   			"\t\t-i, --input Location of the input file.\n"
   			"\t\t-o, --output Location of the output file.\n"
   			"\t\t-a, --action Program action: encode (default) or decode.\n"
			"\tExamples:\n"
			"\t\ttp0 -a encode -i ~/input -o ~/output\n"
			"\t\ttp0 -a decode\n"
		);
	printf("%s\r\n", buffer);
}

void version() {
	printf("%s", "tp0 version 1.0\n");
}



void main( int argc, const char* argv[] )
{
    //checkEndian();

    char* action = "encode";
    char* inputFormat = "stdInput";
    char* outputFormat = "stdOutput";

    if(argc > 1) {
	    if(argv[1][1] == 'h' || strcmp(argv[1],"--help") == 0 ) {
		help_message();
	    } else if (argv[1][1] == 'V' || strcmp(argv[1],"--version") == 0) {
            version();
        }
    }

    //Leer de archivo o de standar input
    char *record = "Man is distinguished, not only by his reason, but";
    int recordLen = strlen(record);

    printf("recordlen: %d", recordLen);

    char* encoded = encode(record, recordLen);
    printf("\ntrying encoding: %s\n\n\n\n", encoded);

    char* cadena = "TWFuIGlzIGRpc3Rpbmd1aXNoZWQsIG5vdCBvbmx5IGJ5IGhpcyByZWFzb24sIGJ1dCBieSB0aGlzIHNpbmd1bGFyIHBhc3Npb24gZnJvbSBvdGhlciBhbmltYWxzLCB3aGljaCBpcyBhIGx1c3Qgb2YgdGhlIG1pbmQsIHRoYXQgYnkgYSBwZXJzZXZlcmFuY2Ugb2YgZGVsaWdodCBpbiB0aGUgY29udGludWVkIGFuZCBpbmRlZmF0aWdhYmxlIGdlbmVyYXRpb24gb2Yga25vd2xlZGdlLCBleGNlZWRzIHRoZSBzaG9ydCB2ZWhlbWVuY2Ugb2YgYW55IGNhcm5hbCBwbGVhc3VyZS4=";
    //char* chars = decode3chars(cadena);
    printf("fdsdfsdfsdfsdfsdfdsfsdfsdf");

    printf("trying decoding: %s\n\n\n", decode(cadena, strlen(cadena)));


    return 0;

}
