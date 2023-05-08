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
        while (head) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
    }
    void insertIntoMiddle(int value, int pos)
    {
        if (pos < 0)
            return;
        Node* new_node = new Node(value);

        std::unique_lock<std::mutex> queue_lock(queue_mutex);

        Node* curr = head;
        int count = 0;

        while (curr && count < pos - 1) {
            std::unique_lock<std::mutex> node_lock((curr->node_mutex));
            curr = curr->next;
            count++;
        }

        if (curr && count == pos - 1) {
            std::unique_lock<std::mutex> node_lock((curr->node_mutex));
            Node* next = curr->next;
            curr->next = new_node;
            new_node->next = next;
        }
        else {
            delete new_node;
            throw std::out_of_range("Invalid position");
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

    queue.insertIntoMiddle(66, 2);

    while (!queue.empty()) {
        std::cout << queue.dequeue() << std::endl;
    }

    return 0;
}


