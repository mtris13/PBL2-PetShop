#pragma once
#include "Node.hpp"
#include <iostream>

/*
LƯU Ý: trước mỗi hàm phải có dòng "template <typename T>"

Thứ tự các hàm:
    Constructor
    Destuctor
    Getter: ham const
    Setter
    Overloading operator
    Display
    Others
*/

template <typename T>

class LinkedList {
private:
  Node<T> *head;
  Node<T> *tail;
  int size;

public:
  // Ham dung
  LinkedList() : head(nullptr), size(0), tail(nullptr) {}
  LinkedList(const T &value) : size(1), head(new Node<T>(value)), tail(head) {}
  LinkedList(const LinkedList<T> &other);

  // Ham huy
  ~LinkedList() { clear(); }

  // Getter
  Node<T> *getHead() const;
  Node<T> *getTail() const;
  int getSize() const;
  // Setter

  // Operator

  LinkedList<T> &operator=(const LinkedList<T> &list);
  // Display
  void printLinkedList();

  // Others
  bool isEmpty() const;
  void pushBack(const T &value);
  void pushFront(const T &value);
  void popBack();
  void popFront();
  void insert(int index, const T &data);
  void erase(int index);
  void clear();
  void swap(T &a, T &b);
};

template <typename T>
LinkedList<T>::LinkedList(const LinkedList<T> &other)
    : head(nullptr), tail(nullptr), size(0) { // Khởi tạo danh sách mới rỗng

  Node<T> *current = other.head; // Bắt đầu từ head của list cần copy

  // Dùng while để duyệt hết danh sách
  while (current != nullptr) {
    pushBack(
        current->getData()); // pushBack tự động tạo Node mới và cập nhật tail
    current = current->getNext();
  }
}

// Getter
template <typename T> Node<T> *LinkedList<T>::getHead() const { return head; }

template <typename T> Node<T> *LinkedList<T>::getTail() const { return tail; }

template <typename T> int LinkedList<T>::getSize() const { return size; }

// Setter

// Display
template <typename T> void LinkedList<T>::printLinkedList() {
  Node<T> *temp = head;
  while (temp != nullptr) {
    temp->printData();
    std::cout << " ";
    temp = temp->getNext();
  }
  std::cout << "\n";
}

// Operator
template <typename T>
LinkedList<T> &LinkedList<T>::operator=(const LinkedList<T> &list) {
  if (this == &list) {
    return *this;
  }
  this->clear();
  Node<T> *src = list.head;
  while (src != nullptr) {
    this->pushBack(src->getData());
    // DÒNG 92 SỬA LẠI:
    src = src->getNext();
  }
  return *this;
}

// Others
template <typename T> bool LinkedList<T>::isEmpty() const {
  return head == nullptr;
}

template <typename T> void LinkedList<T>::pushBack(const T &value) {
  Node<T> *newNode = new Node<T>(value);
  if (isEmpty()) {
    head = newNode;
    tail = newNode;
  } else {
    tail->setNext(newNode);
    newNode->setPrev(tail);
    tail = newNode;
  }
  size++;
}

template <typename T> void LinkedList<T>::pushFront(const T &data) {
  Node<T> *newNode = new Node<T>(data);
  if (isEmpty()) {
    head = newNode;
    tail = newNode;
  } else {
    newNode->setNext(head);
    newNode->setPrev(nullptr);
    head->setPrev(newNode);
    head = newNode;
  }
  this->size++;
}

template <typename T> void LinkedList<T>::popBack() {
  if (isEmpty()) {
    std::cout << "List is empty\n";
    return;
  }
  Node<T> *temp = tail;
  if (size == 1) {
    head = nullptr;
    tail = nullptr;
  } else {
    tail = tail->getPrev();
    tail->setNext(nullptr);
  }
  delete temp;
  size--;
}

template <typename T> void LinkedList<T>::popFront() {
  if (isEmpty()) {
    std::cout << "List is empty\n";
    return;
  }
  Node<T> *temp = head;
  head = head->getNext();
  if (head) {
    head->setPrev(nullptr);
  } else {
    tail = nullptr;
  }
  delete temp;
  size--;
}

template <typename T> void LinkedList<T>::insert(int index, const T &data) {
  if (index < 0 || index > size) {
    std::cout << "Index out of range\n";
    return;
  }
  if (isEmpty() && index == 0) {
    pushFront(data);
  } else if (index == size) {
    pushBack(data);
  } else {
    Node<T> *newNode = new Node<T>(data);
    Node<T> *prevNode = head;
    for (int i = 0; i < index - 1; i++) {
      prevNode = prevNode->getNext();
    }
    newNode->setNext(prevNode->getNext());
    newNode->setPrev(prevNode);
    if (prevNode->getNext() != nullptr) {
      prevNode->getNext()->setPrev(newNode);
    }
    prevNode->setNext(newNode);
    this->size++;
  }
}

template <typename T> void LinkedList<T>::erase(int index) {
  if (index < 0 || index >= size) {
    std::cout << "Index out of range\n";
    return;
  }
  if (index == 0) {
    popFront();
  } else if (index == size - 1) {
    popBack();
  } else {
    Node<T> *current = head;
    for (int i = 0; i < index; i++) {
      current = current->getNext();
    }
    current->getPrev()->setNext(current->getNext());
    if (current->getNext() != nullptr) {
      current->getNext()->setPrev(current->getPrev());
    }
    delete current;
    this->size--;
  }
}

template <typename T> void LinkedList<T>::clear() {
  Node<T> *current = head;
  while (current != nullptr) {
    Node<T> *nextNode = current->getNext();
    delete current;
    current = nextNode;
  }
  head = nullptr;
  tail = nullptr;
  size = 0;
}

template <typename T> void LinkedList<T>::swap(T &a, T &b) {
  T temp = a;
  a = b;
  b = temp;
}
