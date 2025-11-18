#include <stdlib.h>
#include "definition.h"
#include "functions.h"
#include "setup.h"
#include "generation_memory.h"

int initialize_generation(Generation *gen)
{
    gen->generationIndex = 0;
    gen->numberOfIslands = numberOfIslands;
    gen->islands = (Island *)calloc(numberOfIslands, sizeof(Island));
    if (gen->islands == NULL) {
        return -1;
    }

    for (int i = 0; i < numberOfIslands; i++) {
        gen->islands[i].individuals = (Individual *)calloc(
            islandPopulation, sizeof(Individual));
        if (gen->islands[i].individuals == NULL) {
            gen->numberOfIslands = i;
            free_generation(gen);
            return -1;
        }
    }

    gen->numberOfIslands = numberOfIslands;
    gen->globalBestIndividual.colorChromosome = NULL;
    gen->globalBestIndividual.conflictFlags = NULL;

    return 0;
}

void free_generation(Generation *gen)
{
    if (gen == NULL || gen->islands == NULL) {
        return;
    }

    for (int i = 0; i < gen->numberOfIslands; i++) {
        Island *island = &gen->islands[i];
        if (island->individuals != NULL) {
            for (int j = 0; j < islandPopulation; j++) {
                free_individual_contents(&island->individuals[j]);
            }
            free(island->individuals);
            island->individuals = NULL;
        }
        free_individual_contents(&island->islandBestIndividual);
    }

    free_individual_contents(&gen->globalBestIndividual);
    free(gen->islands);
    gen->islands = NULL;
    gen->numberOfIslands = 0;
}

void free_graph(GraphStructure *graph)
{
    if (graph == NULL) return;

    for (int i = 0; i < graph->numberOfVertices; i++) {
        free(graph->adjacencyMatrix[i]);
    }
    free(graph->adjacencyMatrix);
    free(graph);
}
