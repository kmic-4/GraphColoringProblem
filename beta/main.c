#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "definition.h"
#include "functions.h"

int main(int argc, char *argv[])
{
    if (argc < 6) return printf("引数が不足しています。\n"), 1;

    mutationRate    = atof(argv[1]);
    crossoverRate   = atof(argv[2]);
    popSize           = atoi(argv[3]);
    maxGenerations    = atoi(argv[4]);


    //**データの取得**//
    FILE *fp = fopen(argv[5], "r");
    if (fp == NULL) {
        perror("File open error");
        return 1;
    }
    GraphStructure *graphStructure = read_graph_csv(fp);  
    fclose(fp);
    if (graphStructure == NULL) {
        fprintf(stderr, "Failed to read graph from CSV\n");
        return 1;
    }


   //** **//

    return 0;
}
