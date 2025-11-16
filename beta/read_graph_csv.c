#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    char line[BUFSIZ];
    char *token;
    int n = 0;  // 頂点数
    int i, j;

    /* ---- コメント行(#...) を読み飛ばし、ヘッダ行を取得 ---- */
    while (fgets(line, BUFSIZ, fp) != NULL) {
        if (line[0] == '#') {
            continue;
        } else {
            break;
        }
    }

    if (feof(fp)) {
        return NULL;    /* ヘッダが存在しない */
    }

    /* ---- ヘッダ行から頂点数を数える ---- */
    token = strtok(line, ",\n");  /* 先頭空トークン */

    n = 0;
    token = strtok(NULL, ",\n");
    while (token != NULL) {
        n++;
        token = strtok(NULL, ",\n");
    }

    if (n <= 0) {
        return NULL;
    }

    /* ---- GraphStructure を確保 ---- */
    GraphStructure *graph = (GraphStructure *)malloc(sizeof(GraphStructure));
    if (graph == NULL) {
        return NULL;
    }

    graph->numberOfVertices = n;
    graph->adjacencyMatrix = (int **)malloc(n * sizeof(int *));
    if (graph->adjacencyMatrix == NULL) {
        free(graph);
        return NULL;
    }

    for (i = 0; i < n; i++) {
        graph->adjacencyMatrix[i] = (int *)malloc(n * sizeof(int));
        if (graph->adjacencyMatrix[i] == NULL) {
            free_partial_graph(graph, i);
            return NULL;
        }
    }

    /* ---- データ行を読み込み ---- */
    for (i = 0; i < n; i++) {

        if (fgets(line, BUFSIZ, fp) == NULL) {
            /* 行不足 → エラー扱いで終了 */
            free_partial_graph(graph, n);
            return NULL;
        }

        /* 行名（countryX） */
        token = strtok(line, ",\n");
        if (token == NULL) {
            /* 不正行 → 即終了 */
            free_partial_graph(graph, n);
            return NULL;
        }

        /* 隣接行列の n 個の値 */
        for (j = 0; j < n; j++) {
            token = strtok(NULL, ",\n");

            if (token == NULL) {
                /* 列が足りない → エラー終了 */
                free_partial_graph(graph, n);
                return NULL;
            }

            graph->adjacencyMatrix[i][j] = atoi(token);
        }
    }

    return graph;
}
