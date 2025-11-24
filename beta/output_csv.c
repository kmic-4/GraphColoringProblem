#include <stdio.h>
#include "definition.h"

// CSV出力関数 (append_generation_result_to_csv)
// 世代ごとの統計情報（世代番号、適応度、コスト）をCSVファイルに追記する。
// ファイルポインタは呼び出し元で管理し、オープン済みのポインタを受け取る。
//
// fp: 書き込み先のファイルポインタ（FILE*）。"a" または "w" モードでオープンされていること。
// generationIndex: 現在の世代番号（int）。
// coloringCost: 最良個体のコスト値（float）。
// iterationsNumber: 探索回数（int）。
void append_generation_result_to_csv(FILE *fp, int generationIndex, float coloringCost, int iterationsNumber)
{
    if (fp == NULL) {
        return;
    }
    fprintf(fp, "%d,%f,%d\n", generationIndex, coloringCost, iterationsNumber);
}
