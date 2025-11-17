#include <stdlib.h>
#include <string.h>
#include "functions.h"
#include "definition.h"

/* --------- 深いコピー（必須修正） --------- */
void copy_individual(Individual *dst, const Individual *src)
{
    dst->chromosomeLength = src->chromosomeLength;
    dst->fitnessScore     = src->fitnessScore;
    dst->coloringCost     = src->coloringCost;

    /* colorChromosome の deep copy */
    dst->colorChromosome =
        (int *)malloc(sizeof(int) * src->chromosomeLength);

    for (int i = 0; i < src->chromosomeLength; i++) {
        dst->colorChromosome[i] = src->colorChromosome[i];
    }

    /* conflictFlags の deep copy */
    dst->conflictFlags =
        (int *)malloc(sizeof(int) * src->chromosomeLength);

    for (int i = 0; i < src->chromosomeLength; i++) {
        dst->conflictFlags[i] = src->conflictFlags[i];
    }
}
