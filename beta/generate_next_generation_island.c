#include <stdlib.h>
#include <float.h>

#include "definition.h"
#include "functions.h"


// 以前使用していたルーレット選択
// ルーレット選択 (Roulette Selection)
// 適応度（coloringCost）に比例した確率で個体を選択する手法。
// 1. 全個体の適応度の総和（sum）を計算する。
// 2. 0 から sum までの乱数 r を生成する。
// 3. 個体を順に走査し、適応度の累積値（acc）が r を超えた時点で、その個体を選択する。
// 引数 inds: 選択対象の個体群（Individual型の配列）
// 引数 population: 個体群のサイズ（islandPopulation）
// 戻り値: 選択された個体へのポインタ（Individual*）
static Individual* roulette_select(Individual *inds, int population)
{
    double sum = 0.0;

    for (int i = 0; i < population; i++) {
        // Individual構造体のcoloringCostメンバを使用。これが適応度に相当する。
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
        // 再度Individual構造体のcoloringCostにアクセスして累積計算を行う。
        double weight = inds[i].coloringCost;
        if (weight < 0.0) weight = 0.0;
        acc += weight;
        if (acc >= r) {
            return &inds[i];
        }
    }

    return &inds[population - 1];
}

#define TOURNAMENT_SIZE 3

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

// 複数交差点で交叉する数
// #define MULTI_CROSSOVER_POINTS 2

// 次世代を生成する（トーナメント選択 + CEX + mutation）
// Modified to use CEX (Conflict Elimination Crossover) based on user request
// 次世代生成関数 (generate_next_generation_island)
// 1つの島（Island）における次世代の個体群を生成するメイン関数。
// 手順:
// 1. 選択 (Selection): ルーレット選択により親個体を選出。
// 2. 交叉 (Crossover): CEX (Conflict Elimination Crossover) により子個体を生成。
// 3. 突然変異 (Mutation): 確率的に遺伝子（色）を置換。
// 4. 評価 (Evaluation): 生成された子個体の適応度を計算し、島内最良個体を更新。
//
// 引数 nextIsland: 次世代の島データ（Island*）。ここに生成された個体が格納される。
// 引数 curIsland: 現世代の島データ（const Island*）。ここから親が選ばれる。
// 引数 graph: グラフ構造データ（const GraphStructure*）。ペナルティ計算に使用される。
void generate_next_generation_island(Island *nextIsland,
                                     const Island *curIsland,
                                     const GraphStructure *graph)
{
    // Island構造体のindividualsメンバにアクセス。個体群配列の先頭ポインタを取得。
    Individual *parents  = curIsland->individuals;
    Individual *children = nextIsland->individuals;

    float bestCost = -FLT_MAX;
    int   bestIdx  = 0;

    // 1. 各 Island 内でトーナメント選択しペアを作成 → 交叉 (CEX)
    for (int k = 0; k < islandPopulation; k += 2) {

        // 親をトーナメント選択
        // Individual *p1 = tournament_select(parents, islandPopulation);
        // Individual *p2 = tournament_select(parents, islandPopulation);

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
            
            // CEX (Conflict Elimination Crossover) Implementation
            // CEXの実装部分。
            // 親の「良い形質（衝突のない部分）」を優先的に子に引き継ぐことで、解の改善を促進する。
            // 1. 初期化: ベースとなる親をコピー (c1 = p2, c2 = p1)
            // Individual構造体のcolorChromosomeメンバ（色配列）にアクセスし、ベースとなる親の遺伝子をコピーする。
            for (int i = 0; i < len; i++) {
                c1->colorChromosome[i] = p2->colorChromosome[i];
                c2->colorChromosome[i] = p1->colorChromosome[i];
            }

            // 2. 子 c1 の生成:
            // 親 p1 の中で衝突していない頂点 (V_cf^p1) を c1 にコピー
            // conflictFlags[i] == 0 means conflict-free
            // Individual構造体のconflictFlagsメンバを参照。これが0であれば、その頂点は衝突していない（安全な色）。
            for (int i = 0; i < len; i++) {
                if (p1->conflictFlags && p1->conflictFlags[i] == 0) {
                    c1->colorChromosome[i] = p1->colorChromosome[i];
                }
            }

            // 3. 子 c2 の生成:
            // 親 p2 の中で衝突していない頂点 (V_cf^p2) を c2 にコピー
            // 同様に、親 p2 の conflictFlags を確認し、衝突のない遺伝子を c2 に上書きコピーする。
            for (int i = 0; i < len; i++) {
                if (p2->conflictFlags && p2->conflictFlags[i] == 0) {
                    c2->colorChromosome[i] = p2->colorChromosome[i];
                }
            }

        } else {
            // 交叉なし → 親をそのままコピー
            for (int v = 0; v < len; v++) {
                c1->colorChromosome[v] = p1->colorChromosome[v];
                c2->colorChromosome[v] = p2->colorChromosome[v];
            }
        }

        // 交叉後に conflictFlags を更新
        // calculate_penalty関数内で、GraphStructureとIndividualを使用して衝突判定を行い、conflictFlagsを更新する。
        calculate_penalty(graph, c1);
        calculate_penalty(graph, c2);
    }


    // 2. mutation（突然変異：swap）
    for (int j = 0; j < islandPopulation; j++) {

        Individual *ind = &children[j];
        int len = ind->chromosomeLength;

        if ((double)rand() / RAND_MAX < mutationRate) {

            // 異なる2つの頂点を選択（algorithm.md準拠）
            int a = rand() % len;
            int b;
            
            // aと異なるbを選ぶ（len >= 2を前提）
            do {
                b = rand() % len;
            } while (b == a && len > 1);

            // 2頂点の色を入れ替え（転置: Transposition）
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
    // Island構造体のislandBestIndividualメンバに、今回生成された中で最も良かった個体（children[bestIdx]）をコピーして保存する。
    copy_individual(&nextIsland->islandBestIndividual,
                    &children[bestIdx]);
}

