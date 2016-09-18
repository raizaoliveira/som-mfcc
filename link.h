#ifndef __LINK_H__
#define __LINK_H__

#include "linklist.h"
#include "preprocessing.h"
#include "neuron.h"

void get_word();

Inputs *copy(mfcc_frame *mfcc_frames, int n_frames, char *name);

#endif
