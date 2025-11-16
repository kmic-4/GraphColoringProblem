#include <stdio.h>
#include <stdlib.h>

void normalize_chromosome_labels(Individual *individual) {
    if (individual == NULL || individual->colorChromosome == NULL) {
        return;
    }

    int length = individual->chromosomeLength;
    int *chrom = individual->colorChromosome;

    // ラベルマッピング表（元ラベル → 新ラベル）
    // 頂点数が色ラベルの上限になるので十分大きい配列を確保可能
    int *labelMap = (int *)calloc(length + 1, sizeof(int));
    if (labelMap == NULL) {
        return;  // メモリ確保失敗
    }

    int nextLabel = 1;

    for (int i = 0; i < length; i++) {
        int current = chrom[i];

        // 初登場のラベルなら新番号を割り当て
        if (labelMap[current] == 0) {
            labelMap[current] = nextLabel;
            nextLabel++;
        }

        // 再マッピング
        chrom[i] = labelMap[current];
    }

    free(labelMap);
}
