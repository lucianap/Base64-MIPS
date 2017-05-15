#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include "lectura.h"
#include "base64.h"

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
   			"\t\t-i, --input Location of the input file. If this parameter is not present, input is std input\n"
   			"\t\t-o, --output Location of the output file. If this parameter is not present, output is std output\n"
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

int main( int argc, const char* argv[] )
{
	bool doEncode = true;
	bool stdInput = true;
	const char* inputFile = NULL;
	bool stdOutput = true;
	const char* outputFile = NULL;

	if(argc > 1) {
		for(int i = 1; i < argc; i++) {
			if(argv[i][1] == 'h' || strcmp(argv[i],"--help") == 0 ) {
				help_message();
			} else if (argv[i][1] == 'V' || strcmp(argv[i],"--version") == 0) {
				version();
			} else if (strcmp(argv[i],"-a") == 0 || strcmp(argv[i],"--action") == 0) {
				if(strcmp(argv[i + 1], "decode") == 0)
					doEncode = false;
			} else if (strcmp(argv[i],"-i") == 0 || strcmp(argv[i],"--input") == 0) {
				inputFile = argv[i + 1];
				stdInput = false;
			} else if (strcmp(argv[i],"-o") == 0 || strcmp(argv[i],"--output") == 0) {
				outputFile = argv[i + 1];
				stdOutput = false;
			}
		}
	    
    }
	int fd_in = STDIN_FILENO;
	int fd_out = STDOUT_FILENO;

	FILE* fInput;
	FILE* fOutput;

    //Leer de archivo o de standard input
	if(!stdInput) {
		fInput = fopen(inputFile, "r");
		if(fInput == NULL) {
			fprintf(stderr, "Ocurrio un error al abrir el archivo.\n");
			return -1;
		}

		fd_in = fileno(fInput);

//		input = leer_linea_archivo(fInput);
//		fclose(fInput);
	} else {
		printf("Ingrese la cadena: \n");
	}
	
    //Leer de archivo o de standard input
	if(!stdOutput) {
		fOutput = fopen(outputFile, "w");

//		fputs(output, fOutput);

		fd_out = fileno(fOutput);

//		fclose(fOutput);
	}

	char* output;
	if(doEncode) {
		output = base64_encode(fd_in, fd_out);
	} else {
		output = base64_decode(fd_in, fd_out);
	}

	if(!stdInput) {
		fclose(fInput);
	}

	if(!stdOutput) {
		fclose(fOutput);
	} else {
		printf("\n");
	}

//	if(output != NULL) {
//		if(!stdOutput) {
//			FILE* fOutput = fopen(outputFile, "w");
//			fputs(output, fOutput);
//			fclose(fOutput);
//		} else {
//			printf("%s\n", output);
//		}
//	}
	
	// La lectura por stdInput esta hecha con memoria dinamica. La tengo que liberar.
//	free(input);
 
	return 0;

}
