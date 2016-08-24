#include "neuron.h"


void init_neuron(neuron *n, int x, int y, mfcc_frame *mfcc_frames, unsigned int n_frames, char *name){

	double r;
	register int i, j;
	n->frames = mfcc_frames;
	n->num_weights = n_frames;
	n->x = x; 
	n->y = y;

	n->name = malloc (strlen(name) * sizeof(char));
	strcpy(n->name, name);
	n->weights= malloc (n_frames * sizeof (double));

	for(i = 0; i < n_frames; i++)
		for(j = 0; j < N_MFCC; j++)
			n->weights[i] = mfcc_frames[i].features[j];
  
    printf("%s lattice %d, %d\n", n->name, n->x, n->y);
}

map* init_map(int sideX, int sideY, int scale){
	register int i, x, y;
	char *name = NULL;
	void **word_adresses;
	unsigned int n = 0, count = 0;
	int aux = 0;
	word *words = malloc(sizeof(word));

    map *_map = malloc(sizeof(map));
    _map->latice_size = sideX * sideY;
    _map->sideX       = sideX;
    _map->sideY       = sideY; 
    _map->scale       = scale;
    _map->lattice     = malloc(_map->latice_size * sizeof(neuron));
    mt_seed ();

    if ((n = get_list(words))){
		word_adresses = malloc(n * sizeof(void *));
		while (words != NULL){
			x = mt_rand() %sideX;
			y = mt_rand() %sideY;
			printf("y : %d  x: %d\n", y, x);
			init_neuron(_map->lattice + y * sideX + x, x, y, words->frames, words->n, words->name);
				
			word_adresses[count++] = words;		
			words = words->next;
		}
		for (i = 0; i < count; i++)
			free(word_adresses[i]);
		free(word_adresses);
		aux++;
	}
    
    return _map;
}

/*void train(Map *m, int num_inputs, int numEpoch) 
{
    int iteration = 0;
    double epsilon = EPSILON;
    double timeCst = numEpoch / log(m->mapRadius); 

    while(iteration < numEpoch) 
    {
        int input_chosen = rand() % num_inputs;
        double *input = (inputs + input_chosen)->data; 
        epoch(m, input, iteration, timeCst, &epsilon, numEpoch);  
        ++iteration;
        draw(display, m);
        if(iteration % 100 == 0) 
        {
            printf("Epoch %d / %d\n", iteration, numEpoch);
        }
    }
}


double neuron_distance(Neuron *n, mfcc_frame *mfcc_frames2, unsigned int n2) 
{
    double val = 0;
    register int i;
  
    for (j = 0; j < n2; j++)
	{
			
		val = 0;/*Distancias euclidianas
		for (k = 0; k < N_MFCC; k++)
		{
			val += pow(n->weights[k] - mfcc_frames2[j].features[k], 2);
			
		}
		val = sqrt(distances[i + 1][j + 1]);
	}
    return val;
}

int neuron_distance_to(Neuron *src, Neuron *dst) 
{
    return (dst->x - src->x) * (dst->x - src->x)
        +  (dst->y - src->y) * (dst->y - src->y);
}



Neuron* find_bmu(Map *m, mfcc_frame *mfcc_frames2, unsigned int n2) 
{
    Neuron *n = m->lattice;
    double min_val = neuron_distance(n, inputs);

    for(int i = 1; i < m->latice_size; ++i) 
    {
        double curr = neuron_distance(&(m->lattice[i]), inputs);
        min_val = min_dbl(curr, min_val);
        if(min_val == curr) { n = &(m->lattice[i]); }
    }

    return n;
}


void adjust_weights(Neuron *n, double *inputs, double epsilon, double theta) 
{
    for(int i = 0; i < n->num_weights; ++i) 
    {
        n->weights[i] += epsilon * theta * (inputs[i] - n->weights[i]);
    }
}


void train(Map *m, Training *inputs, int num_inputs, int numEpoch, SDL_Surface *display) 
{
    int iteration = 0;
    double epsilon = EPSILON;
    double timeCst = numEpoch / log(m->mapRadius); 

    while(iteration < numEpoch) 
    {
        int input_chosen = rand() % num_inputs;
        double *input = (inputs + input_chosen)->data; 
        epoch(m, input, iteration, timeCst, &epsilon, numEpoch);  
        ++iteration;
        draw(display, m);
        if(iteration % 100 == 0) 
        {
            printf("Epoch %d / %d\n", iteration, numEpoch);
        }
    }
}


double distance_between_weights(Neuron *src, Neuron *dst) 
{
    double dist = 0;
    for(int i = 0; i < src->num_weights; ++i)
    {
        dist += dst->weights[i] - src->weights[i];
    }
    return dist < 0 ? -dist : dist;
}

void epoch(Map *m, double *inputs, int iteration, double timeCst, double *epsilon, int numEpoch)
{
    double radius = max_dbl(m->mapRadius * exp(-iteration / timeCst), 1); 
    radius *= radius;

    Neuron *n = find_bmu(m, inputs);

    for(int i = 0; i < m->latice_size; ++i)
    {
        int dst = neuron_distance_to(&(m->lattice[i]), n);
        
        if(dst < radius)
        {
            double theta = exp(-dst / (2 * radius));
            adjust_weights(&(m->lattice[i]), inputs, *epsilon, theta);

        }
        *epsilon = EPSILON * exp((double)-iteration / (numEpoch- iteration));  
    }
}

int get_node_quality(Map *m, int x, int y, Neuron *n)
{
    double quality = 0.;

    if(x != 0) { // left Neuron (if exists)  
        quality += neuron_distance_to(n, &(m->lattice[y * m->sideX + x - 1])); 
    }
    if(x != m->sideX - 1) { //right Neuron
        quality += neuron_distance_to(n, &(m->lattice[y * m->sideX + x + 1])); 

    }
    if(y != 0) { // top neuron  
        quality += neuron_distance_to(n, &(m->lattice[(y - 1) * m->sideX + x])); 
    }
    if(y != m->sideY - 1) { // bottom Neuron
        quality += neuron_distance_to(n, &(m->lattice[(y + 1) * m->sideX + x])); 
    }

    return quality;
}



void destroy_map(Map *m) 
{
    for(int i = 0; i < m->latice_size; ++i)
    {
        free(m->lattice[i].weights);
    }
    free(m->lattice);
    free(m);
}*/