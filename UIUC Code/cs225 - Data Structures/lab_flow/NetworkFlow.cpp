/**
 * @file NetworkFlow.cpp
 * CS 225: Data Structures
 */

#include <vector>
#include <algorithm>
#include <set>

#include "graph.h"
#include "edge.h"

#include "NetworkFlow.h"

int min(int a, int b) {
  if (a<b)
    return a;
  else return b;
}

NetworkFlow::NetworkFlow(Graph & startingGraph, Vertex source, Vertex sink) :
  g_(startingGraph), residual_(Graph(true,true)), flow_(Graph(true,true)), source_(source), sink_(sink) {
    maxFlow_ = 0;
    for(Edge edge : startingGraph.getEdges()){
      residual_.insertEdge(edge.source, edge.dest);

      residual_.setEdgeWeight(edge.source, edge.dest, edge.getWeight());


      residual_.insertEdge(edge.dest, edge.source);

      residual_.setEdgeWeight(edge.dest, edge.source, 0);


      flow_.insertEdge(edge.source, edge.dest);

      flow_.setEdgeWeight(edge.source, edge.dest, 0);




      /*residual_.insertEdge(edge.source, edge.sink);
      residual_.setEdgeWeight(edge.source, edge.sink, edge.getWeight());


      flow_.insertEdge(edge.source,edge.sink);
      flow_.setEdgeWeight(edge.source, edge.sink, edge.getWeight());
    */}

}

  /**
   * findAugmentingPath - use DFS to find a path in the residual graph with leftover capacity.
   *  This version is the helper function.
   *
   * @param source  The starting (current) vertex
   * @param sink    The destination vertex
   * @param path    The vertices in the path
   * @param visited A set of vertices we have visited
   */

bool NetworkFlow::findAugmentingPath(Vertex source, Vertex sink,
  std::vector<Vertex> &path, std::set<Vertex> &visited) {

  if (visited.count(source) != 0)
    return false;
  visited.insert(source);

  if (source == sink) {
    return true;
  }

  vector<Vertex> adjs = residual_.getAdjacent(source);
  for(auto it = adjs.begin(); it != adjs.end(); it++) {
    if (visited.count(*it) == 0 && residual_.getEdgeWeight(source,*it) > 0) {
      path.push_back(*it);
      if (findAugmentingPath(*it,sink,path,visited))
        return true;
      else {
        path.pop_back();
      }
    }
  }

  return false;
}

  /**
   * findAugmentingPath - use DFS to find a path in the residual graph with leftover capacity.
   *  This version is the main function.  It initializes a set to keep track of visited vertices.
   *
   * @param source The starting (current) vertex
   * @param sink   The destination vertex
   * @param path   The vertices in the path
   */

bool NetworkFlow::findAugmentingPath(Vertex source, Vertex sink, std::vector<Vertex> &path) {
   std::set<Vertex> visited;
   path.clear();
   path.push_back(source);
   return findAugmentingPath(source,sink,path,visited);
}

  /**
   * pathCapacity - Determine the capacity of a path in the residual graph.
   *
   * @param path   The vertices in the path
   */

int NetworkFlow::pathCapacity(const std::vector<Vertex> & path) const {
  int first = residual_.getEdgeWeight(path[0],path[1]);

  for(size_t i=2; i< path.size();i++){
    int temp = residual_.getEdgeWeight(path[i-1], path[i]);
    if(temp < first){
      first = temp;
    }
  }



  return first;
}

  /**
   * calculuateFlow - Determine the capacity of a path in the residual graph.
   * Sets the member function `maxFlow_` to be the flow, and updates the
   * residual graph and flow graph according to the algorithm.
   *
   * @return The network flow graph.
   */

const Graph & NetworkFlow::calculateFlow() {
    std::vector<Vertex> path;
    int counter = 0;
    while(findAugmentingPath(source_,sink_, path)){
      int cap = pathCapacity(path);
      counter += cap;
      for(size_t i=1; i<path.size(); i++){


          int temp = residual_.getEdgeWeight(path[i-1],path[i]);
          int temp2 = temp;
          temp2 += cap;
          temp -= cap;
          residual_.setEdgeWeight(path[i-1],path[i],temp);
          residual_.setEdgeWeight(path[i],path[i-1],temp2);

      }
    }
    maxFlow_ = counter;


    return flow_;
}

int NetworkFlow::getMaxFlow() const {
  return maxFlow_;
}

const Graph & NetworkFlow::getGraph() const {
  return g_;
}

const Graph & NetworkFlow::getFlowGraph() const {
  return flow_;
}

const Graph & NetworkFlow::getResidualGraph() const {
  return residual_;
}
