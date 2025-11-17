#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#include "definition.h"
#include "functions.h"

int main(int argc, char *argv[])
{
    if (argc < 6) return printf("引数が不足しています。\n"), 1;

    mutationRate     = atof(argv[1]);
    crossoverRate    = atof(argv[2]);
    maxGenerations   = atoi(argv[3]);
    islandPopulation = atoi(argv[4]);
    numberOfIslands  = atoi(argv[5]);

    FILE *fp = fopen(argv[6], "r");
    if (!fp) { perror("file"); return 1; }

    GraphStructure *graph = read_graph_csv(fp);
    fclose(fp);

    Generation currentGen, nextGen;

    generate_first_generation(&currentGen, graph);

    /* 並列設定 */
    omp_set_num_threads(8);

    for (int gen = 0; gen < maxGenerations; gen++) {

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

        if (nextGen.globalBestIndividual.fitnessScore == 4) {
            printf("彩色数4の最適個体を発見（世代 %d）\n", nextGen.generationIndex);
            break;
        }

        if (gen % 10 == 0) {
            migrate(&nextGen);
            update_global_best(&nextGen);
        }

        currentGen = nextGen;
}


    return 0;
}
