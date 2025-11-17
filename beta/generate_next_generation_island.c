#include <stdlib.h>
#include <float.h>

#include "definition.h"
#include "functions.h"


// ルーレット選択：coloringCost に比例して個体を選択（方式A）
static Individual* roulette_select(Individual *inds, int population)
{
    double sum = 0.0;

    for (int i = 0; i < population; i++) {
        sum += inds[i].coloringCost;
    }

    double r = ((double)rand() / RAND_MAX) * sum;

    double acc = 0.0;
    for (int i = 0; i < population; i++) {
        acc += inds[i].coloringCost;
        if (acc >= r) {
            return &inds[i];
        }
    }

    /* 理論上到達しないが、念のため最後を返す */
    return &inds[population - 1];
}



//次世代を生成する（ルーレット選択 + conflictFlags 交叉 + mutation）
void generate_next_generation(Generation *nextGen,
                              const Generation *curGen,
                              const GraphStructure *graph)
{
    nextGen->generationIndex = curGen->generationIndex + 1;

    for (int islandIdx = 0; islandIdx < curGen->numberOfIslands; islandIdx++) {

        const Island *parentIsland = &curGen->islands[islandIdx];
        Island *childIsland       = &nextGen->islands[islandIdx];

        Individual *parents  = parentIsland->individuals;
        Individual *children = childIsland->individuals;

        float bestCost = FLT_MAX;
        int   bestIdx  = 0;

        //1. 各 Island 内でルーレット選択しペアを作成 → 交叉
        for (int k = 0; k < islandPopulation; k += 2) {

            /* 親をルーレット選択（方式A） */
            Individual *p1 = roulette_select(parents, islandPopulation);
            Individual *p2 = roulette_select(parents, islandPopulation);

            /* 子供の格納先 */
            Individual *c1 = &children[k];
            Individual *c2 = &children[k + 1];

            int len = p1->chromosomeLength;

            /* 子個体のメモリ確保 */
            c1->chromosomeLength = len;
            c2->chromosomeLength = len;

            c1->colorChromosome = malloc(sizeof(int) * len);
            c2->colorChromosome = malloc(sizeof(int) * len);
            c1->conflictFlags   = malloc(sizeof(int) * len);
            c2->conflictFlags   = malloc(sizeof(int) * len);

            if (!c1->colorChromosome || !c2->colorChromosome ||
                !c1->conflictFlags   || !c2->conflictFlags) {
                exit(EXIT_FAILURE);
            }

            //交叉を行うか判定
            if ((double)rand() / RAND_MAX < crossoverRate) {

                /* 一旦コピー */
                for (int v = 0; v < len; v++) {
                    c1->colorChromosome[v] = p1->colorChromosome[v];
                    c2->colorChromosome[v] = p2->colorChromosome[v];
                }

                /* conflictFlags に応じて修正する交叉 */
                for (int v = 0; v < len; v++) {
                    if (p1->conflictFlags[v]) {
                        c1->colorChromosome[v] = p2->colorChromosome[v];
                    }
                    if (p2->conflictFlags[v]) {
                        c2->colorChromosome[v] = p1->colorChromosome[v];
                    }
                }

            } else {
                /* 交叉なし → 親をそのままコピー */
                for (int v = 0; v < len; v++) {
                    c1->colorChromosome[v] = p1->colorChromosome[v];
                    c2->colorChromosome[v] = p2->colorChromosome[v];
                }
            }

            /* ★修正（問題1）：交叉直後の conflictFlags を確定させる */
            calculate_penalty(graph, c1);
            calculate_penalty(graph, c2);
        }


        // 2. mutation（突然変異：swap）
        for (int j = 0; j < islandPopulation; j++) {

            Individual *ind = &children[j];
            int len = ind->chromosomeLength;

            if ((double)rand() / RAND_MAX < mutationRate) {

                int a = rand() % len;
                int b = rand() % len;

                int tmp = ind->colorChromosome[a];
                ind->colorChromosome[a] = ind->colorChromosome[b];
                ind->colorChromosome[b] = tmp;
            }

            /* ★修正（問題1）：mutation 後の conflictFlags 再更新 */
            calculate_penalty(graph, ind);
        }


        //3. 評価（fitnessScore / coloringCost / islandBest）
        for (int j = 0; j < islandPopulation; j++) {

            Individual *ind = &children[j];

            normalize_chromosome_labels(ind);
            ind->fitnessScore = evaluate_color_variety_fitness(ind);

            int penalty = calculate_penalty(graph, ind);
            if (penalty < 0) exit(EXIT_FAILURE);

            float denom = (float)(ind->fitnessScore + penalty);
            if (denom <= 0.0f) denom = 1.0f;

            ind->coloringCost = 1.0f / denom;

            if (ind->coloringCost < bestCost) {
                bestCost = ind->coloringCost;
                bestIdx  = j;
            }
        }

        /* ★修正（問題2）：deep copy により island best を確保 */
        copy_individual(&childIsland->islandBestIndividual,
                        &children[bestIdx]);
    }


    //4. global best 個体の決定（deep copy）
    float globalBestCost   = FLT_MAX;
    int   globalBestIsland = 0;

    for (int i = 0; i < nextGen->numberOfIslands; i++) {

        if (nextGen->islands[i].islandBestIndividual.coloringCost < globalBestCost) {
            globalBestCost   = nextGen->islands[i].islandBestIndividual.coloringCost;
            globalBestIsland = i;
        }
    }

    copy_individual(&nextGen->globalBestIndividual,
                    &nextGen->islands[globalBestIsland].islandBestIndividual);
}
