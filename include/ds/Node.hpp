#pragma once
#include <iostream>

template <typename T> class Node {
private:
  T data;
  Node *next;
  Node *prev;

public:
  Node();
  Node(const T &value);
  ~Node();

  // Getter
  T &getData();
  const T &getData() const;
  Node *getNext() const;
  Node *getPrev() const;

  // Setter
  void setData(const T &);
  void setNext(Node<T> *nextValue);
  void setPrev(Node<T> *prevValue);

  // Operators
  bool operator==(Node &value);
  Node &operator=(const Node<T> &value);

  // Display
  void printData();
};

// ================= Implement =================
template <typename T>
Node<T>::Node() : data(T()), next(nullptr), prev(nullptr) {}

template <typename T>
Node<T>::Node(const T &value) : data(value), next(nullptr), prev(nullptr) {}

template <typename T> Node<T>::~Node() {}

template <typename T> T &Node<T>::getData() { return data; }

template <typename T> const T &Node<T>::getData() const { return data; }

template <typename T> Node<T> *Node<T>::getNext() const { return next; }

template <typename T> Node<T> *Node<T>::getPrev() const { return prev; }

template <typename T> void Node<T>::setData(const T &value) { data = value; }

template <typename T> void Node<T>::setNext(Node<T> *nextValue) {
  next = nextValue;
}

template <typename T> void Node<T>::setPrev(Node<T> *prevValue) {
  prev = prevValue;
}

template <typename T> bool Node<T>::operator==(Node<T> &value) {
  return data == value.data;
}

template <typename T> Node<T> &Node<T>::operator=(const Node<T> &value) {
  if (this != &value)
    data = value.data;
  return *this;
}

template <typename T> void Node<T>::printData() {
  if constexpr (std::is_pointer<T>::value)
    std::cout << *data;
  else
    std::cout << data;
}
