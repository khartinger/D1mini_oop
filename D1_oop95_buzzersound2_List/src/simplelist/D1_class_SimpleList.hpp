//_____SimpleList.hpp____________________________khartinger_____
//https://stackoverflow.com/questions/9986591/vectors-in-arduino
// Created by Karl Hartinger, June 01, 2021.
// Modified: -
#ifndef SIMPLE_LIST_HPP
#define SIMPLE_LIST_HPP

template <class T>
class ListNode {
 public:
  T element;
  ListNode* next;
  ListNode* prev;
  ListNode(T element, ListNode* prev, ListNode* next) : element(element)
  {
   this->next = next;
   this->prev = prev;
  };
};

template <class T>
class SimpleList {
 private:
  int length;
  ListNode<T>* head;
  ListNode<T>* tail;
  ListNode<T>* curr;
 public:
  SimpleList();
  SimpleList(const SimpleList<T>&);
  ~SimpleList();
  T& getCurrent();
  T& getFirst() const;
  T& getLast() const;
  int  size();
  void append(T);
  void eraseLast();
  void eraseFirst();
  void eraseCurrent();
  bool next();
  bool moveToStart();
  bool prev();
  void erase(T&);
  bool search(T);
  void clear();
  void putFirstToLast();
  bool update(T elemOld, T elemNew);
  SimpleList& operator = (const SimpleList<T>&);
};

template <class T>
SimpleList<T>::SimpleList() {
 length = 0;
 head = nullptr;
 tail = nullptr;
 curr = nullptr;
}

template <class T>
SimpleList<T>::SimpleList(const SimpleList<T> & list) {
 length = 0;
 head = nullptr;
 tail = nullptr;
 curr = nullptr;
 ListNode<T> * temp = list.head;
 while(temp != nullptr)
 {
  append(temp->element);
  temp = temp->next;
 }
}

template <class T>
SimpleList<T> & SimpleList<T>::operator=(const SimpleList<T> & list)
{
 clear();
 ListNode<T> * temp = list.head;
 while(temp != nullptr)
 {
  append(temp->element);
  temp = temp->next;
 }
 return *this;
}

template <class T>
SimpleList<T>::~SimpleList() {
 clear();
}

template<class T>
T& SimpleList<T>::getCurrent()
{
 return curr->element;
}

template<class T>
T& SimpleList<T>::getFirst() const
{
 return head->element;
}

template<class T>
T& SimpleList<T>::getLast() const
{
 return tail->element;
}

template<class T>
int SimpleList<T>::size()
{
 return length;
}

template <class T>
void SimpleList<T>::append(T element)
{
 ListNode<T> * node = new ListNode<T>(element, tail, nullptr);
 if(length == 0)
  curr = tail = head = node;
 else {
  tail->next = node;
  tail = node;
 }
 length++;
}

template <class T>
void SimpleList<T>::eraseLast()
{
 if(length == 0) return;
 curr = tail;
 eraseCurrent();
}

template <class T>
void SimpleList<T>::eraseFirst()
{
 if(length == 0) return;
 curr = head;
 eraseCurrent();
}

template <class T>
bool SimpleList<T>::next()
{
 if(length == 0) return false;
 if(curr->next == nullptr) return false;
 curr = curr->next;
 return true;
}

template <class T>
bool SimpleList<T>::moveToStart()
{
 curr = head;
 return length != 0;
}

template<class T>
bool SimpleList<T>::prev()
{
 if(length == 0) return false;
 if(curr->prev != nullptr) return false;
 curr = curr->prev;
 return true;
}

template <class T>
void SimpleList<T>::erase(T & elem)
{
 if(search(elem)) eraseCurrent();
}

template <class T>
void SimpleList<T>::eraseCurrent()
{
 if(length == 0) return;
 length--;
 ListNode<T> * temp = curr;
 if(temp->prev != nullptr) temp->prev->next = temp->next;
 if(temp->next != nullptr) temp->next->prev = temp->prev;
 if(length == 0) 
  head = curr = tail = nullptr;
 else 
  if(curr == head)
   curr = head = head->next;
  else 
   if(curr == tail)
    curr = tail = tail->prev;
   else
    curr = curr->prev;
 delete temp;
}

template <class T>
bool SimpleList<T>::search(T elem)
{
 if(length == 0) return false;
 if(moveToStart())
 do {
  if(curr->element == elem) return true;
 } while (next());
 return false;
}

template <class T>
void SimpleList<T>::putFirstToLast()
{
 if(length < 2) return;
 ListNode<T>* temp = head->next;
 head->next->prev = nullptr;
 head->next = nullptr;
 head->prev = tail;
 tail->next = head;
 tail = head;
 head = temp;
}

template <class T>
bool SimpleList<T>::update(T elemOld, T elemNew)
{
 bool bRet=search(elemOld);
 if(bRet) curr->element = elemNew;
 return bRet;
}

template <class T>
void SimpleList<T>::clear()
{
 if(length == 0) return;
 ListNode<T> * temp = head;
 while(temp != nullptr)
 {
  head = head->next;
  delete temp;
  temp = head;
 }
 head = curr = tail = nullptr;
 length = 0;
}
#endif
