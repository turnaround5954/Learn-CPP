#pragma once
#include "pqueue.h"
#include "vector.h"
#include <string>

class BinomialHeapPQueue : public PQueue {
  public:
    BinomialHeapPQueue();
    ~BinomialHeapPQueue();

    static BinomialHeapPQueue *merge(BinomialHeapPQueue *one,
                                     BinomialHeapPQueue *two);

    void enqueue(const std::string &elem);
    std::string extractMin();
    const std::string &peek() const;

  private:
    // provide data methods and helper methods to
    // help realize the binomial heap-backed PQueue
    struct Node {
        std::string data;
        Vector<Node *> children;
    };

    Vector<Node *> TreeVec;
    static void freeTree(Node *root);
    // static Node *mergeTree(Node *one, Node *two);
    static int findMinIdx(const Vector<Node *> &vec);
    static bool mergeTree(Node *&one, Node *two);
    static void mergeTreeVec(Vector<Node *> &one, Vector<Node *> &two);
};
