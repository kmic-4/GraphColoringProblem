#include <stdlib.h>
#include "definition.h"
#include "functions.h"

int calculate_penalty(const GraphStructure *graph, Individual *individual)
{
    int illegalPairAmount = 0;
    const int discountRate = 2;
    int numVertices = graph->numberOfVertices;

    int *colors = individual->colorChromosome;

    /* conflictFlags の初期化 */
    if (individual->conflictFlags == NULL) {
        individual->conflictFlags = malloc(sizeof(int) * numVertices);
        if (individual->conflictFlags == NULL) {
            return -1;
        }
    }

    for (int i = 0; i < numVertices; i++) {
        individual->conflictFlags[i] = 0;
    }

    /* 隣接ペアを走査して衝突をカウント */
    for (int i = 0; i < numVertices; i++) {
        for (int j = i + 1; j < numVertices; j++) {

            if (graph->adjacencyMatrix[i][j] == 1 &&
                colors[i] == colors[j]) {

                illegalPairAmount++;

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
