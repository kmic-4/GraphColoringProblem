#include <stdlib.h>
#include "definition.h"
#include "functions.h"

void evaluate_color_variety_fitness(Individual *individual) {
    if (individual == NULL || individual->colorChromosome == NULL) {
        return;
    }

    int length = individual->chromosomeLength;
    int *chrom = individual->colorChromosome;

    // 色種の最大値は頂点数以下であるため、
    // 頂点数（=length）を上限とした簡易フラグ配列を用いる
    int *appeared = (int *)calloc(length + 1, sizeof(int));
    if (appeared == NULL) {
        return;  // メモリ確保失敗時はスキップ
    }

    int varietyCount = 0;

    for (int i = 0; i < length; i++) {
        int color = chrom[i];

        // 初登場の色 → カウント
        if (appeared[color] == 0) {
            appeared[color] = 1;
            varietyCount++;
        }
    }

    free(appeared);

    // 評価スコアとして代入
    individual->fitnessScore = varietyCount;
}
