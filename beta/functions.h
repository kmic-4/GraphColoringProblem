#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdio.h>
#include "definition.h"

GraphStructure *read_graph_csv(FILE *fp); // CSVファイルからグラフ構造を読み込む関数
void normalize_chromosome_labels(Individual *individual); // 遺伝子配列のラベルを正規化する関数
void evaluate_color_variety_fitness(Individual *individual); // 適合度評価関数
int calculate_penalty(const GraphStructure *graph, Individual *individual); // ペナルティ計算関数
void copy_individual(Individual *dst, const Individual *src);
void generate_first_generation(Generation *gen,
                             const GraphStructure *graph,
                             int numberOfColor); // 第一世代の生成関数
void generate_next_generation_island(Island *nextIsland,
                                     const Island *currentIsland,
                                     const GraphStructure *graph,
                                     unsigned int *seed); // 次世代の生成関数
void migrate(Generation *gen);// 島間移民関数
void update_global_best(Generation *gen); // グローバルベスト更新関数
void run_generations(Generation *currentGen,
                    Generation *nextGen,
                    GraphStructure *graph);
void append_generation_result_to_csv(FILE *fp, int generationIndex, float coloringCost, int iterationsNumber); // CSV出力関数

#endif
