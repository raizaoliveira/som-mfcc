#ifndef __NEURON_H__
#define __NEURON_H__

#include "stdio.h"
#include "mtrand.h"
#include "preprocessing.h"
#include "linklist.h"
#include <string.h>


typedef struct _neuron
	{
		mfcc_frame *frames;
		char *name;
		double *weights;
    	int num_weights;
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

void init_neuron(neuron *n, mfcc_frame *mfcc_frames, unsigned int n_frames, char *name);

map* init_map(int sideX, int sideY, int scale);

#endif
