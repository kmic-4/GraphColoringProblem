#include <stdlib.h>
#include "definition.h"
#include "functions.h"

// 移住関数 
void migrate(Generation *generation)
{
    // Generation構造体のnumberOfIslandsメンバから島の総数を取得。
    int islandCount = generation->numberOfIslands;

    // 移住処理前に全島の最良個体を一時保存
    Individual *bestIndividuals = (Individual *)calloc(islandCount, sizeof(Individual));
    if (bestIndividuals == NULL) {
        // メモリ確保失敗時は移住処理をスキップ
        return;
    }

    // 全島の最良個体を一時配列にコピー
    for (int i = 0; i < islandCount; i++) {
        copy_individual(&bestIndividuals[i], &generation->islands[i].islandBestIndividual);
    }

    // 各受信島ごとに、重複しない位置リストを事前準備
    int **receiverPositions = (int **)calloc(islandCount, sizeof(int *));
    if (receiverPositions == NULL) {
        free(bestIndividuals);
        return;
    }

    for (int i = 0; i < islandCount; i++) {
        receiverPositions[i] = (int *)malloc(sizeof(int) * islandPopulation);
        if (receiverPositions[i] == NULL) {
            // メモリ確保失敗時は解放して終了
            for (int j = 0; j < i; j++) {
                free(receiverPositions[j]);
            }
            free(receiverPositions);
            free(bestIndividuals);
            return;
        }

        // 位置リストを0からislandPopulation-1で初期化
        for (int j = 0; j < islandPopulation; j++) {
            receiverPositions[i][j] = j;
        }

        // 移住個体が毎回被らないようにシャッフルして位置をランダム化
        
        for (int j = islandPopulation - 1; j > 0; j--) {
            int k = rand() % (j + 1);
            int temp = receiverPositions[i][j];
            receiverPositions[i][j] = receiverPositions[i][k];
            receiverPositions[i][k] = temp;
        }
    }

    // 各受信島が次に使用する位置のインデックスを追跡
    int *nextPositionIndex = (int *)calloc(islandCount, sizeof(int));
    if (nextPositionIndex == NULL) {
        for (int i = 0; i < islandCount; i++) {
            free(receiverPositions[i]);
        }
        free(receiverPositions);
        free(bestIndividuals);
        return;
    }

    // 一時配列から各島へ配布
    for (int sender = 0; sender < islandCount; sender++) {

        // 送り元の最良個体
        Individual *senderBest = &bestIndividuals[sender];

        // 自分以外の全Islandに送る 
        for (int receiver = 0; receiver < islandCount; receiver++) {

            // ここ sender自身を除外
            if (receiver == sender) continue;

            // 受け取り側の島を取得。
            Island *recvIsland = &generation->islands[receiver];

            // 重複しない位置を取得(すでにランダムに)
            int dstIdx = receiverPositions[receiver][nextPositionIndex[receiver]];
            nextPositionIndex[receiver]++;
            
            // 全情報をコピー
            copy_individual(&recvIsland->individuals[dstIdx], senderBest);
        }
    }

    // 一時配列のメモリを解放
    for (int i = 0; i < islandCount; i++) {
        free(receiverPositions[i]);
    }
    free(receiverPositions);
    free(nextPositionIndex);

    // 一時配列のメモリを解放
    free(bestIndividuals);
}
