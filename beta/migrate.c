#include <stdlib.h>
#include "definition.h"
#include "functions.h"

void migrate(Generation *gen)
{
    int islandCount = gen->numberOfIslands;

    // 島を1つずつ送り元として処理
    for (int sender = 0; sender < islandCount; sender++) {

        Island *senderIsland = &gen->islands[sender];

        //1. 送り元が 5 個体をランダム選択
        Individual *selected[10];

        for (int m = 0; m < 5; m++) {
            int idx = rand() % islandPopulation;
            selected[m] = &senderIsland->individuals[idx];
        }

        // 自分以外の全 Island に送る 
        for (int receiver = 0; receiver < islandCount; receiver++) {

            // ここで sender 自身を除外
            if (receiver == sender) continue;

            Island *recvIsland = &gen->islands[receiver];

            // 受け取り 5 個体分
            for (int m = 0; m < 5; m++) {

                // 上書き先のランダムな位置（0〜pop-1）
                int dstIdx = rand() % islandPopulation;

                // deep copy で全情報をコピー
                copy_individual(&recvIsland->individuals[dstIdx],
                                selected[m]);
            }
        }
    }
}
