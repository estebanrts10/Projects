#include <queue>
#include <algorithm>
#include <string>
#include <list>

/**
 * Returns an std::list of vertex keys that creates any shortest path between `start` and `end`.
 *
 * This list MUST include the key of the `start` vertex as the first vertex in the list, the key of
 * the `end` vertex as the last element in the list, and an ordered list of all vertices that must
 * be traveled along the shortest path.
 *
 * For example, the path a -> c -> e returns a list with three elements: "a", "c", "e".
 *
 * You should use undirected edges. Hint: There are no edge weights in the Graph.
 *
 * @param start The key for the starting vertex.
 * @param end   The key for the ending vertex.
 */
template <class V, class E>
std::list<std::string> Graph<V,E>::shortestPath(const std::string start, const std::string end) {
  // TODO: Part 3
  std::list<std::string> path;
  std::list<std::string> queue;
  std::unordered_map<std::string, std::string> predecessor;
  std::unordered_map<std::string, bool> visited;


  for (auto it = vertexMap.begin(); it != vertexMap.end(); ++it){
    visited.insert({it->first, false});
    predecessor.insert({it->first, "xxx"});
  }

  // while (!queue.empty()){
  //   std::string tempKey = queue.front();
  //   queue.pop_front();
  //   std::cout << "\n" << tempKey << "-";
  //   for (auto & it : adjList.at(tempKey)){
  //     std::string tempKey2 = it->get().dest().key();
  //     std::cout << tempKey2 << "-";
  //     if (visited.at(tempKey2) == false){
  //       visited[tempKey2] = true;
  //       predecessor[tempKey2] = tempKey;
  //       queue.push_back(tempKey2);
  //
  //       if (tempKey2 == end){
  //         std::cout << "broken\n";
  //         break;
  //       }
  //     }
  //   }
  // }
  queue.push_back(start);
  visited[start] = true;
  while (!queue.empty()){
    std::string tempKey = queue.front();
    queue.pop_front();
    //std::cout << "\n" << tempKey << "-";
    //std::list<std::reference_wrapper<E>> iEdges = incidentEdges(tempKey);
    for (auto & it : adjList.at(tempKey)){
      std::string tempKey2 = it->get().dest().key();
      if (tempKey2 == tempKey)
        tempKey2 = it->get().source().key();
      //std::cout << tempKey2 << "-";
      if (visited.at(tempKey2) == false){
        visited[tempKey2] = true;
        predecessor[tempKey2] = tempKey;
        queue.push_back(tempKey2);

        if (tempKey2 == end){
        //  std::cout << "broken\n";
          break;
        }
      }
    }
  }

  std::string crawl = end;
  path.push_back(end);
  while (predecessor.at(crawl) != "xxx"){
    path.push_back(predecessor.at(crawl));
    crawl = predecessor.at(crawl);
  }

  path.reverse();

  for (std::string & it : path){
    //std::cout << it << " - ";
  }
  //std::cout << std::endl;

  return path;
}
