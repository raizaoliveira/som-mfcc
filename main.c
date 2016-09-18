
#include "link.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdint.h>
#include "neuron.h"

#define true 1


int main(){

    int netSizeX = 10, netSizeY = 10;
    int scale = 4;
    void **word_adresses;
    unsigned int n = 0;
    int  count = 0;
    int epochs = 3000;
    register int i;
    Inputs *teste = malloc(sizeof(Inputs));
    word *words = malloc(sizeof(word));
    map *m;
    if( ( n = get_list(words) ) )
        printf("na main = %d\n", n);
            
    m = init_map(netSizeX, netSizeY, scale);
    puts("inicializou o map");
    word_adresses = malloc(n * sizeof(void *));
    while (words != NULL){
        teste = copy(words->frames, words->n, words->name);
        printf("teste %s\n", teste->name);
        train(m, teste, epochs);
        word_adresses[count++] = words;
        words = words->next;
    }
    printf("count %d\n", count);
    for (i = 0; i < count; i++)
        free(word_adresses[i]);
    free(word_adresses);

    destroy_map(m);
    return 0;
}
    


