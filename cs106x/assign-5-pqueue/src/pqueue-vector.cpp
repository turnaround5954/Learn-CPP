#include "pqueue-vector.h"
#include "error.h"
using namespace std;

VectorPQueue::VectorPQueue() {}
VectorPQueue::~VectorPQueue() {}

const string &VectorPQueue::peek() const {
    // placeholder so method compiles..
    // replace with your own implementation
    // return kEmptyString;
    if (isEmpty()) {
        error("peek: Attempting to peek at an empty queue");
    }
    int minIndex = 0;
    for (int i = 0; i < queueVector.size(); i++) {
        if (queueVector[minIndex] > queueVector[i]) {
            minIndex = i;
        }
    }
    return queueVector[minIndex];
}

string VectorPQueue::extractMin() {
    // placeholder so method compiles..
    // replace with your own implementation
    string smallest = peek();
    queueVector.removeValue(smallest);
    logSize--;
    return smallest;
}

void VectorPQueue::enqueue(const string &elem) {
    // placeholder so method compiles..
    // replace with your own implementation
    queueVector.add(elem);
    logSize++;
}

VectorPQueue *VectorPQueue::merge(VectorPQueue *one, VectorPQueue *two) {
    // placeholder so method compiles..
    // replace with your own implementation
    VectorPQueue *pq = new VectorPQueue();
    pq->logSize = one->size() + two->size();
    pq->queueVector = one->queueVector + two->queueVector;
    return pq;
}
