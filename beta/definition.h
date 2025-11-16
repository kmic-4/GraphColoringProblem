#ifndef DEFINITION_H
#define DEFINITION_H

#define REQUIRED_COLORS_FOR_SUCCESS 4     // 4色問題であります
#define MAX_COLOR_LABEL 100             
extern double mutationRate;
extern double crossoverRate;
extern int populationSize;
extern int maxGenerations;


// グラフ構造を表す
typedef struct {
    int numberOfVertices;     // グラフが持つ頂点数
    int **adjacencyMatrix;    // 隣接行列
} GraphStructure;


// int colorChromosome[]; // グラフの状態を持つ遺伝子配列。
// int fitnessScore; // 適合度スコア

// コンフリクトを起こしている頂点ペア
typedef struct {
    int vertexA;
    int vertexB;
} ConflictPair;

typedef struct {
    int *colorChromosome;
    int fitnessScore;
    int chromosomeLength;
    ConflictPair **conflictPairs;  // ConflictPair* の配列（可変長）
    int conflictPairCount;
} Individual;

typedef struct { 
    Individual *individuals;  
    int populationSize; 
    int generationIndex;  
    Individual bestIndividual;  
} Generation;

#endif
