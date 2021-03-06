#include "Isolates3.h"
#include "ArraySet.h"
#include "SparseArraySet.h"

#include <vector>
#include <set>
#include <iostream>
#include <ctime>
#include <cassert>

#define NO_INVARIANT // we don't maintain that the graph always has all isolated vertices removed.

using namespace std;

template <typename NeighborSet>
Isolates3<NeighborSet>::Isolates3(vector<vector<int>> const &adjacencyArray)
 : m_ReorderedAdjacencyArray(adjacencyArray)
 , neighbors(adjacencyArray.size())
 , inGraph(adjacencyArray.size())
 , isolates(adjacencyArray.size())
 , remaining(adjacencyArray.size())
 , vMarkedVertices(adjacencyArray.size(), false)
 , vRecentlyRemovedVertices(adjacencyArray.size(), false)
 , m_AlternativeVertices()
#ifdef TIMERS
 , timer(0)
 , removeTimer(0)
 , replaceTimer(0)
 , sortDuringNextTimer(0)
 , removeOneDuringNextTimer(0)
 , removeDuringNextTimer(0)
 , replaceDuringNextTimer(0)
 #endif // TIMERS
 , m_bConnectedComponentMode(false)
{
    for (size_t u=0; u < adjacencyArray.size(); ++u) {
        remaining.Insert(u);
        inGraph.Insert(u);
#ifdef SPARSE
        neighbors[u].Resize(m_ReorderedAdjacencyArray[u].size());
#else
        neighbors[u].InitializeFromAdjacencyArray(m_ReorderedAdjacencyArray, u);
#endif // SPARSE
        for (int const vertex : m_ReorderedAdjacencyArray[u]) {
            neighbors[u].Insert(vertex);
        }
    }
}

template <typename NeighborSet>
Isolates3<NeighborSet>::~Isolates3()
{

#ifdef TIMERS
    cout << "Total time spent computing next vertex: " << (timer/(double)CLOCKS_PER_SEC) << endl;
    cout << "    Sort      : " << (sortDuringNextTimer/(double)CLOCKS_PER_SEC) << endl;
    cout << "    RemoveOne : " << (removeOneDuringNextTimer/(double)CLOCKS_PER_SEC) << endl;
    cout << "    RemoveRest: " << (removeDuringNextTimer/(double)CLOCKS_PER_SEC) << endl;
    cout << "    ReplaceAll: " << (replaceDuringNextTimer/(double)CLOCKS_PER_SEC) << endl;
   
    cout << "Total time spent applying reductions  : " << (removeTimer/(double)CLOCKS_PER_SEC) << endl;
    cout << "Total time spent undoing  reductions  : " << (replaceTimer/(double)CLOCKS_PER_SEC) << endl;
#endif // TIMERS
}

////void Isolates3::RemoveEdges(vector<pair<int,int>> const &vEdges)
////{
////    for (pair<int,int> const &edge : vEdges) {
////        neighbors[edge.first].Remove(edge.second);
////        neighbors[edge.second].Remove(edge.first);
////
////        m_AdjacencyArray[edge.first].pop_back();
////        m_AdjacencyArray[edge.second].pop_back();
////    }
////}

