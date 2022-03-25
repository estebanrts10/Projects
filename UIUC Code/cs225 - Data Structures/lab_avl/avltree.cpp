/**
 * @file avltree.cpp
 * Definitions of the binary tree functions you'll be writing for this lab.
 * You'll need to modify this file.
 */

template <class K, class V>
V AVLTree<K, V>::find(const K& key) const
{
    return find(root, key);
}

template <class K, class V>
V AVLTree<K, V>::find(Node* subtree, const K& key) const
{
    if (subtree == NULL)
        return V();
    else if (key == subtree->key)
        return subtree->value;
    else {
        if (key < subtree->key)
            return find(subtree->left, key);
        else
            return find(subtree->right, key);
    }
}

template <class K, class V>
void AVLTree<K, V>::rotateLeft(Node*& t)
{
    functionCalls.push_back("rotateLeft"); // Stores the rotation name (don't remove this)
    Node * temp;
    temp = t->right;
    t->right = temp->left;
    temp->left = t;
    t = temp;
    //t->height =heightOrNeg1(t)-1;

}

template <class K, class V>
void AVLTree<K, V>::rotateLeftRight(Node*& t)
{
    functionCalls.push_back("rotateLeftRight"); // Stores the rotation name (don't remove this)
    // Implemented for you:
    rotateLeft(t->left);
    rotateRight(t);
}

template <class K, class V>
void AVLTree<K, V>::rotateRight(Node*& t)
{
    functionCalls.push_back("rotateRight"); // Stores the rotation name (don't remove this)
    Node * temp;
    temp = t->left;
    t->left = temp->right;
    temp->right = t;
    t = temp;
    //t->height =heightOrNeg1(t)-1;

}

template <class K, class V>
void AVLTree<K, V>::rotateRightLeft(Node*& t)
{
    functionCalls.push_back("rotateRightLeft"); // Stores the rotation name (don't remove this)
    rotateRight(t->right);
    rotateLeft(t);
}

template <class K, class V>
void AVLTree<K, V>::rebalance(Node*& subtree)
{
    Node * leftNode = subtree->left;
    Node * rightNode = subtree->right;
    subtree->height = height(subtree);
    if(leftNode != NULL){
      leftNode->height = height(leftNode);
    }
    if(rightNode != NULL){
      rightNode->height = height(rightNode);
    }

    int balance = heightOrNeg1(rightNode) - heightOrNeg1(leftNode);
    if(balance>-2 && balance<2){
      return;
    }
    else{
      if(balance==-2){
        int bal = heightOrNeg1(leftNode->right) - heightOrNeg1(leftNode->left);
        if(bal<0){
          rotateRight(subtree);
        }
        else{
          rotateLeftRight(subtree);
        }
      }
      if(balance==2){
        int bal = heightOrNeg1(rightNode->right) - heightOrNeg1(rightNode->left);
        if(bal>0){
          rotateLeft(subtree);
        }
        else{
          rotateRightLeft(subtree);
        }
      }

    }
}

template <class K, class V>
void AVLTree<K, V>::insert(const K & key, const V & value)
{
    insert(root, key, value);
}

template <class K, class V>
void AVLTree<K, V>::insert(Node*& subtree, const K& key, const V& value)
{

    if(subtree==NULL){
      Node * temp = new Node(key,value);
      subtree = temp;
    }
    else{
      if(subtree->key > key){
        //rebalance(subtree);
        insert(subtree->left,key,value);
        rebalance(subtree);
      }
      if(subtree->key <= key){
        //rebalance(subtree);
        insert(subtree->right,key,value);
        rebalance(subtree);
      }
    }
}

template <class K, class V>
int AVLTree<K, V>::height(const Node* node) const{
    if(node == NULL){
      return 0;
    }
    if(node->left==NULL&&node->right==NULL){
      return 0;
    }
    return 1 + std::max(height(node->left), height(node->right));

}



template <class K, class V>
void AVLTree<K, V>::remove(const K& key)
{
    remove(root, key);
}

template <class K, class V>
void AVLTree<K, V>::remove(Node*& subtree, const K& key)
{
    if (subtree == NULL)
        return;

    if (key < subtree->key) {
        remove(subtree->left,key);

        // your code here
    } else if (key > subtree->key) {
        remove(subtree->right,key);

        // your code here
    } else {
        if (subtree->left == NULL && subtree->right == NULL) {
          delete subtree;
          subtree = NULL;
            /* no-child remove */
            // your code here
        } else if (subtree->left != NULL && subtree->right != NULL) {
          Node * iop = subtree->left;
            while(iop->right!=NULL){
              iop = iop->right;
            }
            swap(iop,subtree);
            remove(subtree->left,key);

            /* two-child remove */
            // your code here
        } else {
          //rightChild remove
          if(subtree->right==NULL){
            Node * temp = subtree->left;
            delete subtree;
            subtree = temp;
          }
          if(subtree->left==NULL){
            Node * temp = subtree->right;
            delete subtree;
            subtree = temp;
          }
            /* one-child remove */
            // your code here
        }

        }
        // your code here
        if(subtree != NULL){
          rebalance(subtree);
    }
}
