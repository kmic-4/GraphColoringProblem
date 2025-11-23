#include <stdlib.h>
#include "definition.h"
#include "functions.h"

// 移住関数 (migrate)
// 島モデル（Island Model）における移住（Migration）処理を行う。
// 定期的に島間で個体を交換することで、遺伝的多様性を維持し、局所最適解への早期収束を防ぐ。
// ここでは、各島の「最良個体（Representative）」を他のすべての島へコピーする（ブロードキャスト型）。
// 引数 gen: 現在の世代データ（Generation*）。全島の情報が含まれる。
void migrate(Generation *gen)
{
    // Generation構造体のnumberOfIslandsメンバから島の総数を取得。
    int islandCount = gen->numberOfIslands;

    // 島を1つずつ送り元として処理
    for (int sender = 0; sender < islandCount; sender++) {

        // Generation構造体のislands配列にアクセスし、送り元となる島（Island*）を取得。
        Island *senderIsland = &gen->islands[sender];

        // 1. 送り元が最良個体を選択 (algorithm.md準拠)
        // Island構造体のislandBestIndividualメンバ（その島で最も良い個体）へのポインタを取得。
        Individual *selected[1];
        selected[0] = &senderIsland->islandBestIndividual;
        int migrationCount = 1; // 送る個体数

        // 自分以外の全 Island に送る 
        for (int receiver = 0; receiver < islandCount; receiver++) {

            // ここで sender 自身を除外
            if (receiver == sender) continue;

            // 受け取り側の島（Island*）を取得。
            Island *recvIsland = &gen->islands[receiver];

            // 受け取り (最良個体1つ)
            for (int m = 0; m < migrationCount; m++) {
                // 上書き先のランダムな位置
                int dstIdx = rand() % islandPopulation;
                
                // deep copy で全情報をコピー
                // copy_individual関数を使用し、送り元の最良個体（selected[m]）を受け取り側の個体群（recvIsland->individuals）のランダムな位置に上書きコピーする。
                // これにより、良い遺伝子が他の島へ伝播する。
                copy_individual(&recvIsland->individuals[dstIdx],
                                selected[m]);
            }
        }
    }
}
