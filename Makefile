CC= gcc
CFLAGS= -Wall -pedantic -std=c99 -g

AUXILIARES= lectura.o main.o
EXEC= run

#Configuracion Entrega
ARCHIVOS=*.c *.h Makefile 	#Tipos de archivos que se van a agregar
CUATRIMESTRE=2017-1C
ENTREGA=TP0
ENCODING=ISO-8859-1
OUTPUTFILE=out.ps 		 	#archivo intermedio
FORMATO=portrait 			#portrait o landscape (vertical u horizontal)
COLUMNAS=1 	  			#paginas por hoja (en columnas)
NUMEROS_LINEA=1   			#cada cuantas lineas se imprime el numero de linea

ARCHIVO_ENTREGA=-$(ENTREGA)
NOMBRE_ZIP= $(ARCHIVO_ENTREGA).zip
NOMBRE_PDF= $(ARCHIVO_ENTREGA).pdf
ENCABEZADO = "[66.20] Organizacion de Computadoras"
PIE = "$(CUATRIMESTRE) -- $(ENTREGA)"
#Fin configuracion



all: $(EXEC)

lectura.o: lectura.c lectura.h
	$(CC) $(CFLAGS) lectura.c -c

main.o: main.c main.h
	$(CC) $(CFLAGS) main.c -c

$(EXEC): $(AUXILIARES)
	$(CC) $(CFLAGS) $(AUXILIARES) run.c -o $(EXEC)

run: $(EXEC)
	./$(EXEC)

clean:
	rm -f *.o $(EXEC)


#Seccion Entrega
pdf: clean
	a2ps $(ARCHIVOS) -Av --header=$(ENCABEZADO) --footer=$(PIE) --line-numbers=$(NUMEROS_LINEA) --borders=yes --columns=$(COLUMNAS)  --$(FORMATO) --output=$(OUTPUTFILE) --encoding=$(ENCODING) --tabsize=4 --major=columns --toc  | ps2pdf $(OUTPUTFILE) $(NOMBRE_PDF)
	rm -f *.ps #elimino los archivos temporales

entrega: pdf
	zip $(NOMBRE_ZIP) $(ARCHIVOS) *.pdf

clean_entrega: clean
	rm *.zip *.pdf
