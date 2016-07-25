#ifndef __PREPROCESSING_H__
#define __PREPROCESSING_H__


#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include "inaudio.h"

#define N 512
#define QTD_FILTER 20
#define N_MFCC QTD_FILTER 
#define OVERLAY 64

typedef
	struct
	{
		int16_t *buffer;/*inteiro de 16 bits*/
		unsigned int number;
	} voice_signal;


/*
  * Estrutura constituindo um quadro.
  * Os valores no quadro são as intensidades da representação da função no domínio da frequência.
  * Magnitudes (duplo), as intensidades dos valores Fourier transformado.
  */
typedef struct
	{
		double magnitudes[N];
	} frame;

typedef struct
	{
		double features[N_MFCC];
	} mfcc_frame;

typedef struct
	{
		double real;
		double imag;
	}comp;


int split(int16_t *buffer, unsigned int n, voice_signal **signals);

unsigned int make_frames_hamming(int16_t *buffer, unsigned int n, frame **frames);

void fft(comp *time, comp *freq, unsigned int n);

void mfcc_features(frame *frames, unsigned int n, mfcc_frame *mfcc_frames);

#endif
