#ifndef SETUP_H
#define SETUP_H

#include "definition.h"

int setup_graph_from_args(int argc, char *argv[], GraphStructure **outGraph);
int prepare_generations(Generation *currentGen, Generation *nextGen);
void free_individual_contents(Individual *individual);

#endif
