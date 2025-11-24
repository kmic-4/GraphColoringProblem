#include <stdlib.h>
#include "definition.h"
#include "functions.h"

// ペナルティ計算関数 (calculate_penalty)
// 個体の染色体（色の割り当て）を評価し、グラフの隣接条件に違反している（衝突している）箇所を特定する。
// 衝突数に応じてペナルティスコアを計算し、同時に個体内の衝突フラグ（conflictFlags）を更新する。
//
// 引数 graph: 対象となるグラフ構造（const GraphStructure*）。隣接行列を持つ。
// 引数 individual: 評価対象の個体（Individual*）。色配列と衝突フラグを持つ。
// 戻り値: 計算されたペナルティ値（int）。衝突がない場合は0。
int calculate_penalty(const GraphStructure *graph, Individual *individual)
{
    iterationsNumber++;
    int illegalPairAmount = 0;
    // const int discountRate = 4; // 元論文からいじった。元は2
    const int discountRate = 2; // algorithm.md準拠に戻す
    // GraphStructure構造体のnumberOfVerticesメンバから頂点数を取得。
    int numVertices = graph->numberOfVertices;

    // Individual構造体のcolorChromosomeメンバ（色配列）へのポインタを取得。
    int *colors = individual->colorChromosome;

    // conflictFlags を初期化
    // Individual構造体のconflictFlagsメンバ（衝突フラグ配列）が未確保の場合はメモリを確保する。
    if (individual->conflictFlags == NULL) {
        individual->conflictFlags = malloc(sizeof(int) * numVertices);
        if (individual->conflictFlags == NULL) {
            return -1;
        }
    }

    for (int i = 0; i < numVertices; i++) {
        // まず全ての頂点の衝突フラグを0（衝突なし）にリセットする。
        individual->conflictFlags[i] = 0;
    }

    // 隣接ペアを走査して衝突をカウント
    for (int i = 0; i < numVertices; i++) {
        for (int j = i + 1; j < numVertices; j++) {

            // GraphStructure構造体のadjacencyMatrixメンバ（隣接行列）を参照し、辺が存在するか確認。
            // かつ、Individual構造体のcolorChromosome（colors）を参照し、色が同じか確認。
            if (graph->adjacencyMatrix[i][j] == 1 &&
                colors[i] == colors[j]) {

                illegalPairAmount++;

                // 衝突している場合、両端点のconflictFlagsを1（衝突あり）に設定する。
                // この情報はCEX（交叉）で利用される。
                individual->conflictFlags[i] = 1;
                individual->conflictFlags[j] = 1;
            }
        }
    }

    if (illegalPairAmount == 0) {
        return 0;
    }

    return illegalPairAmount * discountRate + 1;
}
