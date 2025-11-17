#include <float.h>
#include <stdlib.h>
#include "definition.h"
#include "functions.h"

void update_global_best(Generation *gen)
{
    float bestCost = FLT_MAX;
    int bestIsland = 0;

    /* islandBestIndividual の中から最小 coloringCost を探す */
    for (int i = 0; i < gen->numberOfIslands; i++) {
        if (gen->islands[i].islandBestIndividual.coloringCost < bestCost) {
            bestCost = gen->islands[i].islandBestIndividual.coloringCost;
            bestIsland = i;
        }
    }

    /* deep copy */
    copy_individual(&gen->globalBestIndividual,
                    &gen->islands[bestIsland].islandBestIndividual);
}
