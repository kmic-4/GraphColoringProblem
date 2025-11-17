#include <stdlib.h>
#include <float.h>

#include "definition.h"
#include "functions.h"


// 以前使用していたルーレット選択
// static Individual* roulette_select(Individual *inds, int population)
// {
//     double sum = 0.0;
//
//     for (int i = 0; i < population; i++) {
//         double weight = inds[i].coloringCost;
//         if (weight < 0.0) weight = 0.0;
//         sum += weight;
//     }
//
//     if (sum <= 0.0) {
//         return &inds[rand() % population];
//     }
//
//     double r = ((double)rand() / RAND_MAX) * sum;
//
//     double acc = 0.0;
//     for (int i = 0; i < population; i++) {
//         double weight = inds[i].coloringCost;
//         if (weight < 0.0) weight = 0.0;
//         acc += weight;
//         if (acc >= r) {
//             return &inds[i];
//         }
//     }
//
//     return &inds[population - 1];
// }

#define TOURNAMENT_SIZE 3

static Individual* tournament_select(Individual *inds, int population)
{
    Individual *best = NULL;
    float bestCost = -FLT_MAX;

    for (int i = 0; i < TOURNAMENT_SIZE; i++) {
        int idx = rand() % population;
        Individual *candidate = &inds[idx];
        if (candidate->coloringCost > bestCost) {
            bestCost = candidate->coloringCost;
            best = candidate;
        }
    }

    return best ? best : &inds[0];
}



static void prepare_child(Individual *child, int length)
{
    child->chromosomeLength = length;

    free(child->colorChromosome);
    child->colorChromosome = malloc(sizeof(int) * length);
    if (child->colorChromosome == NULL) {
        exit(EXIT_FAILURE);
    }

    free(child->conflictFlags);
    child->conflictFlags = NULL; // calculate_penalty() が再確保する
}

//複数交差点で交叉する数
#define MULTI_CROSSOVER_POINTS 2

// 次世代を生成する（トーナメント選択 + 複数交差点交叉 + mutation）
void generate_next_generation_island(Island *nextIsland,
                                     const Island *curIsland,
                                     const GraphStructure *graph)
{
    Individual *parents  = curIsland->individuals;
    Individual *children = nextIsland->individuals;

    float bestCost = -FLT_MAX;
    int   bestIdx  = 0;

    // 1. 各 Island 内でトーナメント選択しペアを作成 → 交叉
    for (int k = 0; k < islandPopulation; k += 2) {

        // 親をトーナメント選択
        Individual *p1 = tournament_select(parents, islandPopulation);
        Individual *p2 = tournament_select(parents, islandPopulation);

        // 子供の格納先
        Individual *c1 = &children[k];
        Individual *c2 = &children[k + 1];

        int len = p1->chromosomeLength;

        prepare_child(c1, len);
        prepare_child(c2, len);

        //交叉を行うか判定
        if ((double)rand() / RAND_MAX < crossoverRate && len > 1) {

            int cutCount = len - 1;
            if (cutCount > MULTI_CROSSOVER_POINTS) {
                cutCount = MULTI_CROSSOVER_POINTS;
            }

            int cutPoints[MULTI_CROSSOVER_POINTS] = {0};
            for (int cp = 0; cp < cutCount; cp++) {
                int cut;
                int duplicate;
                do {
                    cut = rand() % (len - 1) + 1; // 1..len-1
                    duplicate = 0;
                    for (int prev = 0; prev < cp; prev++) {
                        if (cutPoints[prev] == cut) {
                            duplicate = 1;
                            break;
                        }
                    }
                } while (duplicate);
                cutPoints[cp] = cut;
            }

            // ソート（挿入法で十分）
            for (int cp = 1; cp < cutCount; cp++) {
                int key = cutPoints[cp];
                int j = cp - 1;
                while (j >= 0 && cutPoints[j] > key) {
                    cutPoints[j + 1] = cutPoints[j];
                    j--;
                }
                cutPoints[j + 1] = key;
            }

            int prevIndex = 0;
            int takeFromP1 = 1;
            for (int seg = 0; seg <= cutCount; seg++) {
                int end = (seg == cutCount) ? len : cutPoints[seg];
                for (int v = prevIndex; v < end; v++) {
                    if (takeFromP1) {
                        c1->colorChromosome[v] = p1->colorChromosome[v];
                        c2->colorChromosome[v] = p2->colorChromosome[v];
                    } else {
                        c1->colorChromosome[v] = p2->colorChromosome[v];
                        c2->colorChromosome[v] = p1->colorChromosome[v];
                    }
                }
                takeFromP1 = !takeFromP1;
                prevIndex = end;
            }

        } else {
            // 交叉なし → 親をそのままコピー
        for (int v = 0; v < len; v++) {
            c1->colorChromosome[v] = p1->colorChromosome[v];
            c2->colorChromosome[v] = p2->colorChromosome[v];
        }
        }

        // 交叉後に conflictFlags を更新
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

        // 突然変異後に conflictFlags を更新
        calculate_penalty(graph, ind);
    }


    //3. 評価（fitnessScore / coloringCost / islandBest）
    for (int j = 0; j < islandPopulation; j++) {

        Individual *ind = &children[j];

        normalize_chromosome_labels(ind);
        evaluate_color_variety_fitness(ind);

        int penalty = calculate_penalty(graph, ind);
        if (penalty < 0) exit(EXIT_FAILURE);

        float denom = (float)(ind->fitnessScore + penalty);
        if (denom <= 0.0f) denom = 1.0f;

        ind->coloringCost = 1.0f / denom;

        if (ind->coloringCost > bestCost) {
            bestCost = ind->coloringCost;
            bestIdx  = j;
        }
    }

    // deep copy で島内最良を保存
    copy_individual(&nextIsland->islandBestIndividual,
                    &children[bestIdx]);
}
