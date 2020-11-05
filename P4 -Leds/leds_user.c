/*
 * leds_user.c
 *
 *  Created on: 15 may. 2019
 *      Author: Veronica Calzada y David Prats
 *      Calculadora que puede sumar numeros dando como resultado desde el 0 hasta el 7
 *      Los sumandos se escriben por consola,
 *      mientras que el resultado se da por los leds del teclado
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void parse_sumando(int sumando, int fichero) {
	if (sumando == 1) {
		write(fichero, "4", 3);
	}
	else if (sumando == 2) {
		write(fichero, "2", 3);
	}
	else if (sumando == 3) {
		write(fichero, "23", 3);
	}
	else if (sumando == 4) {
		write(fichero, "1", 3);
	}
	else if (sumando == 5) {
		write(fichero, "13", 3);
	}
	else if (sumando == 6) {
		write(fichero, "12", 3);
	}
	else if (sumando == 7) {
		write(fichero, "123", 3);
	}
	usleep(500000);
}

int main() {
	int fichero = open("/dev/leds", O_WRONLY);
	write(fichero, "0", 3);
	int sumando1, sumando2, total = 0;
	printf("----Bienvenido a la calculadora que te da los resultados en binario.----\n\n");
	printf("~~Esta calculadora solo puede dar resultados del 0 al 7~~\n");
	printf("Escribe el sumando 1: ");
	scanf("%d", &sumando1);
	printf("Escribe el sumando 2: ");
	scanf("%d", &sumando2);
	printf("A continuacion se mostrara durante un segundo en los leds del teclado primero\n"
			"el sumando uno, y despues el sumando 2.\nPor ultimo se mostrara la solucion");
	if (sumando1 < 8 && sumando2 < 8) {
		parse_sumando(sumando1, fichero);
		parse_sumando(sumando2, fichero);
		total = sumando1 + sumando2;
		if (total < 8 && total >= 0) {
			parse_sumando(total, fichero);
			printf("\nResultado: %d", total);
			printf("\n");
		}
	}
	close(fichero);
	return 0;
}



