#ifndef __NEURON_H__
#define __NEURON_H__

#include "stdio.h"
#include "mtrand.h"
#include "preprocessing.h"
#include "linklist.h"

#include <string.h>

#define EPSILON 0.1

typedef struct _neuron
	{
		mfcc_frame *weights; // compara aos frames da entrada
    	int num_weights; // N de frames
    	char *name; // Nome da palavra
		int x;
		int y;
	} neuron;

typedef struct _map
	{
		neuron *lattice;
    	int latice_size;
    	double mapRadius;
    	int sideX, sideY; 
    	int scale;
	} map;


typedef struct{
	mfcc_frame *input;
	int num_frames;
	char *name;
} Inputs;


void make_quality_map(map *m);

void init_neuron(neuron *n, int x, int y, mfcc_frame *mfcc_frames, int n_frames, char *name);

map* init_map(int sideX, int sideY, int scale);

double neuron_distance(neuron *n, Inputs *s);

int neuron_distance_to(neuron *src, neuron *dst);

neuron* find_bmu(map *m, Inputs *inputs);

void adjust_weights(neuron *n, Inputs *in, double epsilon, double theta);

void epoch(map *m, Inputs *in, int iteration, double timeCst, double *epsilon, int numEpoch);

void destroy_map(map *m);

void train(map *m, Inputs *in, int numEpoch);





#endif
