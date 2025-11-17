#ifndef DEFINITION_H
#define DEFINITION_H

#define REQUIRED_COLORS_FOR_SUCCESS 4
#define MAX_COLOR_LABEL 100

extern double mutationRate;
extern double crossoverRate;
extern int islandPopulation; // 島ごとの個体数。偶数であること。
extern int numberOfIslands;
extern int maxGenerations;

// グラフ構造
typedef struct {
    int numberOfVertices;
    int **adjacencyMatrix;
} GraphStructure;


// 個体構造
typedef struct {
    int *colorChromosome;
    int fitnessScore;
    float coloringCost;
    int chromosomeLength;
    int *conflictFlags; // 交叉用の衝突確認フラグ
    int conflictPairCount;
} Individual;

// Island（サブポピュレーション）
typedef struct {
    Individual *individuals;          // islandPopulation 個
    Individual islandBestIndividual;  // 最良個体
} Island;

// 世代（Generation）
typedef struct {
    Island *islands;                 // numberOfIslands 個
    int generationIndex;             // 世代番号
    Individual globalBestIndividual; // 全体の最良個体
} Generation;

#endif
