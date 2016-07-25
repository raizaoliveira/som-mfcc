/*
Biblioteca linklist.h. Funções para manipulacao de lista simplesmente encadeada.
Autores: Raiza Arteman e Willian Souza
Curso de Ciência da Computação
Universidade estadual de Mato Grosso do Sul - UEMS
*/
#ifndef __LINKLIST_H__
#define __LINKLIST_H__

#include "preprocessing.h"
#include <dirent.h>

typedef
	struct _word
	{
		mfcc_frame *frames;
		unsigned int n;
		char *name;
		struct _word *next;
	} word;


void new_word(mfcc_frame *frames, unsigned int n, char *name);

int get_list(word *head);

#endif
