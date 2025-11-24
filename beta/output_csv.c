#include <stdio.h>
#include "definition.h"

// CSV出力関数 (append_generation_result_to_csv)
// fp: 書き込み先のファイルポインタ（FILE*）。"a" または "w" モードでオープンされていること。
void append_generation_result_to_csv(FILE *fp, int generationIndex, float coloringCost, int iterationsNumber)
{
    if (fp == NULL) {
        return;
    }
    fprintf(fp, "%d,%f,%d\n", generationIndex, coloringCost, iterationsNumber);
}
