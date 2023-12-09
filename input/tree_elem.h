#ifndef TREE_ELEM_H
#define TREE_ELEM_H

class tree_elem
{
 public:
     int m_data;
     tree_elem * m_left;
     tree_elem * m_right;
     tree_elem(int val)
     {
         m_left = nullptr;
         m_right = nullptr;
         m_data = val;
     }
};

#endif // TREE_ELEM_H