#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mytar.h"


/** Copy nBytes bytes from the origin file to the destination file.
 *
 * origin: pointer to the FILE descriptor associated with the origin file
 * destination:  pointer to the FILE descriptor associated with the destination file
 * nBytes: number of bytes to copy
 *
 * Returns the number of bytes actually copied or -1 if an error occured.
 */
static int copynFile(FILE * origin, FILE * destination, unsigned int nBytes)
{
	int cont = 0; //Al tener que retornar el numero de bytes que ha copiado, iniciamos un contador a 0
	int read, write; //creamos los buffer
	while(cont < nBytes && (read = getc(origin)) != EOF) { //leemos del archivo origen
		write = putc(read, destination); //copiamos al archivo destino
		cont++; //aumentamos el contador cada vez que leemos un byte
		if (write == EOF) {
			fclose(origin); //en caso de que el descriptor de ficheros origen de EOF lo cerramos
		}
	}
	return cont; //devolvemos el numero de bytes
}

/** Loads a string from a file.
 *
 * file: pointer to the FILE descriptor 
 * 
 * The loadstr() function must allocate memory from the heap to store 
 * the contents of the string read from the FILE. 
 * Once the string has been properly built in memory, the function returns
 * the starting address of the string (pointer returned by malloc()) 
 * 
 * Returns: !=NULL if success, NULL if error
 */
static char* loadstr(FILE * file)
{
	char caracter = getc(file); //creamos una variable de tipo char y leemos el caracter que apunta el argumento (asumiendo que apunta al lugar correcto)
	int longitud_string = 0; //creamos una variable para calcular posteriormente la longitud del string que vamos a leer y lo inicializamos a 0
	char *string; //creamos el puntero que apuntara al array donde se va a almacenar cada char que pertenece al string en cuestion
	while (caracter != '\0') { //hasta que encuentra \0 lo que indica el final del string va sumando y obteniendo la longitud de dicho string
		longitud_string++;
		caracter = getc(file);
	}
	string = malloc(sizeof(char) * (longitud_string + 1)); //reserva espacio en memoria para almacenar en el array creado, todos los char, almacena el tamaño de un char por la longitud creada + 1 para almacenar el \0
	fseek(file, -(longitud_string + 1), SEEK_CUR); //situa el puntero del descriptor de fichero en la posicion correcta para empezar a escribir en el array, en este caso retrasa el puntero
	for (int i = 0; i < longitud_string + 1; i++) {
		string[i] = getc(file); //escribe caracter a caracter el string completo
	}
	return string; //retorna el array de char (su puntero)
}

/** Read tarball header and store it in memory.
 *
 * tarFile: pointer to the tarball's FILE descriptor 
 * nFiles: output parameter. Used to return the number 
 * of files stored in the tarball archive (first 4 bytes of the header).
 *
 * On success it returns the starting memory address of an array that stores 
 * the (name,size) pairs read from the tar file. Upon failure, the function returns NULL.
 */
static stHeaderEntry* readHeader(FILE * tarFile, unsigned int *nFiles)
{
	int nr_files = 0; //inicializamos el numero de archivos a 0 para posteriormente leer la primera linea y darle el valor
	stHeaderEntry *array = NULL; //inicializamos el array donde se va a guardar toda la informacion de los ficheros (nombre, tamanio)
	fread(&nr_files, sizeof(int), 1, tarFile); //leemos el numero de ficheros
	array = malloc(sizeof(stHeaderEntry) * nr_files); //guardamos memoria para toda la informacion de los ficheros (nombre, tamanio)
	for (int i = 0; i < nr_files; i++) { //leemos y almacenamos en el array el nombre y el tamanio de los nr_files ficheros
		if ((array[i].name = loadstr(tarFile)) == NULL) return NULL;
		fread(&array[i].size, sizeof(unsigned int), 1, tarFile);
	}
	*nFiles = nr_files; //guardamos el numero de ficheros en el parametro de entrada
	return array; //retornamos toda la informacion leida (cabecera completa)
}

/** Creates a tarball archive 
 *
 * nfiles: number of files to be stored in the tarball
 * filenames: array with the path names of the files to be included in the tarball
 * tarname: name of the tarball archive
 * 
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First reserve room in the file to store the tarball header.
 * Move the file's position indicator to the data section (skip the header)
 * and dump the contents of the source files (one by one) in the tarball archive. 
 * At the same time, build the representation of the tarball header in memory.
 * Finally, rewind the file's position indicator, write the number of files as well as 
 * the (file name,file size) pairs in the tar archive.
 *
 * Important reminder: to calculate the room needed for the header, a simple sizeof 
 * of stHeaderEntry will not work. Bear in mind that, on disk, file names found in (name,size) 
 * pairs occupy strlen(name)+1 bytes.
 *
 */