template <typename NeighborSet>
bool Isolates3<NeighborSet>::RemoveIsolatedClique(int const vertex, vector<int> &vIsolateVertices, vector<int> &vOtherRemovedVertices)
{
////    bool const debug(vertex==40653);
////    if (vertex == 31) {
////        cout << "Removing isolated clique with vertex " << vertex << endl << flush;
////        if (!inGraph.Contains(31)) cout << "Vertex 31 is not in the graph!" << endl << flush;
////    }

    size_t neighborCount(0);
    for (int const neighbor : neighbors[vertex]) {
        if (neighbors[neighbor].Size() < neighbors[vertex].Size()) {
            return false;
        }
    }

    bool superSet(true);

////    bool debug(vertex == 113);
////
////    if (debug) {
////        cout << "vertex" << vertex << " has " << neighbors[vertex].size() << " neighbors." << endl << flush;
////    }

#if 1
////    bool const debug(true); //vertex==12);
////    if (debug) {
////        cout << "vertex " << vertex << " has neighbors: ";
////    }
////    if (debug) {
////        for (int const neighbor : neighbors[vertex]) {
////            cout << neighbor << " " << flush;
////        }
////        cout << endl << flush;
////        cout << "(";
////
////        for (int const neighbor : m_AdjacencyArray[vertex]) {
////            cout << neighbor << " " << flush;
////        }
////        cout << ")" << endl << flush;
////    }
////
    for (int const neighbor : neighbors[vertex]) {
////        if (debug) {
////            cout << "Considering neighbor " <<  neighbor  << flush;
////        }
        neighborCount++;

        for (int const nNeighbor : neighbors[neighbor]) {
            vMarkedVertices[nNeighbor] = true;
        }
        vMarkedVertices[neighbor] = true;

        for (int const neighbor2 : neighbors[vertex]) {
////            if (debug && superSet && !vMarkedVertices[neighbor2]) {
////                cout << "(missing neighbor " << neighbor2 << ") ";
////            }
            superSet = superSet && vMarkedVertices[neighbor2];
        }

        //superSet = superSet && (neighborCount == neighbors[vertex].size()); // TODO/DS : put back?

        for (int const nNeighbor : neighbors[neighbor]) {
            vMarkedVertices[nNeighbor] = false;
        }
        vMarkedVertices[neighbor] = false;

        if (!superSet) {
////            if (debug) {
////                cout << "[x] " << endl;
////            }
            return false;
        }

////        if (debug) cout << endl << flush;

        ////            cout << "Iteration(" << neighborCount << ": Intersection=";
        ////            for (int const vertex : intersection) {
        ////                cout << vertex << " ";
        ////            }
        ////            cout << endl << flush;
    }
#else

    for (int const neighbor : neighbors[vertex]) {
////        cout << "Evaluating neighbor: " << neighbor << endl << flush;
        neighborCount++;

        for (int const nNeighbor : neighbors[neighbor]) {
////            cout << "Marking nNeighbor: " << nNeighbor << endl << flush;
            vMarkedVertices[nNeighbor] = true;
        }
////        cout << "Marking neighbor: " << neighbor << endl << flush;
        vMarkedVertices[neighbor] = true;

        for (int const neighbor2 : neighbors[vertex]) {
////            cout << "Checking neighbor: " << neighbor2 << endl << flush;
            superSet = superSet && vMarkedVertices[neighbor2];
        }

        for (int const nNeighbor : neighbors[neighbor]) {
////            cout << "Unmarking nNeighbor: " << nNeighbor << endl << flush;
            vMarkedVertices[nNeighbor] = false;
        }
////        cout << "Unmarking neighbor: " << neighbor << endl << flush;
        vMarkedVertices[neighbor] = false;

        if (!superSet) return false;

        ////            cout << "Iteration(" << neighborCount << ": Intersection=";
        ////            for (int const vertex : intersection) {
        ////                cout << vertex << " ";
        ////            }
        ////            cout << endl << flush;
    }
#endif

////    cout << "Done evaluating neighbors" << endl << flush;

    ////        cout << "Iteration(" << neighborCount << ": Intersection=";
    ////        for (int const vertex : intersection) {
    ////            cout << vertex << " ";
    ////        }
    ////        cout << endl << flush;

    if (superSet) {
////        if (debug) {
////            cout << "Removing Clique: [" << vertex;
////            for (int const neighbor : neighbors[vertex]) {
////                cout << " " << neighbor;
////            }
////            cout << "]" << endl;
////        }
////        for (int const neighbor : neighbors[vertex]) {
////            cout << neighbor << " ";
////        }
////        cout << endl;

        for (int const neighbor : neighbors[vertex]) {
////            if (!inGraph.Contains(neighbor)) {
////                cout << "Trying to remove non-existant neighbor " << neighbor << " from " << vertex << " neighbor list!" << endl << flush;
////            }
////            cout << __LINE__ << ": calling remove" << endl << flush;
            inGraph.Remove(neighbor);
            remaining.Remove(neighbor);
            vRecentlyRemovedVertices[neighbor] = true;
        }

////        if (!inGraph.Contains(vertex)) {
////            cout << "Trying to remove non-existant vertex " << vertex << " from graph!" << endl << flush;
////        }

////        cout << __LINE__ << ": calling remove" << endl << flush;
        vRecentlyRemovedVertices[vertex] = true;
        inGraph.Remove(vertex);
        isolates.Insert(vertex);
        vIsolateVertices.push_back(vertex);
////        if (vertex == 0) {
////            cout << __LINE__ << ": Removing " << vertex << " from graph." << endl << flush;
////        }
        vOtherRemovedVertices.insert(vOtherRemovedVertices.end(), neighbors[vertex].begin(), neighbors[vertex].end());

        for (int const neighbor : neighbors[vertex]) {
            ////                cout << "   Expunging neighbor " << neighbor << " with " << neighbors[neighbor].size() << " neighbors" << endl << flush;
            for (int const nNeighbor : neighbors[neighbor]) {
                if (inGraph.Contains(nNeighbor)) {
                    remaining.Insert(nNeighbor);
                }

                if (nNeighbor != vertex) {
                    SwapToEnd(m_ReorderedAdjacencyArray[nNeighbor], neighbor);
                    neighbors[nNeighbor].Remove(neighbor);
                }
            }
            neighbors[neighbor].Clear();
        }

        vRecentlyRemovedVertices[vertex] = false;
        for (int const neighbor : neighbors[vertex]) {
            vRecentlyRemovedVertices[neighbor] = false;
        }

        neighbors[vertex].Clear();

////    if (vertex == 21952) {
////        cout << "vertex" << vertex << " is being removed." << endl << flush;
////    }
        
        return true;
    }
    return false;
}

