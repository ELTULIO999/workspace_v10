/*
 ============================================================================
 Name        : lab3.c
 Author      : Julio Lopez
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

int main(void) {
	//puts("!!!Hello World!!!"); /* prints !!!Hello World!!! */

	  for (int cont = 0; cont <= 5; cont++) {
	    printf ("%d",cont);
	    puts("");
	    sleep(1);
	  }
	  puts("saludos program end !!!!!!");
	  return 0;
	}
