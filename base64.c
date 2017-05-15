#include "base64.h"
#include <stdint.h>
#include <stdio.h>

static char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/'};
static char padding = '=';

int base64_encode(int infd, int outfd) {

	unsigned char buffer_read[3];
	unsigned char buffer_write[4];

	int bytes_read = 0;
	do {
		bytes_read = read(infd, buffer_read, sizeof(buffer_read));

		//Si no leo ningún byte, salgo.
		if (bytes_read == 0) break;

		//Si leo 1 byte, completo en 0 al resto.
		if (bytes_read == 1) {
			buffer_read[1] = 0;
			buffer_read[2] = 0;
		}

		//Si leo 2 byte2, completo en 0 al faltante.
		if (bytes_read == 2) {
			buffer_read[2] = 0;
		}

		//Determino si leí un newline o un EOF, y reemplazo por 0.
		if (buffer_read[bytes_read-1] == '\n' || buffer_read[bytes_read-1] == EOF)  {
			buffer_read[--bytes_read] = 0;
		}

		//Transformo char a int32
		uint32_t first_char = buffer_read[0];
		uint32_t second_char = buffer_read[1];
		uint32_t third_char = buffer_read[2];

		//Shifteo para ubicar los chars en los bytes correspondientes a int32.
		first_char = first_char << 0x10;
		second_char = second_char << 0x08;

		//Armo el int32 compuesto por los 3 bytes.
		uint32_t sum = 0;
		sum = sum + first_char;
		sum = sum + second_char;
		sum = sum + third_char;

		//Calculo los 6 bits de cada caracter del encoding
		int first_coded = sum >> 18;

		int second_coded = sum >> 12;
		second_coded = second_coded & 0x3F;

		int third_coded = sum >> 6;
		third_coded = third_coded & 0x3F;

		int fourth_coded = sum & 0x3F;

		//Completo el buffer de salida con los chars de Base64
		buffer_write[0] = encoding_table[first_coded];
		buffer_write[1] = encoding_table[second_coded];
		buffer_write[2] = encoding_table[third_coded];
		buffer_write[3] = encoding_table[fourth_coded];

		//Reemplazo por padding correspondiente
		if (bytes_read == 1) {
			buffer_write[2] = padding;
			buffer_write[3] = padding;
		}

		if (bytes_read == 2) {
			buffer_write[3] = padding;
		}

		write(outfd, buffer_write, sizeof(buffer_write));

	} while (bytes_read == sizeof(buffer_read));

	return 0;
}

int base64_decode(int infd, int outfd) {
	return 0;
}
