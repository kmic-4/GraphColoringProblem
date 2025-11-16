#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "definition.h"

GraphStructure *read_graph_csv(FILE *fp); // CSVファイルからグラフ構造を読み込む関数
void normalize_chromosome_labels(Individual *individual); // 遺伝子配列のラベルを正規化する関数
void evaluate_color_variety_fitness(Individual *individual); // 適合度評価関数
int calculatePenalty(const GraphStructure *graph, const Individual *individual); // ペナルティ計算関数

#endif