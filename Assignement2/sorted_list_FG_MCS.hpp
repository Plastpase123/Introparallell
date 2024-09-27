#ifndef lacpp_sorted_list_hpp
#define lacpp_sorted_list_hpp lacpp_sorted_list_hpp
#include <atomic>
#include <iostream>
#include <thread>
#include "mcs_lock.hpp"
/* a sorted list implementation by David Klaftenegger, 2015
 * please report bugs or suggest improvements to david.klaftenegger@it.uu.se
 */

thread_local MCSLock::QNode MCSLock::qnode;

void MCSLock::lock() {
	qnode.next.store(nullptr, std::memory_order_release);
    QNode* predNode = tail.exchange(&qnode, std::memory_order_relaxed);

    if (predNode != nullptr) {
		qnode.locked.store(true, std::memory_order_release);
		predNode->next.store(&qnode, std::memory_order_release);
        while (qnode.locked.load(std::memory_order_acquire)) {}
     }
}

void MCSLock::unlock() {
	QNode* successor = qnode.next.load(std::memory_order_acquire);
	if (successor == nullptr) {
		QNode* expected = &qnode;
		if (tail.compare_exchange_strong(expected, nullptr, std::memory_order_release, std::memory_order_relaxed)) {
			return;
		}
		while (successor == nullptr){
			successor = qnode.next.load(std::memory_order_acquire);
		}
	}
	successor->locked.store(false, std::memory_order_release);
	successor->next.store(nullptr, std::memory_order_release);

}



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
			std::cout << "I: locking first with value " << succ->value << "\n";
			succ->lock.lock();
		}

		while (succ != nullptr && succ->value < v) {
			if (pred != nullptr) {
				std::cout << "I: unlocking pred in loop with value: " << pred->value << "\n";
				pred->lock.unlock();
			}

			pred = succ;
			succ = succ->next;

			if (succ != nullptr) {
				std::cout << "I: locking succ in loop with value: " << succ->value << "\n";
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
			std::cout << "I: unlocking last succ with value " << succ->value << "\n";
			succ->lock.unlock();
		} if (pred != nullptr) {
			std::cout << "I: unlocking last pred with value " << pred->value << "\n";
			pred->lock.unlock();
		}

	}

	void remove(T v)
	{
		/* first find position */
		node<T> *pred = nullptr;
		node<T> *current = first;

		if (current != nullptr) {
			std::cout << "R: locking first with value " << current->value << "\n";
			current->lock.lock();
		}

		while (current != nullptr && current->value < v) {
			if (pred != nullptr) {
				std::cout << "R: unlocking pred in loop with value " << pred->value << "\n";
				pred->lock.unlock();
			}

			pred = current;
			current = current->next;

			if (current != nullptr) {
				std::cout << "R: locking current in loop with value " << current->value << "\n";

				current->lock.lock();
			}
		}


		if (current == nullptr || current->value != v)
		{
			if (current != nullptr) {
				std::cout << "R: unlocking early return current with value " << current->value << "\n";

				current->lock.unlock();
			} if (pred != nullptr) {
				std::cout << "R: unlocking early return pred with value " << pred->value << "\n";

				pred->lock.unlock();
			}
			/* v not found */
			return;
		}
		/* remove current */
		if (pred == nullptr)
		{
			first = current->next;
		} else {
			pred->next = current->next;
		}

		current->lock.unlock();
		delete current;

		if (pred != nullptr) {
			std::cout << "R: last unlocking pred with value " << pred->value << "\n";

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
			std::cout << "C: locking first with value " << current->value << "\n";
			current->lock.lock();
		}

		while (current != nullptr && current->value < v) {
			if (current->next != nullptr) {
				std::cout << "C: locking current in loop 1 with value: " << current->value << "\n";
				current->next->lock.lock();
			}
			if (pred != nullptr) {
				std::cout << "C: unlocking pred in loop 1 with value: " << pred->value << "\n";
				pred->lock.unlock();
			}

			pred = current;
			current = current->next;
		}


		/* count elements */
		while (current != nullptr && current->value == v) {
			cnt++;
			if (pred != nullptr) {
				std::cout << "C: unlocking pred in loop 2 with value " << pred->value << "\n";
				pred->lock.unlock();
			}
			pred = current;
			current = current->next;
			if (current != nullptr) {
				std::cout << "C: locking current in loop 2 with value " << current->value << "\n";
				current->lock.lock();
			}
		}
		if (current != nullptr) {
			std::cout << "C: last unlocking current with value " << current->value << "\n";
			current->lock.unlock();
		} if (pred != nullptr) {
			std::cout << "C: last unlocking pred with value " << pred->value << "\n";
			pred->lock.unlock();
		}
		return cnt;
	}


};

#endif // lacpp_sorted_list_hpp
