#include <stdlib.h>
#include "definition.h"
#include "functions.h"

// 移住関数 (migrate)
// 島モデル（Island Model）における移住（Migration）処理を行う。
// 定期的に島間で個体を交換することで、遺伝的多様性を維持し、局所最適解への早期収束を防ぐ。
// ここでは、各島の「最良個体（Representative）」を他のすべての島へコピーする（ブロードキャスト型）。
// 引数 gen: 現在の世代データ（Generation*）。全島の情報が含まれる。
void migrate(Generation *generation)
{
    // Generation構造体のnumberOfIslandsメンバから島の総数を取得。
    int islandCount = generation->numberOfIslands;

    // 移住処理前に全島の最良個体を一時保存
    // 理由: forループ内で他島の最良個体が上書きされる前に、全島の「移住前の」最良個体を保存する必要がある。
    // そうしないと、island[0]の最良個体がisland[1]に送られた際にisland[1]の最良個体位置を上書きし、
    // その後island[1]が送り元になった時に「既に上書きされた個体」を他島に送ってしまう可能性がある。
    Individual *bestIndividuals = (Individual *)calloc(islandCount, sizeof(Individual));
    if (bestIndividuals == NULL) {
        // メモリ確保失敗時は移住処理をスキップ
        return;
    }

    // 1. 全島の最良個体を一時配列にコピー
    for (int i = 0; i < islandCount; i++) {
        copy_individual(&bestIndividuals[i], &generation->islands[i].islandBestIndividual);
    }

    // 2. 各受信島ごとに、重複しない位置リストを事前準備
    // 各島は islandCount-1 個の移住個体を受け取るため、その分のユニークな位置を確保
    int **receiverPositions = (int **)calloc(islandCount, sizeof(int *));
    if (receiverPositions == NULL) {
        free(bestIndividuals);
        return;
    }

    for (int i = 0; i < islandCount; i++) {
        receiverPositions[i] = (int *)malloc(sizeof(int) * islandPopulation);
        if (receiverPositions[i] == NULL) {
            // メモリ確保失敗時のクリーンアップ
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

        // シャッフルして位置をランダム化
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

    // 3. 一時配列から各島へ配布（重複なし）
    // 島を1つずつ送り元として処理
    for (int sender = 0; sender < islandCount; sender++) {

        // 送り元の最良個体（一時保存されたもの）
        Individual *senderBest = &bestIndividuals[sender];

        // 自分以外の全 Island に送る 
        for (int receiver = 0; receiver < islandCount; receiver++) {

            // ここで sender 自身を除外
            if (receiver == sender) continue;

            // 受け取り側の島（Island*）を取得。
            Island *recvIsland = &generation->islands[receiver];

            // 重複しない位置を取得
            // receiverPositionsは事前にシャッフルされた位置リスト
            // nextPositionIndexで次に使う位置を追跡
            int dstIdx = receiverPositions[receiver][nextPositionIndex[receiver]];
            nextPositionIndex[receiver]++;
            
            // deep copy で全情報をコピー
            // copy_individual関数を使用し、送り元の最良個体（一時保存されたもの）を
            // 受け取り側の個体群（recvIsland->individuals）のランダムな位置に上書きコピーする。
            // これにより、良い遺伝子が他の島へ伝播する。
            copy_individual(&recvIsland->individuals[dstIdx], senderBest);
        }
    }

    // 4. 一時配列のメモリを解放
    for (int i = 0; i < islandCount; i++) {
        free(receiverPositions[i]);
    }
    free(receiverPositions);
    free(nextPositionIndex);

    // 3. 一時配列のメモリを解放
    free(bestIndividuals);
}
