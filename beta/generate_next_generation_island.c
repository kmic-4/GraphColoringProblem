#include <stdlib.h>
#include <float.h>

#include "definition.h"
#include "functions.h"

// ルーレット選択 (Roulette Selection)
// 適応度（coloringCost）に比例した確率で個体を選択する
static Individual* roulette_select(Individual *inds, int population)
{
    double sum = 0.0;

    for (int i = 0; i < population; i++) {
        // Individual構造体のcoloringCostメンバを使用
        double weight = inds[i].coloringCost;
        if (weight < 0.0) weight = 0.0;
        sum += weight;
    }

    if (sum <= 0.0) {
        return &inds[rand() % population];
    }

    double r = ((double)rand() / RAND_MAX) * sum;

    double acc = 0.0;
    for (int i = 0; i < population; i++) {
        double weight = inds[i].coloringCost;
        if (weight < 0.0) weight = 0.0;
        acc += weight;
        if (acc >= r) {
            return &inds[i];
        }
    }

    return &inds[population - 1];
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
    child->conflictFlags = NULL; 
}

// 次世代生成関数 
//  nextIsland: 次世代の島データ（Island*）。ここに生成された個体が格納される。
//  currentIsland: 現世代の島データ（const Island*）。ここから親が選ばれる。
void generate_next_generation_island(Island *nextIsland,
                                     const Island *currentIsland,
                                     const GraphStructure *graph)
{
    // Island構造体のindividualsメンバにアクセス。個体群配列の先頭ポインタを取得。
    Individual *parents  = currentIsland->individuals;
    Individual *children = nextIsland->individuals;

    float bestCost = -FLT_MAX;
    int   bestIdx  = 0;

    for (int k = 0; k < islandPopulation; k += 2) {

        // 親をルーレット選択
        // roulette_select関数を呼び出し、親となるIndividualへのポインタを取得。
        Individual *p1 = roulette_select(parents, islandPopulation);
        Individual *p2 = roulette_select(parents, islandPopulation);

        // 子供の格納先
        Individual *c1 = &children[k];
        Individual *c2 = &children[k + 1];

        // Individual構造体のchromosomeLengthメンバから染色体長（頂点数）を取得。
        int len = p1->chromosomeLength;

        prepare_child(c1, len);
        prepare_child(c2, len);

        // 交叉を行うか判定
        if ((double)rand() / RAND_MAX < crossoverRate) {
            
            // Individual構造体のcolorChromosomeにアクセスし、ベースとなる親の遺伝子をコピー
            for (int i = 0; i < len; i++) {
                c1->colorChromosome[i] = p2->colorChromosome[i];
                c2->colorChromosome[i] = p1->colorChromosome[i];
            }

            // 親 p1 の中で衝突していない頂点をc1にコピー
            for (int i = 0; i < len; i++) {
                if (p1->conflictFlags && p1->conflictFlags[i] == 0) {
                    c1->colorChromosome[i] = p1->colorChromosome[i];
                }
            }

            // 親p2の中で衝突していない頂点をc2にコピー
            for (int i = 0; i < len; i++) {
                if (p2->conflictFlags && p2->conflictFlags[i] == 0) {
                    c2->colorChromosome[i] = p2->colorChromosome[i];
                }
            }

        } else {
            // 交叉なしなら親をそのままコピー
            for (int v = 0; v < len; v++) {
                c1->colorChromosome[v] = p1->colorChromosome[v];
                c2->colorChromosome[v] = p2->colorChromosome[v];
            }
        }

        // conflictFlags を更新
        calculate_penalty(graph, c1);
        calculate_penalty(graph, c2);
    }


    // 突然変異
    for (int j = 0; j < islandPopulation; j++) {

        Individual *ind = &children[j];
        int len = ind->chromosomeLength;

        if ((double)rand() / RAND_MAX < mutationRate) {

            // 異なる2つの頂点を選択
            int a = rand() % len;
            int b;
            
            // aと異なるbを選ぶ
            do {
                b = rand() % len;
            } while (b == a && len > 1);

            // 2頂点の色を入れ替え
            int tmp = ind->colorChromosome[a];
            ind->colorChromosome[a] = ind->colorChromosome[b];
            ind->colorChromosome[b] = tmp;
        }

        // 突然変異後に conflictFlags を更新
        calculate_penalty(graph, ind);
    }


    // 適応度の計算と島内最良の更新
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

    // deep copy で島内最良をislandBestIndividualに保存
    copy_individual(&nextIsland->islandBestIndividual,
                    &children[bestIdx]);
}

