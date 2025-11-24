#include <stdlib.h>
#include "definition.h"
#include "functions.h"

// ペナルティ計算関数
// 個体のを評価し、グラフの隣接条件に違反している箇所を特定する。
// 衝突数に応じてペナルティスコアを計算し、同時に個体内の衝突フラグを更新する。
int calculate_penalty(const GraphStructure *graph, Individual *individual)
{
    iterationsNumber++;
    int illegalPairAmount = 0;
    
    const int discountRate = 2; 
    // GraphStructure構造体のnumberOfVerticesメンバから頂点数を取得。
    int numVertices = graph->numberOfVertices;

    // Individual構造体のcolorChromosomeメンバへのポインタを取得。
    int *colors = individual->colorChromosome;

    // conflictFlags を初期化
    // Individual構造体のconflictFlagsメンバが未確保の場合はメモリを確保する。
    if (individual->conflictFlags == NULL) {
        individual->conflictFlags = malloc(sizeof(int) * numVertices);
        if (individual->conflictFlags == NULL) {
            return -1;
        }
    }

    for (int i = 0; i < numVertices; i++) {
        // 全ての頂点の衝突フラグを0にリセットする。
        individual->conflictFlags[i] = 0;
    }

    // 隣接ペアを走査して衝突をカウント
    for (int i = 0; i < numVertices; i++) {
        for (int j = i + 1; j < numVertices; j++) {

            // GraphStructure構造体のadjacencyMatrixメンバを参照し、辺が存在するか確認。
            // かつ、Individual構造体のcolorChromosomeを参照し、色が同じか確認。
            if (graph->adjacencyMatrix[i][j] == 1 &&
                colors[i] == colors[j]) {

                illegalPairAmount++;

                // 衝突している場合、両端点のconflictFlagsを1に設定する。
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
