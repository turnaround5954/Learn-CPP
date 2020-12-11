#include "pqueue-binomial-heap.h"
#include "error.h"
using namespace std;

BinomialHeapPQueue::BinomialHeapPQueue() {}
BinomialHeapPQueue::~BinomialHeapPQueue() {
    for (Node *tree : TreeVec) {
        if (tree != nullptr) {
            freeTree(tree);
        }
    }
}

void BinomialHeapPQueue::freeTree(Node *root) {
    while (!root->children.isEmpty()) {
        Node *child = root->children.removeBack();
        freeTree(child);
    }
    delete root;
}

int BinomialHeapPQueue::findMinIdx(const Vector<Node *> &vec) {
    int minIdx = 0;
    int startIdx = 0;
    while (startIdx < vec.size() && vec[startIdx] == nullptr) {
        startIdx++;
    }
    minIdx = startIdx;
    for (int i = startIdx; i < vec.size(); i++) {
        if (vec[i] != nullptr && vec[i]->data < vec[minIdx]->data) {
            minIdx = i;
        }
    }
    return minIdx;
}

const string &BinomialHeapPQueue::peek() const {
    // placeholder so method compiles..
    // replace with your own implementation
    if (isEmpty()) {
        error("peek: Attempting to peek at an empty queue");
    }
    int minIdx = findMinIdx(TreeVec);
    return TreeVec[minIdx]->data;
}

string BinomialHeapPQueue::extractMin() {
    // placeholder so method compiles..
    // replace with your own implementation
    if (isEmpty()) {
        error("peek: Attempting to peek at an empty queue");
    }
    int minIdx = findMinIdx(TreeVec);
    string minVal = TreeVec[minIdx]->data;
    Vector<Node *> aux = TreeVec[minIdx]->children;
    delete TreeVec[minIdx];
    TreeVec[minIdx] = nullptr;
    mergeTreeVec(TreeVec, aux);
    logSize--;
    return minVal;
}

void BinomialHeapPQueue::enqueue(const string &elem) {
    // placeholder so method compiles..
    // replace with your own implementation
    Vector<Node *> aux;
    aux.add(new Node());
    aux[0]->data = elem;
    mergeTreeVec(TreeVec, aux);
    logSize++;
}

BinomialHeapPQueue *BinomialHeapPQueue::merge(BinomialHeapPQueue *one,
                                              BinomialHeapPQueue *two) {
    // placeholder so method compiles..
    // replace with your own implementation
    BinomialHeapPQueue *pq = new BinomialHeapPQueue();
    pq->logSize = one->logSize + two->logSize;
    pq->TreeVec = one->TreeVec;
    mergeTreeVec(pq->TreeVec, two->TreeVec);
    return pq;
}

void BinomialHeapPQueue::mergeTreeVec(Vector<Node *> &one,
                                      Vector<Node *> &two) {
    int size = max(one.size(), two.size());
    Node *carry = nullptr;
    while (one.size() < size) {
        one.add(nullptr);
    }
    for (int i = 0; i < size; i++) {
        Node *aux = nullptr;
        if (i < two.size() && two[i] != nullptr) {
            aux = two[i];
        }
        if (mergeTree(one[i], aux)) {
            swap(one[i], carry);
        } else {
            if (mergeTree(one[i], carry)) {
                carry = one[i];
                one[i] = nullptr;
            } else {
                carry = nullptr;
            }
        }
    }

    if (carry != nullptr) {
        one.add(carry);
    }
}

bool BinomialHeapPQueue::mergeTree(Node *&one, Node *two) {
    if (one == nullptr || two == nullptr) {
        one = (one == nullptr) ? two : one;
        return false;
    }

    if (one->data > two->data) {
        swap(one, two);
    }
    one->children.add(two);
    return true;
}
