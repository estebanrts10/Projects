#include "Graph.h"
#include "Edge.h"
#include "Vertex.h"

#include <string>
#include <iostream>

/**
* @return The number of vertices in the Graph
*/
template <class V, class E>
unsigned int Graph<V,E>::numVertices() const {
  int counter = 0;
  for(auto i : vertexMap){
    counter++;
  }
  return counter;
}


/**
* The degree of the vertex. For directed: Sum of in-degree and out-degree
* @return Returns the degree of a given vertex.
* @param v Given vertex to return degree.
*/
template <class V, class E>
unsigned int Graph<V,E>::degree(const V & v) const {
  //given a vertex, find all edges attached to that vertex
  return incidentEdges(v.key()).size();
}


/**
* Inserts a Vertex into the Graph.
* @param key The key of the Vertex to insert
* @return The inserted Vertex
*/
template <class V, class E>
V & Graph<V,E>::insertVertex(std::string key) {
  // TODO: Part 2
  V & v = *(new V(key));
  vertexMap.insert({key,v});
  std::list<edgeListIter> list1;
  adjList.insert({key,list1});
  return v;
}


/**
* Removes a given Vertex
* @param v The Vertex to remove
*/
template <class V, class E>
void Graph<V,E>::removeVertex(const std::string & key) {

  for(auto x : adjList.at(key)){
    removeEdge(x);
  }
  vertexMap.erase(key);
  adjList.erase(key);




  /*for(auto x : edgeList){
    std::string key1 = x.get().source().key();
    std::string key2 = x.get().dest().key();
    if(key1 == key || key2 == key){
      delete &x;
    }
  }
  for(auto x: vertexMap){
    std::string z = x.first;
    if(z == x.key()){

    }//use x.erase(key) for deleting a vertex
  }*/
}


/**
* Inserts an Edge into the Graph.
* @param v1 The source Vertex
* @param v2 The destination Vertex
* @return The inserted Edge
*/
template <class V, class E>
E & Graph<V,E>::insertEdge(const V & v1, const V & v2) {
  E & e = *(new E(v1, v2));
  edgeList.push_back(e);
  edgeListIter iter = --edgeList.end();
  std::string key1 = v1.key();
  std::string key2 = v2.key();
  adjList.at(key1).push_back(iter);
  adjList.at(key2).push_back(iter);
  return e;
}


/**
* Removes an Edge from the Graph. Consider both the undirected and directed cases.
* min(degree(key1), degree(key2))
* @param key1 The key of the source Vertex
* @param key2 The key of the destination Vertex
*/
template <class V, class E>
void Graph<V,E>::removeEdge(const std::string key1, const std::string key2) {
  for(edgeListIter x : adjList.at(key1)){
    Vertex y = x->get().dest();
    if(y == vertexMap.at(key2)){
      adjList.at(key1).remove(x);
      edgeList.erase(x);
      break;
    }
  }

}


/**
* Removes an Edge from the Graph given by the location of the given iterator into the edge list.
* @param it An iterator at the location of the Edge that
* you would like to remove
*/
template <class V, class E>
void Graph<V,E>::removeEdge(const edgeListIter & it) {
  // TODO: Part 2
  //removeEdge(it->get().source().key(),it->get().dest().key());
  edgeList.erase(it);
}


/**
* Return the list of incident edges from a given vertex.
* For the directed case, consider all edges that has the vertex as either a source or destination.
* @param key The key of the given vertex
* @return The list edges (by reference) that are adjacent to the given vertex
*/
template <class V, class E>
const std::list<std::reference_wrapper<E>> Graph<V,E>::incidentEdges(const std::string key) const {
  // TODO: Part 2
  //return a list of all the edges that are connected to that vertex
  std::list<std::reference_wrapper<E>> edges;
  for(auto edge : edgeList){
    std::string key1 = edge.get().source().key();

    std::string key2 = edge.get().dest().key();

    if(key1 == key || key2 == key){
      edges.push_back(edge);
    }
  }

  return edges;
}


/**
* Return whether the two vertices are adjacent to one another. Consider both the undirected and directed cases.
* When the graph is directed, v1 and v2 are only adjacent if there is an edge from v1 to v2.
* @param key1 The key of the source Vertex
* @param key2 The key of the destination Vertex
* @return True if v1 is adjacent to v2, False otherwise
*/
template <class V, class E>
bool Graph<V,E>::isAdjacent(const std::string key1, const std::string key2) const {
//directed

  for(auto x : adjList.at(key1)){
    if(!(x->get().directed())){
      if(x->get().dest().key() == key2 || x->get().source().key() == key2)
        return true;
    }
    else
      if(x->get().dest().key() == key2)
        return true;
    }



  return false;
}
