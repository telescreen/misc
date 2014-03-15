#ifndef __BINARY_TREE_H
#define __BINARY_TREE_H

#include <stdlib.h>
#include <assert.h>

template<typename Key, typename Value>
class BinaryTree {
private:
  struct Node;

public:
  BinaryTree(): root(NULL) {}
  ~BinaryTree() { _freetree(root); }

  bool add(const Key& key, const Value& value) {
    return _add(root, NULL, key, value);
  }

  bool contain(const Key& key) const {
    return _contain(root, key);
  }

  Value& getValue(const Key& key) const {
    return _lookup(root, key);
  }

  bool isEmpty() const { return (root == NULL); }

  void deleteKey(const Key& key) {
    _deletenode(root, key);
  }

  // Iterator over this binary tree
  class Iterator {
  public:
    Iterator(const BinaryTree<Key, Value>& tree_);
    ~Iterator() {}

    bool hasNext() const;
    const Key& key() const;
    Value& value() const;

    void next();
    void prev();

    void seek(const Key& target);

  private:
    const Node* _begin;
    const Node* _end;
  };

private:
  bool _add(Node* &tree, Node *parent, const Key& key, const Value& value);

  bool _contain(Node *tree, const Key& key) const;

  void _freetree(Node *&tree);

  void _deletenode(Node* &tree, const Key& key);

  Value& _lookup(Node *tree, const Key& key) const;

  const Node* getLeftMost() const {
    Node *t = root;
    while(t->left != NULL) t = t->left;
    return t;
  }

  const Node* getRightMost() const {
    Node *t = root;
    while(t->right != NULL) t = t->right;
    return t;
  }

  inline void relink(Node* tree, Node *newnode) {
    if (tree->parent->key > tree->key)
      tree->parent->left = newnode;
    else
      tree->parent->right = newnode;
  }

  Node *root;
  // prevent copy
  BinaryTree(const BinaryTree<Key, Value>&);
};

template<typename Key, typename Value>
BinaryTree<Key, Value>::Iterator::Iterator(const BinaryTree<Key, Value> &tree) {
  _begin = tree->getLeftMost();
  _end = tree->getRightMost();
}

template<typename Key, typename Value>
struct BinaryTree<Key, Value>::Node {
  Node(const Key& key_, const Value& value_)
    : key(key_), value(value_),
      left(NULL), right(NULL), parent(NULL) { }
  Key key;
  Value value;
  Node *left, *right, *parent;
};

template<typename Key, typename Value>
bool BinaryTree<Key, Value>::_add(Node* &tree, Node *parent,
    const Key& key, const Value& value) {
  // if the tree is empty
  if (tree == NULL) {
    tree = new Node(key, value);

    assert(tree != NULL);

    if(parent == NULL) return true;

    assert(parent != NULL);
    if (key > parent->key)
      parent->right = tree;
    else
      parent->left = tree;
    tree->parent = parent;

    return true;
  }

  if (key < tree->key)
    return _add(tree->left, tree, key, value);
  else if (key > tree->key)
    return _add(tree->right, tree, key, value);
  else
    return false;
}

template<typename Key, typename Value>
bool BinaryTree<Key, Value>::_contain(Node *tree,
    const Key& key) const {
  if (tree == NULL) return false;

  if (key < tree->key)
    return _contain(tree->left, key);
  else if (key > tree->key)
    return _contain(tree->right, key);
  else
    return true;
}


template<typename Key, typename Value>
void BinaryTree<Key, Value>::_freetree(Node* &tree) {
  if (tree != NULL) {
    _freetree(tree->left);
    _freetree(tree->right);
    delete tree;
    tree = NULL;
  }
}

template<typename Key, typename Value>
void BinaryTree<Key, Value>::_deletenode(Node* &tree, const Key& key) {
  // found node which need to be deleted
  if (tree->key == key) {
    if (tree->left != NULL && tree->right != NULL) {
      Node *t = tree->left;
      while (t->right != NULL) t = t->right;
      relink(tree, t);
      t->left = tree->left;
      t->right = tree->right;
    } else if (tree->left != NULL || tree->right != NULL) {
      if (tree->left)
        relink(tree, tree->left);
      if (tree->right)
        relink(tree, tree->right);
    }
    delete tree;
    tree = NULL;
  } else if (tree->key > key)
    _deletenode(tree->left, key);
  else
    _deletenode(tree->right, key);
}

template<typename Key, typename Value>
Value& BinaryTree<Key, Value>::_lookup(Node *tree, const Key& key) const {
  if (key == tree->key)
    return tree->value;
  else if (key < tree->key)
    return _lookup(tree->left, key);
  else
    return _lookup(tree->right, key);
}


#endif //__BINARY_TREE_H

