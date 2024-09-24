#include "test.h"

#include <atomic>
#include <thread>
#include <iostream>

// Define the MCS lock node structure
struct MCSNode {
    std::atomic<MCSNode*> next{nullptr}; // Pointer to the next node in the queue
    std::atomic<bool> locked{true};      // Locked flag to control spinning
};

// Define the MCS lock class
class MCSLock {
private:
    std::atomic<MCSNode*> tail;  // Atomic pointer to the tail of the queue

public:
    // Constructor that initializes the tail to nullptr
    MCSLock() : tail(nullptr) {}

    // Acquire the lock
    void lock(MCSNode* myNode) {
        myNode->next.store(nullptr, std::memory_order_relaxed);
        MCSNode* prevNode = tail.exchange(myNode, std::memory_order_acquire);  // Atomically set the tail

        if (prevNode != nullptr) {
            prevNode->next.store(myNode, std::memory_order_release); // Link to the previous node
            // Spin until we are unlocked
            while (myNode->locked.load(std::memory_order_acquire)) {
                // Busy wait
            }
        }
        // Lock acquired
    }

    // Release the lock
    void unlock(MCSNode* myNode) {
        MCSNode* successor = myNode->next.load(std::memory_order_acquire);
        if (successor == nullptr) {
            // Try to reset the tail to nullptr
            if (tail.compare_exchange_strong(myNode, nullptr, std::memory_order_release)) {
                return; // No successor, lock is free
            }
            // Wait until the next node is set
            while ((successor = myNode->next.load(std::memory_order_acquire)) == nullptr) {
                // Busy wait
            }
        }
        // Unlock the next node
        successor->locked.store(false, std::memory_order_release);
    }
};

// Example usage with multiple threads
MCSLock lock; // Global MCSLock

void thread_work(int id) {
    MCSNode myNode;

    lock.lock(&myNode);  // Acquire lock

    // Critical section
    std::cout << "Thread " << id << " in critical section\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulate some work

    lock.unlock(&myNode);  // Release lock
}

int main() {
    const int num_threads = 5;
    std::thread threads[num_threads];

    // Create and start threads
    for (int i = 0; i < num_threads; ++i) {
        threads[i] = std::thread(thread_work, i + 1);
    }

    // Join threads
    for (int i = 0; i < num_threads; ++i) {
        threads[i].join();
    }

    return 0;
}
