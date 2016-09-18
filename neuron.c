#include "neuron.h"
#define N_SAMPLES 10

double min_dbl(double a, double b) { return a < b ? a : b; }
double max_dbl(double a, double b) { return a > b ? a : b; }


void init_neuron(neuron *n, int x, int y, mfcc_frame *mfcc_frames,int n_frames, char *name){

    double r;
    register int i, j;
    printf("init neuron\n");
    n->num_weights = n_frames;
    n->x = x; 
    n->y = y;
    n->name = malloc (strlen(name) * sizeof(char));
    strcpy(n->name, name);
    n->weights= malloc (n_frames * sizeof (double));

    for(i = 0; i < n_frames; i++)
        for(j = 0; j < N_MFCC; j++){
            n->weights[i].features[j] = mfcc_frames[i].features[j];
        }
  
}

map* init_map(int sideX, int sideY, int scale){

    int x, y;
    unsigned int i, count = 0;
    int aux = 0;
    unsigned int n;
    void **word_adresses;
    word *pt_aux = malloc(sizeof(word));;
    map *_map = malloc(sizeof(map));
    _map->latice_size = sideX * sideY;
    _map->sideX       = sideX;
    _map->sideY       = sideY; 
    _map->scale       = scale;
    _map->lattice     = malloc(_map->latice_size * sizeof( neuron));
    mt_seed ();

    if ((n = get_list(pt_aux)))
    {
        printf("n: %d\n", n);
        word_adresses = malloc(n * sizeof(void *));
        while (pt_aux != NULL)
        {
            x = mt_rand() %sideX;
            y = mt_rand() %sideY;
            init_neuron(_map->lattice, x, y, pt_aux->frames, pt_aux->n, pt_aux->name);
            word_adresses[count++] = pt_aux;
            pt_aux = pt_aux->next;
        }
        printf("count: %d\n", count);
        for (i = 0; i < count; i++)
            free(word_adresses[i]);
        free(word_adresses);
    }

    return _map;
}




double neuron_distance(neuron *n, Inputs *s){
    register int i, j;
    double val;

    val = 0;
    for(i = 0; i < n->num_weights; ++i) {
        for(j = 0; j < N_MFCC; j++){
            val += (s->input[i].features[j] - n->weights[i].features[j]) * (s->input[i].features[j] - n->weights[i].features[j]);    
        }
        
    }
    return val;
}


int neuron_distance_to(neuron *src, neuron *dst) {
    return (dst->x - src->x) * (dst->x - src->x)
        +  (dst->y - src->y) * (dst->y - src->y);
}



neuron* find_bmu(map *m, Inputs *inputs) {

    double curr;
    register int i;
    double min_val;
    neuron *n = m->lattice;
    min_val = neuron_distance(n, inputs);

    for(i = 1; i < m->latice_size; ++i){
        curr = neuron_distance(&(m->lattice[i]), inputs);
        min_val = min_dbl(curr, min_val);
        if(min_val == curr){
            n = &(m->lattice[i]);
        }
    }

    return n;
}


void adjust_weights(neuron *n, Inputs *in, double epsilon, double theta){
    register int i, j;

    for(i = 0; i < n->num_weights; ++i){
        for(j = 0; j < N_MFCC; j++){
            n->weights[i].features[j] += epsilon * theta * (in->input[i].features[j] - n->weights[i].features[j]);
        }
    }
}

void train(map *m, Inputs *in, int numEpoch){
    int iteration;
    int input_chosen;
    double epsilon = EPSILON;
    double timeCst = numEpoch / log(m->mapRadius); 

    iteration = 0;
    printf("train linha 168\n");
    while(iteration < numEpoch) 
    {
        epoch(m, in, iteration, timeCst, &epsilon, numEpoch);  
        ++iteration;
        if(iteration % 100 == 0) 
        {
            //printf("Epoch %d / %d\n", iteration, numEpoch);
        }
    }
}


void epoch(map *m, Inputs *in, int iteration, double timeCst, double *epsilon, int numEpoch)
{
    register int i;
    double radius;
    int dst;
    double theta;    

    radius = max_dbl(m->mapRadius * exp(-iteration / timeCst), 1); 
    radius *= radius;

    neuron *n = find_bmu(m, in);

    for(i = 0; i < m->latice_size; ++i)
    {
        dst = neuron_distance_to(&(m->lattice[i]), n);
        
        if(dst < radius)
        {
            theta = exp(-dst / (2 * radius));
            adjust_weights(&(m->lattice[i]), in, *epsilon, theta);

        }
        *epsilon = EPSILON * exp((double)-iteration / (numEpoch- iteration));  
    }
}



void destroy_map(map *m) 
{
    register int i;
    //for(i = 0; i < m->latice_size; ++i)
        //free(m->lattice[i].weights);
    free(m->lattice);
    free(m);
}