#include "neuron.h"
#define N_SAMPLES 10

double min_dbl(double a, double b) { return a < b ? a : b; }
double max_dbl(double a, double b) { return a > b ? a : b; }


void init_neuron(neuron *n, int x, int y, mfcc_frame *mfcc_frames,int n_frames, char *name){

    double r;
    register int i, j;
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
            printf("Epoch %d / %d\n", iteration, numEpoch);
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

/*
int get_node_quality(map *m, int x, int y, neuron *n)
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


void make_quality_map(map *m){
    for(int y = 0; y < m->sideY; ++y)
        for(int x = 0; x < m->sideX; ++x) { 
            int quality = get_node_quality(m, x, y, m->lattice + y * m->sideX +x);
            int grayscale = 255 / (1 + quality);
            for (size_t k = 0; k < (size_t)m->scale; k++)
                for (size_t l = 0; l < (size_t)m->scale; l++)
                    setPixel(display, x * m->scale  + k, y * m->scale + l, grayscale, grayscale, grayscale);
        }
    
}
*/

void destroy_map(map *m) 
{
    for(int i = 0; i < m->latice_size; ++i)
    {
        free(m->lattice[i].weights);
    }
    free(m->lattice);
    free(m);
}