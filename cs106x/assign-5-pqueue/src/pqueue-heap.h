#pragma once
#include "pqueue.h"
#include <string>

class HeapPQueue : public PQueue {
  public:
    HeapPQueue();
    ~HeapPQueue();

    static HeapPQueue *merge(HeapPQueue *one, HeapPQueue *two);

    void enqueue(const std::string &elem);
    std::string extractMin();
    const std::string &peek() const;

  private:
    // provide data methods and helper methods to
    // help realize the binary heap-backed PQueue
    int capacity;
    std::string *heap;
    void resize(const int &capacity);
    static void heapify(HeapPQueue *pq, int root);
};
