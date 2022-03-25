/**
 * @file list.cpp
 * Doubly Linked List (MP 3).
 */

template <class T>
List<T>::List() {
  // @TODO: graded in MP3.1
    head_ = NULL;
    tail_ = NULL;
    length_ = 0;
}

/**
 * Returns a ListIterator with a position at the beginning of
 * the List.
 */
template <typename T>
typename List<T>::ListIterator List<T>::begin() const {

  return List<T>::ListIterator(head_);
}

/**
 * Returns a ListIterator one past the end of the List.
 */
template <typename T>
typename List<T>::ListIterator List<T>::end() const {
  // @TODO: graded in MP3.1
  return List<T>::ListIterator(NULL);
}


/**
 * Destroys all dynamically allocated memory associated with the current
 * List class.
 */
template <typename T>
void List<T>::_destroy() {
  ListNode* node = head_;
  while(node != NULL){
    ListNode * temp = node->next;
    delete node;
    node = temp;
  }
  tail_ = NULL;
  length_ = 0;
}

/**
 * Inserts a new node at the front of the List.
 * This function **SHOULD** create a new ListNode.
 *
 * @param ndata The data to be inserted.
 */
template <typename T>
void List<T>::insertFront(T const & ndata) {
  /// @todo Graded in MP3.1
  ListNode * newNode = new ListNode(ndata);
  newNode -> next = head_;
  newNode -> prev = NULL;

  if (head_ != NULL) {
    head_ -> prev = newNode;

  }
  if (tail_ == NULL) {
    tail_ = newNode;
  }
  head_ = newNode;

  length_++;

}

/**
 * Inserts a new node at the back of the List.
 * This function **SHOULD** create a new ListNode.
 *
 * @param ndata The data to be inserted.
 */
template <typename T>
void List<T>::insertBack(const T & ndata) {
  ListNode * newNode = new ListNode(ndata);

  newNode -> next = NULL;
  newNode -> prev = tail_;

  if (tail_!=NULL) {
    tail_->next = newNode;
  }
  if (head_ == NULL) {
    head_ = newNode;
  }
  tail_ = newNode;

  length_++;

}

/**
 * Helper function to split a sequence of linked memory at the node
 * splitPoint steps **after** start. In other words, it should disconnect
 * the sequence of linked memory after the given number of nodes, and
 * return a pointer to the starting node of the new sequence of linked
 * memory.
 *
 * This function **SHOULD NOT** create **ANY** new List or ListNode objects!
 *
 * This function is also called by the public split() function located in
 * List-given.hpp
 *
 * @param start The node to start from.
 * @param splitPoint The number of steps to walk before splitting.
 * @return The starting node of the sequence that was split off.
 */
template <typename T>
typename List<T>::ListNode * List<T>::split(ListNode * start, int splitPoint) {

  ListNode * curr = start;

  for (int i = 0; i < splitPoint && curr != NULL; i++) {
    curr = curr->next;
  }

  if (curr != NULL) {
      tail_ = curr->prev;
      curr->prev->next = NULL;
      curr->prev = NULL;
  }

  return curr;//change length maybe
}

/**
 * Modifies the List using the waterfall algorithm.
 * Every other node (starting from the second one) is removed from the
 * List, but appended at the back, becoming the new tail. This continues
 * until the next thing to be removed is either the tail (**not necessarily
 * the original tail!**) or NULL.  You may **NOT** allocate new ListNodes.
 * Note that since the tail should be continuously updated, some nodes will
 * be moved more than once.
 */
template <typename T>
void List<T>::waterfall() {
  ListNode* node = head_;
  ListNode* last = tail_;
  while(node->next!=last && node!=last && node!=NULL && node->next!=NULL&&node->next->next!=NULL){

    node = node->next;
    ListNode* nextuh = node->next;
    node->prev->next = node->next;
    node->next->prev = node->prev;
    node->prev = last;
    last->next = node;
    node->next = NULL;
    last = node;
    node = nextuh;
  }
}

/**
 * Reverses the current List.
 */
template <typename T>
void List<T>::reverse() {
  reverse(head_, tail_);
}

/**
 * Helper function to reverse a sequence of linked memory inside a List,
 * starting at startPoint and ending at endPoint. You are responsible for
 * updating startPoint and endPoint to point to the new starting and ending
 * points of the rearranged sequence of linked memory in question.
 *
 * @param startPoint A pointer reference to the first node in the sequence
 *  to be reversed.
 * @param endPoint A pointer reference to the last node in the sequence to
 *  be reversed.
 */