#if 0
// TODO/DS: need to remember added edge, so we can remove it later.
// TODO/DS: not currently working, proceeding without it.
bool Isolates3::RemoveIsolatedPath(int const vertex, vector<int> &vIsolateVertices, vector<int> &vOtherRemovedVertices, vector<pair<int,int>> &vAddedEdges)
{
    if (neighbors[vertex].Size() != 2) return false;

    for (int const neighbor : neighbors[vertex]) {
        if (neighbors[neighbor].Size() == 2) {
            isolates.Insert(vertex);

            int endVertex1(-1);
            int endVertex2(-1);

            // remove from other neighbor's list...
            for (int const nNeighbor : neighbors[neighbor]) {
                if (nNeighbor != vertex) {
                    endVertex1 = nNeighbor;
                    neighbors[nNeighbor].Remove(neighbor);
////                    cout << __LINE__ << ": Removing edge " << nNeighbor << "," << neighbor << endl;
                    remaining.Insert(nNeighbor);
                    break;
                }
            }

            for (int const otherNeighbor : neighbors[vertex]) {
                if (otherNeighbor != neighbor) {
                    endVertex2 = otherNeighbor;
                    neighbors[otherNeighbor].Remove(vertex);
////                    cout << __LINE__ << ": Removing edge " << otherNeighbor << "," << vertex << endl;
                    remaining.Insert(otherNeighbor);

                    if (!neighbors[otherNeighbor].Contains(endVertex1)) {

                        m_AdjacencyArray[otherNeighbor].push_back(endVertex1);
                        m_AdjacencyArray[endVertex1].push_back(otherNeighbor);

                        neighbors[otherNeighbor].Insert(endVertex1);
                        neighbors[endVertex1].Insert(otherNeighbor);
                        vAddedEdges.push_back(make_pair(endVertex1, otherNeighbor));
                        break;
                    }
                }
            }

            neighbors[vertex].Clear();
            neighbors[neighbor].Clear();

            vIsolateVertices.push_back(vertex);
////        if (vertex == 0) {
////            cout << __LINE__ << ": Removing " << vertex << " from graph." << endl << flush;
////        }
            vOtherRemovedVertices.push_back(neighbor);
////        if (neighbor == 0) {
////            cout << __LINE__ << ": Removing " << neighbor << " from graph." << endl << flush;
////        }

            remaining.Remove(vertex);
            remaining.Remove(neighbor);

            m_AlternativeVertices[vertex]   = neighbor;
            m_AlternativeVertices[neighbor] = vertex;
            ////cout << "Vertex " << vertex << " has alternative " << neighbor << endl;

////            cout << __LINE__ << ": calling remove" << endl << flush;
            inGraph.Remove(vertex);
////            cout << __LINE__ << ": calling remove" << endl << flush;
            inGraph.Remove(neighbor);
////            cout << "Removing Path: [" << vertex << " " << neighbor << "]" << " (" << endVertex1 << " " << endVertex2 << ")" << endl << flush;

            return true;
        }
    }
    return false;
}
#endif //0

template <typename NeighborSet>
void Isolates3<NeighborSet>::SwapToEnd(vector<int> &vVertices, int const vertexToSwap)
{
#if 0
    size_t vertexIndex = ULONG_MAX;
    for (size_t index = 0; index < vVertices.size(); ++index) {
        if (vVertices[index] == vertexToSwap)
            vertexIndex = index;
        else if (!inGraph.Contains(vVertices[index]) && vertexIndex != ULONG_MAX) {
            if (index > 0) {
                vVertices[vertexIndex] = vVertices[index];
                vVertices[index] = vertexToSwap;
            }
            ////                cout << "Swapped to back()" << endl;
            break;
        }
    }
    if (vertexIndex == ULONG_MAX) {
        cout << "ERROR: Couldn't swap out vertex..." << endl;
    }
#else

    // swap all !ingraph vertices to end, stop when we reach a
    // vertex that is not in the graph, and hasn't been recently removed.
    size_t indexFollowingLastInGraph(0);
    for (size_t index = 0; index < vVertices.size(); ++index) {

        // if this vertex is in the graph
        if (inGraph.Contains(vVertices[index])) {
            int const thisVertex = vVertices[indexFollowingLastInGraph];
            vVertices[indexFollowingLastInGraph++] = vVertices[index];
            vVertices[index] = thisVertex;
        } else if (!vRecentlyRemovedVertices[vVertices[index]]) {
            break;
        }
    }
#endif // 0
}

