#include <stdio.h>
#include <stdlib.h>
#include "definition.h"
#include "functions.h"
#include "generation_memory.h"
#include "setup.h"

static void free_individual_contents_impl(Individual *individual)
{
    if (individual == NULL) return;

    free(individual->colorChromosome);
    free(individual->conflictFlags);

    individual->colorChromosome = NULL;
    individual->conflictFlags = NULL;
    individual->chromosomeLength = 0;
    individual->fitnessScore = 0;
    individual->coloringCost = 0.0f;
    individual->conflictPairCount = 0;
}

void free_individual_contents(Individual *individual)
{
    free_individual_contents_impl(individual);
}

int setup_graph_from_args(int argc, char *argv[], GraphStructure **outGraph)
{
    if (argc < 7) {
        printf("引数が不足しています。\n");
        return -1;
    }

    mutationRate     = atof(argv[1]);
    crossoverRate    = atof(argv[2]);
    maxGenerations   = atoi(argv[3]);
    islandPopulation = atoi(argv[4]);
    numberOfIslands  = atoi(argv[5]);

    FILE *fp = fopen(argv[6], "r");
    if (!fp) {
        perror("file");
        return -1;
    }

    GraphStructure *graph = read_graph_csv(fp);
    fclose(fp);
    if (!graph) {
        fprintf(stderr, "グラフの読み込みに失敗しました。\n");
        return -1;
    }

    *outGraph = graph;
    return 0;
}

int prepare_generations(Generation *currentGen, Generation *nextGen)
{
    if (initialize_generation(currentGen) != 0 ||
        initialize_generation(nextGen) != 0) {
        fprintf(stderr, "世代の初期化に失敗しました。\n");
        free_generation(currentGen);
        free_generation(nextGen);
        return -1;
    }

    return 0;
}
