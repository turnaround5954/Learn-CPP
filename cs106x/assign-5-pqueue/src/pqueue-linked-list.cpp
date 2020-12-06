#include "pqueue-linked-list.h"
#include "error.h"
using namespace std;

LinkedListPQueue::LinkedListPQueue() {
    head = nullptr;
    tail = nullptr;
}

LinkedListPQueue::~LinkedListPQueue() {
    while (logSize > 0) {
        extractMin();
    }
}

const string &LinkedListPQueue::peek() const {
    // placeholder so method compiles..
    // replace with your own implementation
    if (isEmpty()) {
        error("peek: Attempting to peek at an empty queue");
    }
    return head->data;
}

string LinkedListPQueue::extractMin() {
    // placeholder so method compiles..
    // replace with your own implementation
    string result = peek();
    Cell *cp = head;
    head = cp->next;
    if (head == nullptr) {
        tail = nullptr;
    } else {
        head->prev = nullptr;
    }
    delete cp;
    logSize--;
    return result;
}

void LinkedListPQueue::enqueue(const string &elem) {
    // placeholder so method compiles..
    // replace with your own implementation

    Cell *item = new Cell();
    item->data = elem;

    // empty
    if (head == nullptr) {
        item->prev = nullptr;
        item->next = nullptr;
        head = item;
        tail = item;
        logSize++;
        return;
    }

    // find place to insert after
    Cell *current = head;
    while (current != nullptr && elem >= current->data) {
        current = current->next;
    }

    // insert after tail
    if (current == nullptr) {
        item->prev = tail;
        item->next = nullptr;
        tail->next = item;
        tail = item;
        logSize++;
        return;
    }

    // insert before head
    if (current->prev == nullptr) {
        item->next = current;
        current->prev = item;
        item->prev = nullptr;
        head = item;
        logSize++;
        return;
    }

    // insert between head and tail (before current)
    item->prev = current->prev;
    item->next = current;
    current->prev->next = item;
    current->prev = item;
    logSize++;
}

LinkedListPQueue *LinkedListPQueue::merge(LinkedListPQueue *one,
                                          LinkedListPQueue *two) {
    // placeholder so method compiles..
    // replace with your own implementation
    LinkedListPQueue *pq = new LinkedListPQueue();
    Cell *oneCurrent = one->head;
    Cell *twoCurrent = two->head;
    while (oneCurrent != nullptr && twoCurrent != nullptr) {
        Cell *cp = new Cell();
        if (oneCurrent->data <= twoCurrent->data) {
            cp->data = oneCurrent->data;
            oneCurrent = oneCurrent->next;
        } else {
            cp->data = twoCurrent->data;
            twoCurrent = twoCurrent->next;
        }
        cp->prev = pq->tail;
        cp->next = nullptr;
        if (pq->tail != nullptr) {
            pq->tail->next = cp;
        }
        pq->tail = cp;
        if (pq->head == nullptr) {
            pq->head = cp;
        }
        pq->logSize++;
    }

    while (oneCurrent != nullptr) {
        Cell *cp = new Cell();
        cp->data = oneCurrent->data;
        cp->prev = pq->tail;
        cp->next = nullptr;
        if (pq->tail != nullptr) {
            pq->tail->next = cp;
        }
        pq->tail = cp;
        oneCurrent = oneCurrent->next;
        if (pq->head == nullptr) {
            pq->head = cp;
        }
        pq->logSize++;
    }

    while (twoCurrent != nullptr) {
        Cell *cp = new Cell();
        cp->data = twoCurrent->data;
        cp->prev = pq->tail;
        cp->next = nullptr;
        if (pq->tail != nullptr) {
            pq->tail->next = cp;
        }
        pq->tail = cp;
        twoCurrent = twoCurrent->next;
        if (pq->head == nullptr) {
            pq->head = cp;
        }
        pq->logSize++;
    }

    return pq;
}
