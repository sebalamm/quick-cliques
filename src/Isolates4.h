#ifndef ISOLATES_4_H
#define ISOLATES_4_H

#include "Set.h"
#include "ArraySet.h"
#include "SparseArraySet.h"
#include "Reduction.h"

#include <vector>
#include <map>
#include <set>
#include <utility>
#include <ctime>

////#define TIMERS
////#define SPARSE

struct VertexFold
{
    VertexFold(int const _removed1, int const _removed2, int const _new)
    : removedVertex1(_removed1)
    , removedVertex2(_removed2)
    , newVertex(_new)
    { }

    int removedVertex1;
    int removedVertex2;
    int newVertex;
};

template <typename NeighborSet> class Isolates4
{
public:
    Isolates4(std::vector<std::vector<int>> const &adjacencyArray);
    ~Isolates4();

    void RemoveVertexAndNeighbors(int const vertex, std::vector<int> &vRemoved, std::vector<Reduction> &vReductions);
    void RemoveVertex(int const vertex, std::vector<Reduction> &vReductions);

    void RemoveAllIsolates(int const independentSetSIze, std::vector<int> &vIsolateVertices, std::vector<int> &vOtherRemovedVertices, std::vector<Reduction> &vReductions, bool const bConsiderAllVertices);
    void ReplaceAllRemoved(std::vector<Reduction> const &vReductions);

#if 0
    int NextVertexToRemove(std::vector<int> &vVertices);
    int NextVertexToRemove();
#endif //0

    size_t size() const { return isolates.Size(); }

    ArraySet const& GetIsolates() const { return isolates; }
    ArraySet const& GetInGraph()  const { return inGraph;  }
#ifdef SPARSE
    std::vector<SparseArraySet> const& Neighbors()  const { return neighbors;  }
#else
    std::vector<NeighborSet> const& Neighbors()  const { return neighbors;  }
#endif //SPARSE

////    void RemoveEdges(std::vector<std::pair<int,int>> const &vEdges);

    int GetAlternativeVertex(int const vertex) const;

    void SetConnectedComponent(std::vector<int> const &vVertices);

    size_t GetFoldedVertexCount() const { return foldedVertexCount; }

    void SetAllowVertexFolds(bool const allow) { m_bAllowVertexFolds = allow; }

protected: // methods
    bool RemoveIsolatedClique    (int const vertex, std::vector<int> &vIsolateVertices, std::vector<int> &vOtherRemovedVertices, std::vector<Reduction> &vReductions);
////    bool RemoveIsolatedPath      (int const vertex, std::vector<int> &vIsolateVertices,  std::vector<int> &vOtherRemovedVertices, std::vector<std::pair<int,int>> &vAddedEdges);
    bool RemoveDominatedVertex(int const vertex, std::vector<int> &vIsolateVertices,  std::vector<int> &vOtherRemovedVertices, std::vector<Reduction> &vReductions);
    bool FoldVertex(int const vertex, std::vector<int> &vIsolateVertices,  std::vector<int> &vOtherRemovedVertices, std::vector<Reduction> &vReductions);

protected: // members
    std::vector<std::vector<int>> const &m_AdjacencyArray;
#ifdef SPARSE
    std::vector<SparseArraySet>     neighbors;
#else
    std::vector<NeighborSet>     neighbors;
#endif // SPARSE
    ArraySet inGraph;
    ArraySet isolates;
    ArraySet remaining;
    std::vector<bool> vMarkedVertices;
    std::map<int,int> m_AlternativeVertices;
#ifdef TIMERS
    clock_t timer;
    clock_t removeTimer;
    clock_t replaceTimer;
    clock_t sortDuringNextTimer;
    clock_t removeOneDuringNextTimer;
    clock_t removeDuringNextTimer;
    clock_t replaceDuringNextTimer;
    #endif // TIMERS
    bool m_bConnectedComponentMode;
    size_t foldedVertexCount;
    bool m_bAllowVertexFolds;
};

#endif //ISOLATES_4_H
