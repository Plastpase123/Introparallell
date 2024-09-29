#ifndef lacpp_sorted_list_hpp
#define lacpp_sorted_list_hpp lacpp_sorted_list_hpp

#include <atomic>
#include <thread>  // for std::this_thread::yield

/* struct for list nodes */
template <typename T>
struct node
{
    T value;
    node<T> *next;
    std::atomic<bool> lock{false};  // TATAS lock for each node

    // Helper function to acquire the node lock
    void acquire_lock()
    {
        while (true)
        {
            while (lock.load(std::memory_order_relaxed))
            {
                std::this_thread::yield();  // Yield to allow other threads to run
            }
            // Try to acquire the lock
            bool expected = false;
            if (lock.compare_exchange_strong(expected, true, std::memory_order_acquire))
            {
                break;  // Lock successfully acquired
            }
        }
    }

    // Helper function to release the node lock
    void release_lock()
    {
        lock.store(false, std::memory_order_release);  // Release the lock
    }
};

/* non-concurrent sorted singly-linked list with fine-grained TATAS lock */
template <typename T>
class sorted_list
{
    node<T> *first = nullptr;

public:
    /* default implementations:
     * default constructor, destructor, copy/move constructors, and assignment operators
     */
    sorted_list() = default;
    sorted_list(const sorted_list<T> &other) = default;
    sorted_list(sorted_list<T> &&other) = default;
    sorted_list<T> &operator=(const sorted_list<T> &other) = default;
    sorted_list<T> &operator=(sorted_list<T> &&other) = default;
    ~sorted_list()
    {
        while (first != nullptr)
        {
            remove(first->value);  // Remove all nodes on destruction
        }
    }

    /* insert v into the list */
    void insert(T v)
    {
        node<T> *pred = nullptr;
        node<T> *curr = first;

        // If the list is not empty, lock the first node
        if (curr != nullptr)
        {
            curr->acquire_lock();  
        }

        // Traverse the list to find the right spot for v
        while (curr != nullptr && curr->value < v)
        {
            if (pred != nullptr)
            {
                pred->release_lock();  // Release the previous node's lock
            }

            pred = curr;
            curr = curr->next;

            if (curr != nullptr)
            {
                curr->acquire_lock();  // Lock the next node
            }
        }

        // Insert new node
        node<T> *new_node = new node<T>();
        new_node->value = v;
        new_node->next = curr;

        if (pred == nullptr)  // Insertion at the head
        {
            first = new_node;
        }
        else
        {
            pred->next = new_node;
        }

        // Release locks
        if (pred != nullptr)
        {
            pred->release_lock();
        }
        if (curr != nullptr)
        {
            curr->release_lock();
        }
    }


/* remove v from the list */
void remove(T v)
{
    node<T> *pred = nullptr;
    node<T> *curr = first;

    // If the list is empty, return immediately
    if (curr == nullptr)
    {
        return;
    }

    // Lock the first node (if it exists)
    curr->acquire_lock();

    // Traverse and lock the relevant nodes
    while (curr != nullptr && curr->value < v)
    {
        // Release predecessor's lock (if there was one)
        if (pred != nullptr)
        {
            pred->release_lock();
        }

        // Move forward in the list
        pred = curr;
        curr = curr->next;

        // Only acquire lock on current if it's still in the list
        if (curr != nullptr && curr == pred->next)  // Ensure curr is valid before locking
        {
            curr->acquire_lock();
        }
    }

    // If we found the node with value v, remove it
    if (curr != nullptr && curr->value == v)
    {
        // If pred is null, we're removing the head
        if (pred == nullptr)
        {
            first = curr->next;  // Update the head of the list
        }
        else
        {
            pred->next = curr->next;  // Bypass the current node
        }

        // Release the lock on the current node before deleting it
        curr->release_lock();

        // Now it's safe to delete the node
        delete curr;
        curr = nullptr;  // Set to null to avoid further access
    }

    // Release locks on any remaining nodes
    if (pred != nullptr)
    {
        pred->release_lock();
    }
    if (curr != nullptr)
    {
        curr->release_lock();
    }
}



    /* count elements with value v in the list */
    std::size_t count(T v)
    {
        std::size_t cnt = 0;
        node<T> *curr = first;

        if (curr == nullptr)
        {
            return cnt;  // Empty list, return 0
        }

        curr->acquire_lock();  // Lock the first node

        // Traverse the list to find the value v
        while (curr != nullptr && curr->value < v)
        {
            node<T> *prev = curr;
            curr = curr->next;

            if (curr != nullptr)
            {
                curr->acquire_lock();  // Lock the next node
            }
            prev->release_lock();  // Release the previous node
        }

        // Count occurrences of value v
        while (curr != nullptr && curr->value == v)
        {
            cnt++;
            node<T> *prev = curr;
            curr = curr->next;

            if (curr != nullptr)
            {
                curr->acquire_lock();  // Lock the next node
            }
            prev->release_lock();  // Release the previous node
        }

        // Release the last lock if the list ends
        if (curr != nullptr)
        {
            curr->release_lock();
        }

        return cnt;
    }
};

#endif // lacpp_sorted_list_hpp
