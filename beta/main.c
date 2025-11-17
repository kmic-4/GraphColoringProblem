#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#include "definition.h"
#include "functions.h"

double mutationRate = 0.0;
double crossoverRate = 0.0;
int islandPopulation = 0;
int numberOfIslands = 0;
int maxGenerations = 0;

static void free_individual_contents(Individual *individual)
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

static void free_generation(Generation *gen)
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

static int initialize_generation(Generation *gen)
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

static void free_graph(GraphStructure *graph)
{
    if (graph == NULL) return;

    for (int i = 0; i < graph->numberOfVertices; i++) {
        free(graph->adjacencyMatrix[i]);
    }
    free(graph->adjacencyMatrix);
    free(graph);
}

int main(int argc, char *argv[])
{
    if (argc < 7) return printf("引数が不足しています。\n"), 1;

    mutationRate     = atof(argv[1]);
    crossoverRate    = atof(argv[2]);
    maxGenerations   = atoi(argv[3]);
    islandPopulation = atoi(argv[4]);
    numberOfIslands  = atoi(argv[5]);

    FILE *fp = fopen(argv[6], "r");
    if (!fp) { perror("file"); return 1; }

    GraphStructure *graph = read_graph_csv(fp);
    fclose(fp);
    if (!graph) {
        fprintf(stderr, "グラフの読み込みに失敗しました。\n");
        return 1;
    }

    Generation currentGen = {0};
    Generation nextGen = {0};

    if (initialize_generation(&currentGen) != 0 ||
        initialize_generation(&nextGen) != 0) {
        fprintf(stderr, "世代の初期化に失敗しました。\n");
        free_generation(&currentGen);
        free_generation(&nextGen);
        free_graph(graph);
        return 1;
    }

    generate_first_generation(&currentGen, graph, REQUIRED_COLORS_FOR_SUCCESS);

    // 並列設定
    omp_set_num_threads(8);

    for (int gen = 0; gen < maxGenerations; gen++) {

        nextGen.generationIndex = currentGen.generationIndex + 1;

        #pragma omp parallel for
        for (int i = 0; i < numberOfIslands; i++) {
            generate_next_generation_island(
                &nextGen.islands[i],
                &currentGen.islands[i],
                graph
            );
        }

        update_global_best(&nextGen);

        printf("Generation %d | FitnessScore=%d | ColoringCost=%f\n",
            nextGen.generationIndex,
            nextGen.globalBestIndividual.fitnessScore,
            nextGen.globalBestIndividual.coloringCost);

        fflush(stdout);

        if (nextGen.globalBestIndividual.fitnessScore == 4 && calculate_penalty(graph, &nextGen.globalBestIndividual) == 0) {
            printf("彩色数4の最適個体を発見（世代 %d）\n", nextGen.generationIndex);
            break;
        }

        if (gen % 10 == 0) {
            migrate(&nextGen);
            update_global_best(&nextGen);
        }

        Generation tmp = currentGen;
        currentGen = nextGen;
        nextGen = tmp;
    }

    free_generation(&currentGen);
    free_generation(&nextGen);
    free_graph(graph);

    return 0;
}