// TODO/DS: need to add 2-neighbors to remaining.
template <typename NeighborSet>
void Isolates3<NeighborSet>::RemoveVertex(int const vertex)
{
    if (!inGraph.Contains(vertex)) return;
////    cout << __LINE__ << ": Removing vertex " << vertex << endl << flush;
////    cout << __LINE__ << ": calling remove" << endl << flush;
    inGraph.Remove(vertex);
    remaining.Remove(vertex);
    isolates.Remove(vertex);
    vRecentlyRemovedVertices[vertex] = true;

    for (int const neighbor : neighbors[vertex]) {
        neighbors[neighbor].Remove(vertex);
        remaining.Insert(neighbor);

        // temporary, can delay until isolate removal
        SwapToEnd(m_ReorderedAdjacencyArray[neighbor], vertex);
    }

    neighbors[vertex].Clear();
    vRecentlyRemovedVertices[vertex] = false;
}

//TODO/DS: need to add 2-neighbors to remaining.
template <typename NeighborSet>
void Isolates3<NeighborSet>::RemoveVertexAndNeighbors(int const vertex, vector<int> &vRemoved)
{
    if (!inGraph.Contains(vertex)) return;
////    cout << __LINE__ << ": Removing vertex " << vertex << endl << flush;
////    cout << __LINE__ << ": calling remove" << endl << flush;
    inGraph.Remove(vertex);
    remaining.Remove(vertex);
    isolates.Insert(vertex);
    vRemoved.push_back(vertex);
    vRecentlyRemovedVertices[vertex] = true;
////    if (vertex == 0) {
////        cout << __LINE__ << ": Removing " << vertex << " from graph." << endl << flush;
////    }

    for (int const neighbor : neighbors[vertex]) {
////        cout << __LINE__ << ":     Removing neighbor " << neighbor << endl << flush;
////        cout << __LINE__ << ": calling remove" << endl << flush;
        inGraph.Remove(neighbor);
        remaining.Remove(neighbor);
        vRemoved.push_back(neighbor);
        vRecentlyRemovedVertices[neighbor] = true;
    }
////        if (neighbor == 0) {
////            cout << __LINE__ << ": Removing " << neighbor << " from graph." << endl << flush;
////        }
    for (int const neighbor : neighbors[vertex]) {
        for (int const nNeighbor : neighbors[neighbor]) {
////            cout << __LINE__ << ":         Removing from neighbor's neighbor "<< nNeighbor << endl << flush;
            if (nNeighbor == vertex) continue;
            neighbors[nNeighbor].Remove(neighbor);
            SwapToEnd(m_ReorderedAdjacencyArray[nNeighbor], neighbor);
////            cout << __LINE__ << ":         Done removing" << endl << flush;
            if (inGraph.Contains(nNeighbor))
                remaining.Insert(nNeighbor);
        }
////        cout << __LINE__ << ":     Done Removing neighbor" << neighbor << endl << flush;
////        cout << __LINE__ << ": Cleared neighbors: " << neighbor << endl << flush;
    }

    for (int const neighbor : neighbors[vertex]) {
        neighbors[neighbor].Clear();
        vRecentlyRemovedVertices[neighbor] = false;
    }

    vRecentlyRemovedVertices[vertex] = false;

////    cout << __LINE__ << ": Done Removing vertex " << vertex << endl << flush;
    neighbors[vertex].Clear();
////    cout << __LINE__ << ": Cleared neighbors: " << vertex << endl << flush;
}