template <typename T>
void List<T>::reverse(ListNode *& startPoint, ListNode *& endPoint) {
  if(startPoint == endPoint||startPoint == NULL || endPoint == NULL)
    return;
  ListNode * temp; // set equal to NULL
  ListNode * temp2;
  ListNode * start = startPoint;
  ListNode * originPrev = startPoint->prev;
  ListNode * endNext = endPoint->next;
  if(start==NULL || endPoint == NULL){
    return;
  }
  while(start != endPoint){
    temp = start->prev;
    temp2 = start->next;
    start->next = temp;
    start->prev = temp2;
    start = temp2;
  }
  temp = start->prev;
  start->prev = originPrev;
  start->next = temp;

  if(originPrev != NULL){
    originPrev->next = start;
  }
  if(endNext != NULL){
    endNext->prev = startPoint;
  }
  startPoint->next = endNext;
  endPoint->prev = originPrev;
  if(startPoint == head_){
    head_ = endPoint;
  }
  temp = startPoint;
  startPoint = endPoint;
  endPoint = temp;

  //if changing the data did not work, go and change the pointers to
  //each one instead
}

/**
 * Reverses blocks of size n in the current List. You should use your
 * reverse( ListNode * &, ListNode * & ) helper function in this method!
 *
 * @param n The size of the blocks in the List to be reversed.
 */
template <typename T>
void List<T>::reverseNth(int n) {
  ListNode* temp;
  ListNode* node = head_;
  ListNode* start = node;

  if(node == NULL)
    return;

  while(start!=NULL && start!=tail_){
    node = start;
    for(int i=0;i<n-1;i++){
      if(node==tail_){break;}
      node = node->next;

  }
    if(node == start){return;}
    reverse(start,node);
    start = node->next;
  }


}


/**
 * Merges the given sorted list into the current sorted list.
 *
 * @param otherList List to be merged into the current list.
 */
template <typename T>
void List<T>::mergeWith(List<T> & otherList) {
    // set up the current list
    head_ = merge(head_, otherList.head_);
    tail_ = head_;

    // make sure there is a node in the new list
    if (tail_ != NULL) {
        while (tail_->next != NULL)
            tail_ = tail_->next;
    }
    length_ = length_ + otherList.length_;

    // empty out the parameter list
    otherList.head_ = NULL;
    otherList.tail_ = NULL;
    otherList.length_ = 0;
}

/**
 * Helper function to merge two **sorted** and **independent** sequences of
 * linked memory. The result should be a single sequence that is itself
 * sorted.
 *
 * This function **SHOULD NOT** create **ANY** new List objects.
 *
 * @param first The starting node of the first sequence.
 * @param second The starting node of the second sequence.
 * @return The starting node of the resulting, sorted sequence.
 */
template <typename T>
typename List<T>::ListNode * List<T>::merge(ListNode * first, ListNode* second) {
  ListNode * temp1 = first;
  ListNode * temp2 = second;
  ListNode * list;
  ListNode * retrn;
  if(temp2->data < temp1->data){
    list = temp2;
    temp2 = temp2->next;
    retrn = second;
  }
  else{
    list = temp1;
    temp1 = temp1->next;
    retrn = first;
}
  if(first==NULL&&second==NULL){
    return NULL;
  }
  while(temp1 != NULL && temp2 != NULL){
    if(temp1->data < temp2->data){
      list->next = temp1;
      temp1->prev = list;
      list = list->next;
      temp1 = temp1->next;
    }
    else{
      list->next = temp2;
      temp2->prev = list;
      list = list->next;
      temp2 = temp2->next;
    }

  }
  if(temp1 == NULL && temp2!=NULL){
    list->next = temp2;
    temp2->prev = list;
  }
  else if(temp2 == NULL && temp1!= NULL){
    list->next = temp1;
    temp1->prev = list;
  }

  //set the rest of the non null temp into the list

  return retrn;

//
}//dont forget to update length of the list
///   ///    /////////
///   ///       ///
/////////       ///
/////////       ///
///   ///       ///
///   ///    /////////




/**
 * Sorts a chain of linked memory given a start node and a size.
 * This is the recursive helper for the Mergesort algorithm (i.e., this is
 * the divide-and-conquer step).
 *
 * Called by the public sort function in List-given.hpp
 *
 * @param start Starting point of the chain.
 * @param chainLength Size of the chain to be sorted.
 * @return A pointer to the beginning of the now sorted chain.
 */
template <typename T>
typename List<T>::ListNode* List<T>::mergesort(ListNode * start, int chainLength) {
  if(chainLength == 1)
    return start;
  ListNode * temp;
  ListNode * leftNode = start;
  ListNode * rightNode = split(start,chainLength/2);
  int length = chainLength/2;
  if(chainLength%2!=0){
    length++;
  }
  rightNode = mergesort(rightNode, length);
  leftNode = mergesort(leftNode, chainLength/2);
  temp = merge(rightNode,leftNode);
  return temp;




}
