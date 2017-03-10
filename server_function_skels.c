#include "server_functions.h"
#include <stdio.h>
#include <string.h>

int f0_Skel(int *argTypes, void **args) {
  printf("f0_skel");
  
  *(int *)args[0] = f0(*(int *)args[1], *(int *)args[2]);
  return 0;
}

int f1_Skel(int *argTypes, void **args) {
  printf("f1_skel");

  *((long *)*args) = f1( *((char *)(*(args + 1))), 
		        *((short *)(*(args + 2))),
		        *((int *)(*(args + 3))),
		        *((long *)(*(args + 4))) );
  return 0;
}

int f2_Skel(int *argTypes, void **args) {
  printf("f2_skel");

  /* (char *)*args = f2( *((float *)(*(args + 1))), *((double *)(*(args + 2))) ); */
  *args = f2( *((float *)(*(args + 1))), *((double *)(*(args + 2))) );

  return 0;
}

int f3_Skel(int *argTypes, void **args) {
  printf("f3_skel");

  f3((long *)(*args));
  return 0;
}

/* 
 * this skeleton doesn't do anything except returns
 * a negative value to mimic an error during the 
 * server function execution, i.e. file not exist
 */
int f4_Skel(int *argTypes, void **args) {
  printf("f4_skel");

  return -1; /* can not print the file */
}