template <typename NeighborSet>
void Isolates3<NeighborSet>::RemoveAllIsolates(int const independentSetSize, vector<int> &vIsolateVertices, vector<int> &vOtherRemovedVertices, vector<pair<int,int>> &vAddedEdges, bool bConsiderAllVertices)
{
////    if (find (vIsolateVertices.begin(), vIsolateVertices.end(), 31) != vIsolateVertices.end())
////        cout << "Calling RemoveAllIsolates with 31 in the isolate set!" << endl;
#ifdef TIMERS
    clock_t startClock = clock();
#endif // TIMERS
////    remaining = inGraph; // TODO/DS : We can optimize this by knowing which vertex (and neighbors where removed last.
////    if (vOtherRemovedVertices.empty()) {

        // TODO/DS: Put this in; it saves us from having to consider obvious non-candidates. Only works if we establish
        // the invariant that the graph contains no vertices that can be reduced.
#ifdef NO_INVARIANT
        if (bConsiderAllVertices) {
#else
        if (true) { //bConsiderAllVertices) {
#endif // NO_INVARIANT
            remaining.Clear();
            for (int const vertex : inGraph) {
                remaining.Insert(vertex);
            }
        }
////    } else {
////        remaining.clear();
////        for (int const removedVertex : vOtherRemovedVertices) {
////            remaining.insert(neighbors[removedVertex].begin(), neighbors[removedVertex].end());
////        }
////    }
////    cout << "Removing all isolates." << endl << flush;
    int iterations(0);
    while (!remaining.Empty()) {
////        size_t const numRemoved(vIsolateVertices.size() + vOtherRemovedVertices.size());
////        if ((vIsolateVertices.size() + vOtherRemovedVertices.size()) %10000 == 0)
////        cout << "Progress: Removed: " << vIsolateVertices.size() << " isolates, and " << vOtherRemovedVertices.size() << " others" << endl << flush;
        int const vertex = *(remaining.begin());
        remaining.Remove(vertex);
////        if (neighbors[vertex].Size() > 2) continue;

    // can prune out high-degree vertices too. // but this is slow right now.

////        if (inGraph.size() - neighbors[vertex].size() < independentSetSize) {
////            remaining.insert(neighbors[vertex].begin(), neighbors[vertex].end());
////            RemoveVertex(vertex);
////            vOtherRemovedVertices.push_back(vertex);
////            continue;
////        }

////        cout << "Attempting to remove vertex " << vertex << endl << flush;

        bool reduction = RemoveIsolatedClique(vertex, vIsolateVertices, vOtherRemovedVertices);
////        if (!reduction) {
////            reduction = RemoveIsolatedPath(vertex, vIsolateVertices, vOtherRemovedVertices, vAddedEdges);
////        }

////    if (find (vIsolateVertices.begin(), vIsolateVertices.end(), 31) != vIsolateVertices.end())
////        cout << "31 was added to the isolate set!" << endl;
////        if (!inGraph.Contains(31)) cout << "And it's not in the graph..." << endl << flush;

        iterations++;

////        size_t const numNewRemoved(vIsolateVertices.size() + vOtherRemovedVertices.size());
////        if (numNewRemoved != numRemoved) {
////            cout << "Progress: Removed: " << vIsolateVertices.size() << " isolates, and " << vOtherRemovedVertices.size() << " others, in " << iterations << " iterations." << endl << flush;
////            iterations = 0;
////            cout << "Remaining graph has " << inGraph.Size() << " vertices." << endl << flush;
////        }
    }

////    cout << "Removed " << isolates.size() - isolateSize << " isolates." << endl << flush;
////    cout << "Removed: " << vIsolateVertices.size() << " isolates, and " << vOtherRemovedVertices.size() << " others, in " << iterations << " iterations." << endl << flush;

#ifdef TIMERS
    clock_t endClock = clock();
    removeTimer += (endClock - startClock);
#endif // TIMERS
}

template <typename NeighborSet>
void Isolates3<NeighborSet>::RemoveAllIsolates(int const independentSetSize, vector<int> &vIsolateVertices, vector<int> &vOtherRemovedVertices, vector<pair<int,int>> &vAddedEdges, bool bConsiderAllVertices, ArraySet const &onlyConsider)
{
////    if (find (vIsolateVertices.begin(), vIsolateVertices.end(), 31) != vIsolateVertices.end())
////        cout << "Calling RemoveAllIsolates with 31 in the isolate set!" << endl;
#ifdef TIMERS
    clock_t startClock = clock();
#endif // TIMERS
////    remaining = inGraph; // TODO/DS : We can optimize this by knowing which vertex (and neighbors where removed last.
////    if (vOtherRemovedVertices.empty()) {

        // TODO/DS: Put this in; it saves us from having to consider obvious non-candidates. Only works if we establish
        // the invariant that the graph contains no vertices that can be reduced.
////        if (bConsiderAllVertices) { ////true) { //bConsiderAllVertices) {
        if (true) { //bConsiderAllVertices) {
            remaining.Clear();
            for (int const vertex : inGraph) {
                remaining.Insert(vertex);
            }
        }
////    } else {
////        remaining.clear();
////        for (int const removedVertex : vOtherRemovedVertices) {
////            remaining.insert(neighbors[removedVertex].begin(), neighbors[removedVertex].end());
////        }
////    }
////    cout << "Removing all isolates." << endl << flush;
    int iterations(0);
    while (!remaining.Empty()) {
////        size_t const numRemoved(vIsolateVertices.size() + vOtherRemovedVertices.size());
////        if ((vIsolateVertices.size() + vOtherRemovedVertices.size()) %10000 == 0)
////        cout << "Progress: Removed: " << vIsolateVertices.size() << " isolates, and " << vOtherRemovedVertices.size() << " others" << endl << flush;
        int const vertex = *(remaining.begin());
        remaining.Remove(vertex);
        if (!onlyConsider.Contains(vertex)) continue;

    // can prune out high-degree vertices too. // but this is slow right now.

////        if (inGraph.size() - neighbors[vertex].size() < independentSetSize) {
////            remaining.insert(neighbors[vertex].begin(), neighbors[vertex].end());
////            RemoveVertex(vertex);
////            vOtherRemovedVertices.push_back(vertex);
////            continue;
////        }

////        cout << "Attempting to remove vertex " << vertex << endl << flush;

        bool reduction = RemoveIsolatedClique(vertex, vIsolateVertices, vOtherRemovedVertices);
////        if (!reduction) {
////            reduction = RemoveIsolatedPath(vertex, vIsolateVertices, vOtherRemovedVertices, vAddedEdges);
////        }

////    if (find (vIsolateVertices.begin(), vIsolateVertices.end(), 31) != vIsolateVertices.end())
////        cout << "31 was added to the isolate set!" << endl;
////        if (!inGraph.Contains(31)) cout << "And it's not in the graph..." << endl << flush;

        iterations++;

////        size_t const numNewRemoved(vIsolateVertices.size() + vOtherRemovedVertices.size());
////        if (numNewRemoved != numRemoved) {
////            cout << "Progress: Removed: " << vIsolateVertices.size() << " isolates, and " << vOtherRemovedVertices.size() << " others, in " << iterations << " iterations." << endl << flush;
////            iterations = 0;
////            cout << "Remaining graph has " << inGraph.Size() << " vertices." << endl << flush;
////        }
    }

////    cout << "Removed " << isolates.size() - isolateSize << " isolates." << endl << flush;
////    cout << "Removed: " << vIsolateVertices.size() << " isolates, and " << vOtherRemovedVertices.size() << " others, in " << iterations << " iterations." << endl << flush;

#ifdef TIMERS
    clock_t endClock = clock();
    removeTimer += (endClock - startClock);
#endif // TIMERS
}

template <typename NeighborSet>
void Isolates3<NeighborSet>::ReplaceAllRemoved(vector<int> const &vRemoved)
{
#ifdef TIMERS
    clock_t startClock = clock();
#endif //TIMERS
    for (int const removedVertex : vRemoved) {
        inGraph.Insert(removedVertex);
        isolates.Remove(removedVertex);
#ifdef NO_INVARIANT
        remaining.Insert(removedVertex);
#endif // NO_INVARIANT
        m_AlternativeVertices.erase(removedVertex);
    }
////    cout << "Replacing all removed vertices." << endl << flush;
    for (int const removedVertex : vRemoved) {
////        if (remaining.size() %10000 == 0)
////            cout << "Remaining: " << remaining.size() << ", Removed: " << removed.size() << endl << flush;

////        bool bRestOfNeighborsAreExcluded(false);
        for (int const neighbor : m_ReorderedAdjacencyArray[removedVertex]) {
            if (!inGraph.Contains(neighbor)) {
////                bRestOfNeighborsAreExcluded = true;
                break;
            }

////            if (bRestOfNeighborsAreExcluded) {
////                cout << "ERROR: All remaining vertices should be excluded from graph!" << endl;
////            }
            neighbors[removedVertex].Insert(neighbor);
            neighbors[neighbor].Insert(removedVertex);
        }
    }
////    cout << "Done replacing vertices..." << endl << flush;

#ifdef TIMERS
    clock_t endClock = clock();
    replaceTimer += (endClock - startClock);
    #endif // TIMERS
}

template <typename NeighborSet>
int Isolates3<NeighborSet>::NextVertexToRemove(std::vector<int> &vVertices)
{
#ifdef TIMERS
    clock_t startClock = clock();
    #endif // TIMERS

#if 1
    if (vVertices.empty()) return -1;

////    set<int> setVertices; setVertices.insert(vVertices.begin(), vVertices.end());
////    cout << "Choices: ";
////    for (int const vertex : setVertices) {
////        cout << vertex << "(degree=" << neighbors[vertex].Size() << ") ";
////    }
////    cout << endl << flush;
    int index(0);
    size_t currentMaxSize(neighbors[vVertices[index]].Size());
    for (int i = 1; i < vVertices.size(); ++i) {
        size_t const newCandidateSize(neighbors[vVertices[i]].Size());
////        cout << vVertices[i] << "(" << newCandidateSize << " neighbors) ";

        // break ties in favor of vertex with smaller id
        if ((currentMaxSize < newCandidateSize) || ((currentMaxSize == newCandidateSize) && (vVertices[i] < vVertices[index])))
        {
            currentMaxSize = newCandidateSize;
            index = i;
        }
    }
////    cout << endl << flush;

    int const vertexWithMaxReductions = vVertices[index];
////    cout << "Choosing to remove " << vertexWithMaxReductions << " next, with " << neighbors[vVertices[index]].Size() << " neighbors " << endl; 

    vVertices[index]= vVertices.back();
    vVertices.pop_back();

#ifdef TIMERS
    clock_t endClock = clock();
    timer += (endClock - startClock);
#endif // TIMERS

    return vertexWithMaxReductions;
#else

    remaining.Clear();
    for (int const vertex : inGraph) {
        remaining.Insert(vertex); // only consider neighbors of removed vertices... makes it faster.
    }

    vector<int> &vVerticesOrderedByDegree(vVertices);

#ifdef TIMERS
    clock_t const startSort(clock());
#endif // TIMERS
    auto sortByDegree = [this](int const &leftVertex, int const &rightVertex) { return neighbors[leftVertex].Size() > neighbors[rightVertex].Size(); };
    sort(vVerticesOrderedByDegree.begin(), vVerticesOrderedByDegree.end(), sortByDegree);
#ifdef TIMERS
    sortDuringNextTimer += (clock() - startSort);
#endif // TIMERS

    if (vVerticesOrderedByDegree.empty()) return -1;

    int vertexWithMaxReductions(vVerticesOrderedByDegree[0]); // default to max degree vertex
    int maxIsolates(-1);
    int maxRemoved(-1);
    int maxIndex(0);
    size_t index(0); // default to index of maximum degree vertex

    for (int const vertex : inGraph) {
#ifdef SPARSE
        SparseArraySet &neighborSet(neighbors[vertex]);
#else
        NeighborSet &neighborSet(neighbors[vertex]);
#endif // SPARSE
        neighborSet.SaveState(); // checkpoint the neighbors so we can easily restore them after test-removing vertices.
    }

////    cout << "Testing remaining " << inGraph.size() << " vertices, to maximize isolate removal" << endl << flush;
    for (int const vertex : vVerticesOrderedByDegree) {
////        cout << "Starting loop..." << endl << flush;
////        set<int> tempRemaining(remaining); // TODO/DS: put back?

        // TODO/DS: Remove when vertex selection becomes faster, this is a compromise between fast vertex selection and good vertex selection.
        if (index >= vVerticesOrderedByDegree.size()/10) break;
////        if (index >= 2) break;

#ifdef DEBUG
#ifdef SPARSE
        vector<SparseArraySet> savedNeighbors(neighbors);
#else
        vector<NeighborSet> savedNeighbors(neighbors);
#endif // SPARSE
#endif // DEBUG

        remaining.Clear();

        inGraph.SaveState();

#ifdef DEBUG
        ArraySet savedInGraph(inGraph);
#endif // DEBUG

#ifdef SPARSE 
        SparseArraySet tempIsolates(m_ReorderedAdjacencyArray.size());
#else
        ArraySet tempIsolates(m_ReorderedAdjacencyArray.size());
#endif // SPARSE
        for (int const isolateVertex : isolates) {
            tempIsolates.Insert(isolateVertex);
        }

#ifdef TIMERS
        clock_t startRemoveOne(clock());
        #endif // TIMERS
        vector<int> vRemoved;
////        cout << "Try removing vertex " << vertex << endl << flush;
#ifdef OPTIMIZE_BRANCHING
        RemoveVertex(vertex); vRemoved.push_back(vertex);
#else
        RemoveVertexAndNeighbors(vertex, vRemoved);
#endif // OPTIMIZE_BRANCHING
////        cout << "And the new isolates..." << vertex << endl << flush;
#ifdef TIMERS
        removeOneDuringNextTimer += (clock()-startRemoveOne);
#endif // TIMERS
        vector<pair<int,int>> vAddedEdges;

#ifdef TIMERS
        clock_t startremove(clock());
        clock_t startRealRemove(clock());
        #endif // TIMERS
        RemoveAllIsolates(0, vRemoved, vRemoved, vAddedEdges);
#ifdef TIMERS
        removeTimer -= (clock() - startRealRemove);
        removeDuringNextTimer += (clock() - startremove);

        clock_t startReplace(clock()); // restoring all sets is considered replace
        #endif // TIMERS
        int const newNumRemoved(m_ReorderedAdjacencyArray.size() - inGraph.Size());

        inGraph.RestoreState();
        int const oldNumRemoved(m_ReorderedAdjacencyArray.size() - inGraph.Size());
        int const deltaRemoved(newNumRemoved - oldNumRemoved);
#if 0 //def OPTIMIZE_ISOLATES
        if (static_cast<int>(isolates.size() - tempIsolates.size()) > maxIsolates) {
            maxIsolates = static_cast<int>(isolates.size() - tempIsolates.size());
            vertexWithMaxReductions = vertex;
        }
#else // optimize removed vertices
        if (deltaRemoved > maxRemoved) {
            maxRemoved = deltaRemoved;
            vertexWithMaxReductions = vertex;
            maxIndex = index;
        }
#endif

////        remaining = std::move(tempRemaining); // TODO/DS: put back?

        isolates.Clear();
        for (int const isolatedVertex : tempIsolates) {
            isolates.Insert(isolatedVertex);
        }

        // need to do this after restoring variable inGraph
////        RemoveEdges(vAddedEdges); // TODO/DS: Put back...


////        for (int const removedVertex : vRemoved) {
////            inGraph.Insert(removedVertex);
////            isolates.Remove(removedVertex);
////            m_AlternativeVertices.erase(removedVertex);
////        }

        for (int const vertex : inGraph) {
#ifdef SPARSE
            SparseArraySet &neighborSet(neighbors[vertex]);
#else
            NeighborSet &neighborSet(neighbors[vertex]);
#endif // SPARSE
            neighborSet.RestoreState(); // checkpoint the neighbors so we can easily restore them after test-removing vertices.
            neighborSet.SaveState();
        }

#ifdef DEBUG
        if (savedInGraph != inGraph) {
            std::cout << "graph size changed..." << inGraph.Size() << "->" << savedInGraph.Size() << std::endl;
        }

        for (int vertex = 0; vertex < m_ReorderedAdjacencyArray.size(); vertex++) {
            if (savedNeighbors[vertex].Size() != neighbors[vertex].Size()) {
                std::cout << "vertex " << vertex << "'s degree changed!" << std::endl << std::flush;
                std::cout << "before: " << savedNeighbors[vertex].Size() << ", after: " << neighbors[vertex].Size() << endl;
                std::cout << "before: ";
                for (int const neighbor : savedNeighbors[vertex]) {
                    cout << neighbor << " ";
                }
                std::cout << "after: ";
                for (int const neighbor : neighbors[vertex]) {
                    cout << neighbor << " ";
                }
            }
        }
#endif // DEBUG

////        clock_t startreplace(clock());
////        ReplaceAllRemoved(vRemoved);
////        replaceTimer -= (clock() - startreplace);

        index++;

////        cout << "Done with loop" << endl;
    }

#ifdef TIMERS
    clock_t endClock = clock();
    timer += (endClock - startClock);

    clock_t startReplace(clock());
#endif // TIMERS
    for (int const vertex : inGraph) {
#ifdef SPARSE
        SparseArraySet &neighborSet(neighbors[vertex]);
#else
        NeighborSet &neighborSet(neighbors[vertex]);
#endif // SPARSE
        neighborSet.RestoreState();
    }
#ifdef TIMERS
    replaceDuringNextTimer += (clock() - startReplace);
    #endif // TIMERS

    // inefficient, should be a better way to elmininate it before getting here.
#ifdef SLOW
    for (int &vertex : vVertices) {
        if (vertex == vertexWithMaxReductions) {
            vertex = vVertices.back();
            vVertices.pop_back();
            return vertexWithMaxReductions;
        }
    }
#else
    vVertices[maxIndex] = vVertices.back();
    vVertices.pop_back();
    return vertexWithMaxReductions;
#endif // SLOW
#endif // 0

    return -1;
}

template <typename NeighborSet>
int Isolates3<NeighborSet>::NextVertexToRemove()
{
    vector<int> vVertices;
    vVertices.insert(vVertices.end(), inGraph.begin(), inGraph.end());
    return NextVertexToRemove(vVertices);
}

template <typename NeighborSet>
int Isolates3<NeighborSet>::GetAlternativeVertex(int const vertex) const
{
    map<int,int>::const_iterator cit(m_AlternativeVertices.find(vertex));
    if (cit != m_AlternativeVertices.end()) {
        return cit->second;
    }
    return -1;
}

// vVertices needs to be a subset of the graph, otherwise this will
// fail horribly.
template <typename NeighborSet>
void Isolates3<NeighborSet>::SetConnectedComponent(vector<int> const &vVertices)
{
    inGraph.Clear();
    for (int const vertex : vVertices) {
        inGraph.Insert(vertex);
    }

    m_bConnectedComponentMode = true;
}

template <typename NeighborSet>
double Isolates3<NeighborSet>::GetDensity() const
{
    size_t edges(0);
    for (int const vertex : inGraph) {
        edges += neighbors[vertex].Size();
    }
    edges >>= 1;
    return (edges / static_cast<double>(inGraph.Size()));
}

template <typename NeighborSet>
size_t Isolates3<NeighborSet>::GetMaxDegree() const
{
    size_t maxDegree(0);
    for (int const vertex : inGraph) {
        maxDegree = max(maxDegree, neighbors[vertex].Size());
    }
    return maxDegree;
}

template class Isolates3<ArraySet>;
template class Isolates3<SparseArraySet>;

