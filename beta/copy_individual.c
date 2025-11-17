#include <stdlib.h>
#include <string.h>
#include "functions.h"
#include "definition.h"

// 個体を深くコピー
void copy_individual(Individual *dst, const Individual *src)
{
    if (dst == NULL || src == NULL) {
        return;
    }

    dst->chromosomeLength = src->chromosomeLength;
    dst->fitnessScore     = src->fitnessScore;
    dst->coloringCost     = src->coloringCost;
    dst->conflictPairCount = src->conflictPairCount;

    if (src->chromosomeLength <= 0) {
        free(dst->colorChromosome);
        free(dst->conflictFlags);
        dst->colorChromosome = NULL;
        dst->conflictFlags = NULL;
        return;
    }

    int *newChrom = realloc(dst->colorChromosome,
                            sizeof(int) * src->chromosomeLength);
    int *newFlags = realloc(dst->conflictFlags,
                            sizeof(int) * src->chromosomeLength);

    if (!newChrom || !newFlags) {
        free(newChrom);
        free(newFlags);
        exit(EXIT_FAILURE);
    }

    dst->colorChromosome = newChrom;
    dst->conflictFlags   = newFlags;

    for (int i = 0; i < src->chromosomeLength; i++) {
        dst->colorChromosome[i] = src->colorChromosome ?
            src->colorChromosome[i] : 0;
        dst->conflictFlags[i] = src->conflictFlags ?
            src->conflictFlags[i] : 0;
    }
}
