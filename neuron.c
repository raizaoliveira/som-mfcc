#include "neuron.h"


void init_neuron(neuron *n, mfcc_frame *mfcc_frames, unsigned int n_frames, char *name)
{
	mt_seed ();
	double r;
	int x, y;
	register int i, j;
	n->frames = mfcc_frames;
	n->num_weights = n_frames;
	n->x = mt_rand()%99; 
	n->y = mt_rand()%99;

	n->name = malloc (strlen(name) * sizeof(char));
	strcpy(n->name, name);
	n->weights= malloc (n_frames * sizeof (double));

	for(i = 0; i < n_frames; i++)
		for(j = 0; j < N_MFCC; j++)
			n->weights[i] = mfcc_frames[i].features[j];
  
    printf("latitude %d, %d\n", n->x, n->y);
}

map* init_map(int sideX, int sideY, int scale) 
{
	register int i;
	char *name = NULL;
	void **word_adresses;
	unsigned int n = 0, count = 0;
	word *words = malloc(sizeof(word));

    map *_map = malloc(sizeof(map));
    _map->latice_size = sideX * sideY;
    _map->sideX       = sideX; 
    _map->sideY       = sideY; 
    _map->scale       = scale;
    _map->lattice     = malloc(_map->latice_size * sizeof(neuron));


    if ((n = get_list(words)))
	{

		word_adresses = malloc(n * sizeof(void *));
		while (words != NULL)
		{
			init_neuron(_map->lattice, words->frames, words->n, words->name);
			word_adresses[count++] = words;		
			words = words->next;
		}
		for (i = 0; i < count; i++)
			free(word_adresses[i]);
		free(word_adresses);
	}
    
    return _map;
}