#include "pqueue-heap.h"
#include "error.h"
using namespace std;

HeapPQueue::HeapPQueue() {
    capacity = 4;
    heap = new string[capacity];
}

HeapPQueue::~HeapPQueue() {
    delete[] heap;
    heap = nullptr;
}

void HeapPQueue::resize(const int &capacity) {
    string *tmp = new string[capacity];
    for (int i = 0; i < logSize; i++) {
        tmp[i] = heap[i];
    }
    this->capacity = capacity;
    delete[] heap;
    heap = tmp;
}

const string &HeapPQueue::peek() const {
    // placeholder so method compiles..
    // replace with your own implementation
    if (isEmpty()) {
        error("peek: Attempting to peek at an empty queue");
    }
    return heap[0];
}

string HeapPQueue::extractMin() {
    // placeholder so method compiles..
    // replace with your own implementation
    string minVal = peek();
    heap[0] = heap[--logSize];

    // bubble down (sink)
    int parent = 1;
    while (true) {
        int child = parent * 2;
        // reach leaf
        if (child > logSize) {
            break;
        }
        // find the minimum child
        if (child + 1 <= logSize && heap[child] < heap[child - 1]) {
            child++;
        }
        // swap if root is bigger than the child otherwise break
        if (heap[parent - 1] > heap[child - 1]) {
            swap(heap[parent - 1], heap[child - 1]);
            parent = child;
        } else {
            break;
        }
    }

    // shrink size of array if necessary.
    if (logSize > 0 && logSize == capacity / 4) {
        resize(capacity / 2);
    }

    return minVal;
}

void HeapPQueue::enqueue(const string &elem) {
    // placeholder so method compiles..
    // replace with your own implementation
    heap[logSize++] = elem;

    // bubble up (swim)
    int child = logSize;
    while (child > 1) {
        int parent = child / 2;
        if (heap[child - 1] < heap[parent - 1]) {
            swap(heap[child - 1], heap[parent - 1]);
            child = parent;
        } else {
            break;
        }
    }

    // double size of array if necessary
    if (logSize == capacity) {
        resize(2 * capacity);
    }
}

void HeapPQueue::heapify(HeapPQueue *pq, int root) {
    // base case
    int child = root * 2;
    if (child > pq->logSize) {
        return;
    }

    // find the minimum child
    if (child + 1 <= pq->logSize && pq->heap[child] < pq->heap[child - 1]) {
        child++;
    }
    // swap the child with root
    if (pq->heap[root - 1] > pq->heap[child - 1]) {
        swap(pq->heap[root - 1], pq->heap[child - 1]);
        // sink
        heapify(pq, child);
    }
}

HeapPQueue *HeapPQueue::merge(HeapPQueue *one, HeapPQueue *two) {
    // placeholder so method compiles..
    // replace with your own implementation

    // concatenate one and two
    HeapPQueue *pq = new HeapPQueue();
    delete[] pq->heap;
    pq->logSize = one->logSize + two->logSize;
    pq->capacity = pq->logSize * 2;
    pq->heap = new string[pq->capacity];
    for (int i = 0; i < one->logSize; i++) {
        pq->heap[i] = one->heap[i];
    }
    for (int i = one->logSize; i < pq->logSize; i++) {
        pq->heap[i] = two->heap[i - one->logSize];
    }

    // heapify
    for (int i = pq->logSize / 2; i >= 1; i--) {
        heapify(pq, i);
    }
    return pq;
}
