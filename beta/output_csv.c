#include <stdio.h>
#include "definition.h"

//~ CSV出力関数 (append_generation_result_to_csv)
//~ 世代ごとの統計情報（世代番号、適応度、コスト）をCSVファイルに追記する。
//~ ファイルポインタは呼び出し元で管理し、オープン済みのポインタを受け取る。
//~
//~ 引数 fp: 書き込み先のファイルポインタ（FILE*）。"a" または "w" モードでオープンされていること。
//~ 引数 generationIndex: 現在の世代番号（int）。
//~ 引数 fitnessScore: 最良個体の適応度（int）。使用色数に相当。
//~ 引数 coloringCost: 最良個体のコスト値（float）。
void append_generation_result_to_csv(FILE *fp, int generationIndex, int fitnessScore, float coloringCost)
{
    if (fp == NULL) {
        return;
    }
    fprintf(fp, "%d,%d,%f\n", generationIndex, fitnessScore, coloringCost);
}
