#include <stdlib.h>
#include "definition.h"
#include "functions.h"

#include <stdlib.h>
#include "definition.h"
#include "functions.h"

int calculatePenalty(const GraphStructure *graph, Individual *individual)
{
    int illegalPairAmount = 0;
    int discountRate = 2; //　論文では、q(u,v)はcolor(u)=color(v)の場合に2を返すとされている
    int numVertices = graph->numberOfVertices;

    int *colors = individual->colorChromosome;

    /* 既存の衝突情報をクリア */
    if (individual->conflictPairs != NULL) {
        free(individual->conflictPairs);
    }
    individual->conflictPairs = NULL;
    individual->conflictPairCount = 0;

    /* 頂点ペアの走査 */
    for (int i = 0; i < numVertices; i++) {
        for (int j = i + 1; j < numVertices; j++) {

            /* 隣接かつ同色なら違法 */
            if (graph->adjacencyMatrix[i][j] == 1 &&
                colors[i] == colors[j]) {

                illegalPairAmount++;

                /* conflictPairs を 1 つ増やす */
                int newCount = individual->conflictPairCount + 1;

                ConflictPair *newArray = realloc(
                    individual->conflictPairs,
                    sizeof(ConflictPair) * newCount
                );

                if (newArray == NULL) {
                    /* realloc 失敗 → 元のポインタは有効だが追加不可 */
                    /* 適切なエラー処理（最低限 -1 を返して呼び出し元で処理） */
                    return -1;
                }

                /* ポインタ更新 */
                individual->conflictPairs = newArray;

                /* 末尾に追加 */
                individual->conflictPairs[individual->conflictPairCount].vertexA = i;
                individual->conflictPairs[individual->conflictPairCount].vertexB = j;

                /* 件数更新 */
                individual->conflictPairCount++;
            }
        }
    }

    /* ペナルティ値算出 */
    if (illegalPairAmount == 0) {
        return 0; // 違法ペアがなければペナルティ0
    }
    int penalty = illegalPairAmount * discountRate + 1; // 元論文で、illegalPairAmount>0であれば加算されているdが1になっている

    return penalty;
}