int createTar(int nFiles, char *fileNames[], char tarName[])
{
	stHeaderEntry *tarball; //creamos el tarball donde guardaremos toda la informacion
	int bytes_cabezera = sizeof(int) + nFiles*sizeof(unsigned int), num_bytes_leidos = 0; //inicializamos los bytes que tendra la cabecera sin contar el tamaño del nombre de los ficheros (solo contamos size de cada fichero y un sizeof(int) para el numero de ficheros)
	tarball = malloc(sizeof(stHeaderEntry) * nFiles); //reservamos memoria para escribir toda la informacion
	FILE *archivodestino, *archivo_origen; //creamos los buffer
	for (int i = 0; i < nFiles; i++) { //aqui aniadimos a los bytes calculados anteriormente, todos los que quedaban del tamaño de los nombres de los ficheros
		bytes_cabezera += strlen(fileNames[i]) + 1;
	}
	archivodestino = fopen(tarName, "w"); //abrimos y ponemos el puntero del fichero tarName al principio y lo almacenamos en el buffer
	fseek(archivodestino, bytes_cabezera, SEEK_SET); //movemos el puntero para dejar espacio arriba para la cabecera y nos ponemos a escribir todo el contenido del fichero
	for (int i = 0; i < nFiles; i++) {
		if ((archivo_origen = fopen(fileNames[i], "r")) != NULL) { // se abre el fichero fileNames[i] para la lectura del mismo y se guarda en el buffer
			num_bytes_leidos = copynFile(archivo_origen, archivodestino, INT_MAX); //se copia con la funcion copynFile toda la informacion del fichero fileNames[i]
			if (num_bytes_leidos == -1) {
				return EXIT_FAILURE;
			}
			else {
				tarball[i].size = num_bytes_leidos; //se guarda el tamaño leido (tamaño del fichero devuelto por copynFiles)
				tarball[i].name = malloc(sizeof(fileNames[i]) + 1); //se reserva memoria para el nombre del fichero, +1 para \0
				strcpy(tarball[i].name, fileNames[i]); //se copia el nombre con strcpy
			}
		}
		else return EXIT_FAILURE;
	}
	if(fclose(archivo_origen) == EOF) return EXIT_FAILURE;
	if (fseek(archivodestino, 0, SEEK_SET) != 0) return EXIT_FAILURE; //se pone el puntero al inicio del fichero mtar para escribir la cabecera
	fwrite(&nFiles, sizeof(unsigned int), 1, archivodestino); //primero se escribe el numero de archivos
	for (int i = 0; i < nFiles; i++) {
		fwrite(tarball[i].name, strlen(tarball[i].name) + 1, 1, archivodestino); //se escribe el nombre del fichero al archivo destino
		fwrite(&tarball[i].size, sizeof(unsigned int), 1, archivodestino); //y se escribe el tamaño del fichero
	}
	for (int i = 0; i < nFiles; i++) {
		free(tarball[i].name); //se libera la memoria de todos los nombres
	}
	free(tarball); //y se libera la memoria del tarball
	if (fclose(archivodestino) == EOF) return EXIT_FAILURE; //se cierra el buffer
	printf("Mtar file created successfuly\n");
	return EXIT_SUCCESS;
}

/** Extract files stored in a tarball archive
 *
 * tarName: tarball's pathname
 *
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First load the tarball's header into memory.
 * After reading the header, the file position indicator will be located at the 
 * tarball's data section. By using information from the 
 * header --number of files and (file name, file size) pairs--, extract files 
 * stored in the data section of the tarball.
 *
*/
int extractTar(char tarName[])
{
	FILE *tarball_aux = fopen(tarName, "r"), *archivo_destino; // se crea un tarball_aux para almacenar los datos temporalmente
	stHeaderEntry *cabecera; //se crea una variable para coger los datos de la cabecera
	unsigned int nFiles = 0; //inicializamos la variable a 0 para almacenar el numero de archivos que hay en el mtar
	int bits_copiados;
	if ((cabecera = readHeader(tarball_aux, &nFiles)) != NULL) { //se lee la cabecera con la funcion readHeader que tambien devuelve en el parametro nFiles el numero de archivos que hay en el archivo mtar
		for (int i = 0; i < nFiles; i++) {
			if ((archivo_destino = fopen(cabecera[i].name, "w")) != NULL) { //crea los ficheros de nombre cabecera[i].name
				if ((bits_copiados = copynFile(tarball_aux, archivo_destino, cabecera[i].size)) == -1) { //copia todo el contenido del fichero en el fichero nuevo
					return EXIT_FAILURE;
				}
				printf("[%d]: Creating file %s, size %d Bytes...Ok\n", i, cabecera[i].name, cabecera[i].size);
			}
			else return EXIT_FAILURE;
		}
	}
	else return EXIT_FAILURE;
	fclose(tarball_aux); //cierra el tarball_aux
	fclose(archivo_destino); //cierra el archivo_destino
	free(cabecera); //libera memoria de la cabecera
	return EXIT_SUCCESS;
}

