#include <float.h>
#include <stdlib.h>
#include "definition.h"
#include "functions.h"

void update_global_best(Generation *gen)
{
    float bestCost = -FLT_MAX;
    int bestIsland = 0;

    // islandBestIndividual から最大 coloringCost を持つ個体を探す
    for (int i = 0; i < gen->numberOfIslands; i++) {
        if (gen->islands[i].islandBestIndividual.coloringCost > bestCost) {
            bestCost = gen->islands[i].islandBestIndividual.coloringCost;
            bestIsland = i;
        }
    }

    // 保存
    copy_individual(&gen->globalBestIndividual,
                    &gen->islands[bestIsland].islandBestIndividual);
}
