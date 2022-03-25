/**
 * @file kdtree.cpp
 * Implementation of KDTree class.
 */

#include <utility>
#include <algorithm>

using namespace std;

template <int Dim>
bool KDTree<Dim>::smallerDimVal(const Point<Dim>& first,
                                const Point<Dim>& second, int curDim) const
{
    if(first[curDim]==second[curDim]){
      return(first<second);
    }
    if(second[curDim]>first[curDim]){
      return true;
    }
    else{
      return false;
    }

    return false;
}

template <int Dim>
bool KDTree<Dim>::shouldReplace(const Point<Dim>& target,
                                const Point<Dim>& currentBest,
                                const Point<Dim>& potential) const
{
    int potentialD = 0;
    int currentD = 0;
    for(int i = 0; i < Dim; i++){
      potentialD = potentialD + (target[i]-potential[i])*(target[i]-potential[i]);
    }
    for(int i = 0; i < Dim; i++){
      currentD = currentD + (target[i]-currentBest[i])*(target[i]-currentBest[i]);
    }
    if(potentialD==currentD){
      if(potential<currentBest){
        return true;
      }
      else{
        return false;
      }
    }
    if(potentialD < currentD){
      return true;
    }
     return false;
}

template <int Dim>
KDTree<Dim>::KDTree(const vector<Point<Dim>>& newPoints)
{
  points = newPoints;
  buildHelper(0,points.size()-1,0);
  treeBuild(root, 0, points.size()-1);

}
template <int Dim>
void KDTree<Dim>::buildHelper(int left, int right, int dimen){

    int middle = (left + right)/2;
    int newIndex = middle;
    while(left<right){
      Point<Dim> value = points[newIndex];
      std::swap(points[newIndex], points[right]);
      int pivotIndex = left;
      for(int i = left; i<right; i++){
        if(smallerDimVal(points[i],value, dimen)==true){
          std::swap(points[pivotIndex], points[i]);
          pivotIndex++;
        }
      }

      std::swap(points[right], points[pivotIndex]);

      if(middle == pivotIndex){
        break;
      }
      if(middle < pivotIndex){
        right = pivotIndex--;
      }
      if(middle > pivotIndex){
        left = pivotIndex++;
      }
    }

    if(left < middle - 1){
      buildHelper(left, middle-1, (dimen + 1) % Dim);
    }
    if(right > middle + 1){
      buildHelper(middle + 1, right, (dimen + 1) % Dim);
    }

}

template <int Dim>
void KDTree<Dim>::treeBuild(KDTreeNode *& nPoint, int left, int right){
    if(right>=left){
      int middle = (left + right)/2;
      nPoint = new KDTreeNode(points[middle]);
      size++;
      treeBuild(nPoint->left, left, middle-1);
      treeBuild(nPoint->right, middle+1, right);
    }
    else{
      nPoint = NULL;
    return;
  }
}

template <int Dim>
KDTree<Dim>::KDTree(const KDTree<Dim>& other) {
    this->points = other.points;
    this->root = other.root;
    this->size = other.size;

  /**
   * @todo Implement this function!
   */
}

template <int Dim>
const KDTree<Dim>& KDTree<Dim>::operator=(const KDTree<Dim>& rhs) {
    this->points = rhs.points;
    this->root = rhs.root;
    this->size = rhs.size;
  /**
   * @todo Implement this function!
   */

  return *this;
}




template <int Dim>
KDTree<Dim>::~KDTree() {
    //for(size_t i = 0; i < points.size(); i++){
      //delet
    //}
    delete root;
    points.clear();
  /**
   * @todo Implement this function!
   */
}

template <int Dim>
Point<Dim> KDTree<Dim>::findNearestNeighbor(const Point<Dim>& query) const
{
    Point<Dim> nPoint = root->point;
    double dist;
    return findNearestNeighborHelper(query, nPoint, root, dist, 0, 0, points.size()-1);
    /**
     * @todo Implement this function!
     */
}

template <int Dim>
Point<Dim> KDTree<Dim>::findNearestNeighborHelper(const Point<Dim>& query, Point<Dim>& nPoint, KDTreeNode* subroot, double & dist, int dimen, int left, int right) const
{
    if(left > right){
      return nPoint;
    }

  if((!subroot->left && !subroot->right) || distance(query, subroot->point)<=dist){
    if(shouldReplace(query,nPoint,subroot->point)){
      nPoint = subroot->point;
      dist = distance(query, nPoint);
    }
  }
      if(subroot->point[dimen] >= query[dimen]-dist){
        nPoint = findNearestNeighborHelper(query, nPoint, subroot->left, dist, (dimen+1)%Dim, left, (left+right)/2-1);
      }
      if(subroot->point[dimen] < query[dimen] + dist){
        nPoint = findNearestNeighborHelper(query, nPoint, subroot->right, dist, (dimen+1)%Dim, (left+right)/2+1, right);
      }

    return nPoint;

}
template <int Dim>
double KDTree<Dim>::distance(const Point<Dim>& query, Point<Dim>& nPoint) const{
    double dist = 0;
    for(int i = 0; i < Dim; i++){
      dist = dist + pow((query[i]-nPoint[i]),2);
    }
    return pow(dist,.5);
}
