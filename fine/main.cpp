#include <iostream>
#include <mutex>
class FineGrainedQueue {
private:
    struct Node {
        int value;
        Node* next;
        std::mutex node_mutex;

        Node(int v, Node* n = nullptr) : value(v), next(n) {}
    };

    Node* head;
    Node* tail;
    std::mutex queue_mutex;

public:
    FineGrainedQueue() : head(nullptr), tail(nullptr) {}

    ~FineGrainedQueue() {
        std::lock_guard<std::mutex> lock(queue_mutex);
        while (head) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
    }
    void insertIntoMiddle(int value, int pos)
    {
        Node* new_node = new Node(value);

        std::unique_lock<std::mutex> queue_lock(queue_mutex);

        if (pos <= 0) {  // insert at beginning
            new_node->next = head;
            head = new_node;
            if (!tail) {  // queue was empty
                tail = new_node;
            }
            return;
        }

        // find the node at position pos-1
        Node* curr = head;
        int count = 0;
        while (curr && count < pos - 1) {
            std::unique_lock<std::mutex> node_lock(curr->node_mutex);
            curr = curr->next;
            count++;
        }

        if (curr) {  // insert after the node at position pos-1
            std::unique_lock<std::mutex> node_lock(curr->node_mutex);
            Node* next = curr->next;
            curr->next = new_node;
            new_node->next = next;
            if (!next) {  // insert at end
                tail = new_node;
            }
        }
        else {  // pos is out of range, insert at end
            if (!tail) {  // queue was empty
                head = tail = new_node;
            }
            else {
                tail->next = new_node;
                tail = new_node;
            }
        }
    }
    void enqueue(int value) {
        Node* newNode = new Node(value);
        std::lock_guard<std::mutex> lock(queue_mutex);
        if (!tail) {
            head = tail = newNode;
        }
        else {
            tail->next = newNode;
            tail = newNode;
        }
    }

    int dequeue() {
        std::lock_guard<std::mutex> lock(queue_mutex);
        if (!head) {
            return false;
        }
        const int value = head->value;
        Node* temp = head;
        head = head->next;
        delete temp;
        if (!head) {
            tail = nullptr;
        }
        return value;
    }

    bool empty() {
        std::lock_guard<std::mutex> lock(queue_mutex);
        return !head;
    }
};


int main() {
    FineGrainedQueue queue;

    queue.enqueue(1);
    queue.enqueue(2);
    queue.enqueue(3);

    queue.insertIntoMiddle(0, -1);
    queue.insertIntoMiddle(0, 55);
    queue.insertIntoMiddle(0, 2);

    while (!queue.empty()) {
        std::cout << queue.dequeue() << std::endl;
    }

    return 0;
}


