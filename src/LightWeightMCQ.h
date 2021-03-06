#ifndef LIGHTWEIGHT_MCQ_H
#define LIGHTWEIGHT_MCQ_H

#include "MaxSubgraphAlgorithm.h"
#include "CliqueColoringStrategy.h"

#include <vector>
#include <list>

class LightWeightMCQ : public MaxSubgraphAlgorithm
{
public:
    LightWeightMCQ(std::vector<std::vector<char>> const &vAdjacencyMatrix);
////    virtual ~LightWeightMCQ();

    virtual void Color(std::vector<int> const &vVertexOrder, std::vector<int> &vVerticesToReorder, std::vector<int> &vColors);

    virtual void InitializeOrder(std::vector<int> &P, std::vector<int> &vVertexOrder, std::vector<int> &vColors);
    virtual void GetNewOrder(std::vector<int> &vNewVertexOrder, std::vector<int> &vVertexOrder, std::vector<int> const &P, int const chosenVertex);
    virtual void ProcessOrderAfterRecursion(std::vector<int> &vVertexOrder, std::vector<int> &P, std::vector<int> &vColors, int const chosenVertex);
    virtual void ProcessOrderBeforeReturn(std::vector<int> &vVertexOrder, std::vector<int> &P, std::vector<int> &vColors) {}

////    void SetInvert(bool const invert);

protected:
    std::vector<std::vector<char>> const &m_AdjacencyMatrix;
    CliqueColoringStrategy coloringStrategy;
////    bool m_bInvert;
};
#endif
