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
    int startIzq = 0;
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

    int counterInput,counterOutput;
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

    return output;
}



void helpMessage() {

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

    if(argc > 1) {
	    if(argv[1][1] == 'h' || strcmp(argv[1],"--help") == 0 ) {
		helpMessage();
	    } else if (argv[1][1] == 'V' || strcmp(argv[1],"--version") == 0) {
	    	version();
            }	
    } 

    //char* palabra = "abbcc";
    //get6bitsFrom(palabra, 12, 5);



    //Leer de archivo o de standar input
    char *record = "Man is distinguished, not only by his reason, but";
    int recordLen = strlen(record);

    printf("recordlen: %d", recordLen);

    printf("\ntrying encoding: %s\n", encode(record, recordLen));


}
