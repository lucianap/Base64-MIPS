#ifndef LECTURA_H
#define LECTURA_H

#include <stdio.h>
#include <stdlib.h>

// Funcion auxiliar para ingreso de texto por teclado
// Pre: -
// Post: Devuelve un puntero a una porcion de memoria con el texto indicado.
// En caso de fallar, devuelve NULL
// Recordar: La memoria es reservada dentro de la funcion, el usuario deberia
// de liberarla.
char* leer_linea();
char* leet_linea_archivo(FILE* file);

#endif //LECTURA_H
