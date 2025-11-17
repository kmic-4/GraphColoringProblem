#include <stdlib.h>
#include <float.h>
#include "definition.h"
#include "functions.h"

// 第一世代の生成
void generate_first_generation(Generation *gen,
                             const GraphStructure *graph,
                             int numberOfColor)
{
    // 全島に対するループ
    for (int islandIdx = 0; islandIdx < gen->numberOfIslands; islandIdx++) {

        Island *island = &gen->islands[islandIdx];
        Individual *inds = island->individuals;

        float bestCost = FLT_MAX;
        int bestIdx = 0;

        // 各島内の個体生成
        for (int j = 0; j < islandPopulation; j++) {

            Individual *ind = &inds[j];

            /* 1. 遺伝子長の決定 */
            ind->chromosomeLength = graph->numberOfVertices;

            /* 2. colorChromosome の確保 */
            ind->colorChromosome =
                (int *)malloc(sizeof(int) * ind->chromosomeLength);

            if (ind->colorChromosome == NULL) {
                // 致命的エラー
                exit(EXIT_FAILURE);
            }

            /* 3. 乱数による初期彩色 */
            for (int v = 0; v < ind->chromosomeLength; v++) {
                ind->colorChromosome[v] = rand() % numberOfColor;
            }

            /* 4. ラベル正規化 */
            normalize_chromosome_labels(ind);

            /* 5. fitnessScore の計算 */
            ind->fitnessScore = evaluate_color_variety_fitness(ind);

            /* 6. ペナルティの計算 */
            int penalty = calculate_penalty(graph, ind);
            if (penalty < 0) {
                // realloc エラーなど
                exit(EXIT_FAILURE);
            }

            /* 7. coloringCost の計算（逆数）*/
            float denom = (float)(ind->fitnessScore + penalty);

            // 0除算対策（念のため）
            if (denom <= 0.0f) {
                denom = 1.0f;
            }

            ind->coloringCost = 1.0f / denom;

            /* 8. 島内最良の更新 */
            if (ind->coloringCost < bestCost) {
                bestCost = ind->coloringCost;
                bestIdx = j;
            }
        }

        /* 9. 島内最良個体のコピー */
        island->islandBestIndividual = inds[bestIdx];
    }

    /* ---------------------------------------------------
       全島の中で最良の islandBestIndividual を選抜
       --------------------------------------------------- */
    float globalBestCost = FLT_MAX;
    int globalBestIsland = 0;

    for (int i = 0; i < gen->numberOfIslands; i++) {
        if (gen->islands[i].islandBestIndividual.coloringCost < globalBestCost) {
            globalBestCost = gen->islands[i].islandBestIndividual.coloringCost;
            globalBestIsland = i;
        }
    }

    gen->globalBestIndividual = gen->islands[globalBestIsland].islandBestIndividual;
}
