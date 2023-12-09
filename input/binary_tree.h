#ifndef BINARY_TREE_H
#define BINARY_TREE_H

#include "tree_elem.h"

class binary_tree
{
 private:
    tree_elem * m_root;
    int m_size;
    void print_tree(tree_elem *);
    void delete_tree(tree_elem *);

 public:
    binary_tree(int);
    ~binary_tree();
    void print();
    bool find(int);
    void insert(int);
    void erase(int);
    int parent(int);
    int size();
};

#endif // BINARY_TREE_H