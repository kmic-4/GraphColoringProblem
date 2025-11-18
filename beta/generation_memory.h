#ifndef GENERATION_MEMORY_H
#define GENERATION_MEMORY_H

#include "definition.h"

int initialize_generation(Generation *gen);
void free_generation(Generation *gen);
void free_graph(GraphStructure *graph);

#endif
