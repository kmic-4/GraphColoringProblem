#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "definition.h"
#include "functions.h"

static void free_partial_graph(GraphStructure *graph, int allocatedRows) {
    if (graph == NULL) return;
    if (graph->adjacencyMatrix != NULL) {
        for (int i = 0; i < allocatedRows; i++) {
            free(graph->adjacencyMatrix[i]);
        }
        free(graph->adjacencyMatrix);
    }
    free(graph);
}

GraphStructure *read_graph_csv(FILE *fp)
{
    char *line = NULL;
    size_t lineCap = 0;
    ssize_t lineLen = 0;
    char *token;
    char *savePtr;
    int n = 0;  // 頂点数
    int i, j;

    // コメント行(#...)を読み飛ばし、ヘッダ行を取得
    while ((lineLen = getline(&line, &lineCap, fp)) != -1) {
        if (line[0] == '#') {
            continue;
        } else {
            break;
        }
    }

    if (lineLen == -1) {
        fprintf(stderr, "read_graph_csv: ヘッダ行が見つかりません。\n");
        free(line);
        // ヘッダが存在しない
        return NULL;
    }

    // ヘッダ行から頂点数を数える
    token = strtok_r(line, ",\r\n", &savePtr);  // 先頭空トークン

    n = 0;
    token = strtok_r(NULL, ",\r\n", &savePtr);
    while (token != NULL) {
        n++;
        token = strtok_r(NULL, ",\r\n", &savePtr);
    }

    if (n <= 0) {
        fprintf(stderr, "read_graph_csv: 頂点数が 0 以下です。\n");
        free(line);
        return NULL;
    }

    // GraphStructure を確保
    GraphStructure *graph = (GraphStructure *)malloc(sizeof(GraphStructure));
    if (graph == NULL) {
        fprintf(stderr, "read_graph_csv: GraphStructure の確保に失敗しました。\n");
        free(line);
        return NULL;
    }

    graph->numberOfVertices = n;
    graph->adjacencyMatrix = (int **)malloc(n * sizeof(int *));
    if (graph->adjacencyMatrix == NULL) {
        fprintf(stderr, "read_graph_csv: adjacencyMatrix の確保に失敗しました (n=%d)。\n", n);
        free(line);
        free(graph);
        return NULL;
    }

    for (i = 0; i < n; i++) {
        graph->adjacencyMatrix[i] = (int *)malloc(n * sizeof(int));
        if (graph->adjacencyMatrix[i] == NULL) {
            fprintf(stderr, "read_graph_csv: 行 %d の確保に失敗しました。\n", i);
            free(line);
            free_partial_graph(graph, i);
            return NULL;
        }
    }

    // データ行を読み込み
    for (i = 0; i < n; i++) {

        if ((lineLen = getline(&line, &lineCap, fp)) == -1) {
            // 行不足 → エラー扱いで終了
            fprintf(stderr, "read_graph_csv: 頂点 %d 行目を読み込めません。\n", i);
            free(line);
            free_partial_graph(graph, n);
            return NULL;
        }

        // 行名（countryX）
        token = strtok_r(line, ",\r\n", &savePtr);
        if (token == NULL) {
            // 不正行 → 即終了
            fprintf(stderr, "read_graph_csv: 頂点 %d 行目の行名が見つかりません。\n", i);
            free(line);
            free_partial_graph(graph, n);
            return NULL;
        }

        // 隣接行列の n 個の値
        for (j = 0; j < n; j++) {
            token = strtok_r(NULL, ",\r\n", &savePtr);

            if (token == NULL) {
                // 列が足りない → エラー終了
                fprintf(stderr, "read_graph_csv: 頂点 %d 行目の列 %d が不足しています。\n", i, j);
                free(line);
                free_partial_graph(graph, n);
                return NULL;
            }

            graph->adjacencyMatrix[i][j] = atoi(token);
        }
    }

    free(line);
    return graph;
}
