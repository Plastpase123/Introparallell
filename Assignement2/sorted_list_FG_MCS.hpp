#ifndef lacpp_sorted_list_hpp
#define lacpp_sorted_list_hpp lacpp_sorted_list_hpp
#include <atomic>
#include <iostream>
#include <thread>
/* a sorted list implementation by David Klaftenegger, 2015
 * please report bugs or suggest improvements to david.klaftenegger@it.uu.se
 */


// MCS Lock node structure
struct QNode {
	bool locked = false;
    QNode* next = nullptr;
};


// MCS Lock class for fine-grained locking
class MCSLock {
    std::atomic<QNode*> tail;
	static thread_local QNode qnode;

    public:
        void lock() {
            QNode* predNode = tail.exchange(&qnode, std::memory_order_acquire);

            if (predNode != nullptr) {
				qnode.locked = true;
                predNode->next = &qnode;
                while (qnode.locked) {}
            }
        }

        void unlock() {
            QNode* successor = qnode.next;
            if (successor == nullptr) {
				QNode* expected = &qnode;
                if (tail.compare_exchange_strong(expected, nullptr, std::memory_order_release)) {
                    return;
                }
                while (successor == nullptr){}
            }
            successor->locked = false;
			qnode.next = nullptr;

        }
    };

thread_local QNode MCSLock::qnode;

/* struct for list nodes */
template <typename T>

struct node
{
	T value;
	node<T> *next;
	MCSLock lock;
};




/* non-concurrent sorted singly-linked list */
template <typename T>
class sorted_list
{
	node<T> *first = nullptr;

public:
	/* default implementations:
	 * default constructor
	 * copy constructor (note: shallow copy)
	 * move constructor
	 * copy assignment operator (note: shallow copy)
	 * move assignment operator
	 *
	 * The first is required due to the others,
	 * which are explicitly listed due to the rule of five.
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
			remove(first->value);
		}
	}
	/* insert v into the list */
	void insert(T v)
	{
		/* first find position */
		node<T> *pred = nullptr;
		node<T> *succ = first;

		if (succ != nullptr) {
			succ->lock.lock();
		}

		while (succ != nullptr && succ->value < v)
		{

			if (pred != nullptr) {
				pred->lock.unlock();
			}

			pred = succ;
			succ = succ->next;

			if (succ != nullptr) {
				succ->lock.lock();
			}

		}

		/* construct new node */
		node<T> *current = new node<T>();
		current->value = v;

		/* insert new node between pred and succ */
		current->next = succ;
		if (pred == nullptr)
		{
			first = current;
		}
		else
		{
			pred->next = current;
		}

		if (succ != nullptr) {
			succ->lock.unlock();
		}

		if (pred != nullptr) {
			pred->lock.unlock();
		}


	}

	void remove(T v)
	{
		/* first find position */
		node<T> *pred = nullptr;
		node<T> *current = first;

		if (current != nullptr) {
			current->lock.lock();
		}
		while (current != nullptr && current->value < v)
		{
			if (pred != nullptr) {
				pred->lock.unlock();
			}

			pred = current;
			current = current->next;

			if (current != nullptr) {
				current->lock.lock();
			}
		}
		if (current == nullptr || current->value != v)
		{
			if (pred != nullptr) {
				pred->lock.unlock();
			}
			if (current != nullptr) {
				current->lock.unlock();
			}
			/* v not found */
			return;
		}
		/* remove current */
		if (pred == nullptr)
		{
			first = current->next;
		}
		else
		{
			pred->next = current->next;
		}
		delete current;

		if (pred != nullptr){
			pred->lock.unlock();
		}
	}

	/* count elements with value v in the list */
	std::size_t count(T v)
	{
		std::size_t cnt = 0;

		node<T> *pred = nullptr;
		node<T> *current = first;

		if (current != nullptr) {
			current->lock.lock();
		}

		while (current != nullptr && current->value < v)
		{
			if (pred != nullptr){
				pred->lock.unlock();
			}

			pred = current;
			current = current->next;

			if (current != nullptr) {
				current->lock.lock();
			}
		}
		/* count elements */
		while (current != nullptr && current->value == v)
		{
			cnt++;
			if (pred != nullptr){
				pred->lock.unlock();
			}

			pred = current;
			current = current->next;

			if (current != nullptr) {
				current->lock.lock();
			}
		}

		if (current != nullptr) {
			current->lock.unlock();
		} if (pred != nullptr) {
			pred->lock.unlock();
		}
		return cnt;
	}


};

#endif // lacpp_sorted_list_hpp
