#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#include "definition.h"
#include "functions.h"
#include "generation_memory.h"
#include "setup.h"

double mutationRate = 0.0; //　依存注入的な、実体の生成
double crossoverRate = 0.0;
int islandPopulation = 0;
int numberOfIslands = 0;
int maxGenerations = 0;
int iterationsNumber = 0;

void run_generations(Generation *currentGen,
                     Generation *nextGen,
                     GraphStructure *graph)
{
    generate_first_generation(currentGen, graph, REQUIRED_COLORS_FOR_SUCCESS);

    // CSVファイルを書き込みモードでオープン
    FILE *csvFp = fopen("generation_stats.csv", "w");
    if (csvFp != NULL) {
        // ヘッダー行を出力
        fprintf(csvFp, "Generation,ColoringCost,Iterations\n");
    } else {
        fprintf(stderr, "Warning: Could not open generation_stats.csv for writing.\n");
    }

    // 並列設定
    omp_set_num_threads(8);

    for (int gen = 0; gen < maxGenerations; gen++) {

        nextGen->generationIndex = currentGen->generationIndex + 1;

        #pragma omp parallel for
        for (int i = 0; i < numberOfIslands; i++) {
            unsigned int seed = (unsigned int)(i + gen * numberOfIslands); 
            generate_next_generation_island(
                &nextGen->islands[i],
                &currentGen->islands[i],
                graph,
                &seed
            );
        }

        update_global_best(nextGen);

        printf("Generation %d | ColoringCost=%f | Iterations=%d\n",
            nextGen->generationIndex,
            nextGen->globalBestIndividual.coloringCost,
            iterationsNumber);
        
        // CSVファイルへ現在の世代の統計情報を出力
        if (csvFp != NULL) {
            append_generation_result_to_csv(csvFp,
                nextGen->generationIndex,
                nextGen->globalBestIndividual.coloringCost,
                iterationsNumber);
        }

        fflush(stdout);

        if (nextGen->globalBestIndividual.fitnessScore == 4 &&
            calculate_penalty(graph, &nextGen->globalBestIndividual) == 0) {
            printf("彩色数4の最適個体を発見（世代 %d）\n", nextGen->generationIndex);
            break;
        }

        if (gen % 10 == 0) {
            migrate(nextGen);
            update_global_best(nextGen);
        }

        Generation tmp = *currentGen;
        *currentGen = *nextGen;
        *nextGen = tmp;
    }

    // CSVファイルをクローズ
    if (csvFp != NULL) {
        fclose(csvFp);
    }
}

int main(int argc, char *argv[])
{
    GraphStructure *graph = NULL;
    if (setup_graph_from_args(argc, argv, &graph) != 0) {
        return 1;
    }

    Generation currentGen = {0};
    Generation nextGen = {0};

    if (prepare_generations(&currentGen, &nextGen) != 0) {
        free_graph(graph);
        return 1;
    }

    run_generations(&currentGen, &nextGen, graph);

    free_generation(&currentGen);
    free_generation(&nextGen);
    free_graph(graph);

    printf("Total iterations: %d\n", iterationsNumber);

    return 0;
}
