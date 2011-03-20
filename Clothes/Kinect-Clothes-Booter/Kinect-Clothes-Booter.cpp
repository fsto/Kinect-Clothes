#include <stdio.h>
#include <stdlib.h>

int main ()
{
	printf ("Checking if processor is available...");
	if(system(NULL)) puts ("Ok");
	else{
		printf("--------------------------------------\n");
		printf("Kinect Clothes Booter failed to run.\n");
		printf("Please run Kinect-Clothes.exe\n");
		printf("--------------------------------------\n");
		getchar();
		exit(1);
	}
	printf("Running Kinect Clothes\n");
	while(system("Kinect-Clothes.exe") != 0);
	return 0;
}
