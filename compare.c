
#include "compare.h"


/*Busca de padrão com metodo deterministico, calculada a distancia euclidiana entre o 
padrão buscado e os padrões de referencia*/
double compare(mfcc_frame *mfcc_frames1, unsigned int n1, mfcc_frame *mfcc_frames2, unsigned int n2)
{
	double distances[n1 + 1][n2 + 1];
	double prev_min;
	unsigned int i = 0, j = 0, k = 0;
	puts("comparando coeficientes mfcc");
	printf("padrao 1 %u\npadrao 2 %u\n", n1 ,n2);
	/*Calcule as distâncias de todos os pares de coeficientes MFCC*/
	for (i = 0; i < n1; i++)
	{
		
		for (j = 0; j < n2; j++)
		{
			
			distances[i + 1][j + 1] = 0;/*Distancias euclidianas*/
			for (k = 0; k < N_MFCC; k++)
			{
				distances[i + 1][j + 1] += pow(mfcc_frames1[i].features[k] - mfcc_frames2[j].features[k], 2);
			
			}
			distances[i + 1][j + 1] = sqrt(distances[i + 1][j + 1]);
		}
	}

	/* Preenchendo as fronteiras com o infinito*/
	for (i = 0; i <= n1; i++)
		distances[i][0] = atof("Inf");
	for (i = 0; i <= n2; i++)
		distances[0][i] = atof("Inf");
	distances[0][0] = 0;
	
	for (i = 1; i <= n1; i++)
		for (j = 1; j <= n2; j++)
		{
			prev_min = distances[i - 1][j];
			if (distances[i - 1][j - 1] < prev_min)
				prev_min = distances[i - 1][j - 1];
			if (distances[i][j - 1] < prev_min)
				prev_min = distances[i][j - 1];
			
			distances[i][j] += prev_min;
		}

	// Normalizar a distância
	return distances[n1][n2] / sqrt(pow(n1, 2) + pow(n2, 2));
}


