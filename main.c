
#include "link.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdint.h>
#include "neuron.h"



Inputs *copy(mfcc_frame *mfcc_frames, int n_frames, char *name){
    register int i, j;
    Inputs *n = malloc(sizeof(Inputs));
    n->num_frames = n_frames;

    n->name = malloc (strlen(name) * sizeof(char));
    strcpy(n->name, name);
    n->input = malloc (n_frames * sizeof (double));

    for(i = 0; i < n_frames; i++)
        for(j = 0; j < N_MFCC; j++){
            n->input[i].features[j] = mfcc_frames[i].features[j];
            //printf("input[%d].features[%d]   %s   %.5f\n", i, j, n->name, n->input[i].features[j]);
        }
  
    printf("%s  frames: %d\n", n->name, n->num_frames);
    return n;


}

int main(void)
{

    int netSizeX = 100, netSizeY = 100;
    int scale = 4;
    void **word_adresses;
    unsigned int n = 0;
    int  count = 0;
    int epochs = 3000;
    register int i;
    Inputs *teste = malloc(sizeof(Inputs));
    word *words = malloc(sizeof(word));
	char opt;
    //setar os parametros de entrada
    //if( ( n = get_list(words) ) )
    //{
      //  printf("n = %d\n", n);

    //}
    //map *m = init_map(netSizeX, netSizeY, scale);
    //puts("inicializou o map");
    //printf("result %d\n\n", get_list(words));


	do
	{
		do{
			printf("a - inserir palavra\nb - listar palavras\nc - reconhecer\nd- treinar rede\ne - sair: ");
			scanf(" %c", &opt);
		}while(opt < 'a' || opt > 'e');

		switch (opt)
		{
			case 'a':
				usleep(200000);
				get_word(0);
				break;
			case 'b':
				system("ls palavras");
				break;
            case 'c':
                usleep(200000);
                get_word(1);
                break;
			case 'd':
                //word_adresses = malloc(n * sizeof(void *));
                //while (words != NULL){
                   // teste = copy(words->frames, words->n, words->name);
                    //printf("teste %s\n", teste->name);
                    //train(m, teste, epochs);
                   // word_adresses[count++] = words;
                  //  words = words->next;
               // }
               // printf("count %d\n", count);
                //for (i = 0; i < count; i++)
                  //  free(word_adresses[i]);
                //free(word_adresses);
                break;
		}
	}
	while (opt != 'e');
    
    //destroy_map(m);

	return 0;
}


