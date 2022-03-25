#include "List.h"
#include <iostream>

int main() {
  List<int> list;
  List<int> list2;
  list.insertBack(1);
  list2.insertBack(2);
  list.insertBack(3);
  list2.insertBack(4);
  list.insertBack(5);
  list2.insertBack(6);
  list.insertBack(7);
  list2.insertBack(8);
  list.print();
  list2.mergeWith(list);
  list2.print();
  std::cout << std::endl;
}
