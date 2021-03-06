#ifndef LIGHTWEIGHT_REDUCTION_SPARSE_FULL_MISS_H
#define LIGHTWEIGHT_REDUCTION_SPARSE_FULL_MISS_H

#include "LightWeightReductionSparseStaticOrderMISS.h"
#include "CliqueColoringStrategy.h"

#include <vector>
#include <list>

class LightWeightReductionSparseFullMISS : public LightWeightReductionSparseStaticOrderMISS
{
public:
    LightWeightReductionSparseFullMISS(std::vector<std::vector<int>> const &vAdjacencyArray);

    virtual void Color(std::vector<int> const &vVertexOrder, std::vector<int> &vVerticesToReorder, std::vector<int> &vColors);
};

#endif //LIGHTWEIGHT_REDUCTION_SPARSE_FULL_MISS_H
