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
	qnode.next.store(nullptr);
    QNode* predNode = tail.exchange(&qnode);

    if (predNode != nullptr) {
		qnode.locked.store(true);
		predNode->next.store(&qnode);
        while (qnode.locked.load()) {}
     }
}

void MCSLock::unlock() {
	QNode* successor = qnode.next.load();
	if (successor == nullptr) {
		QNode* expected = &qnode;
		if (tail.compare_exchange_strong(expected, nullptr)) {
			return;
		}
		while (successor == nullptr){
			successor = qnode.next.load();
		}
	}
	successor->locked.store(false);
	successor->next.store(nullptr);

}



/* struct for list nodes */
template <typename T>

struct node
{
	T value;
	node<T> *next;
};


/* non-concurrent sorted singly-linked list */
template <typename T>
class sorted_list
{
	node<T> *first = nullptr;
	MCSLock lock;

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
		lock.lock();
		/* first find position */
		node<T> *pred = nullptr;
		node<T> *succ = first;

		while (succ != nullptr && succ->value < v) {

			pred = succ;
			succ = succ->next;

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
		lock.unlock();

	}

	void remove(T v)
	{
		lock.lock();
		/* first find position */
		node<T> *pred = nullptr;
		node<T> *current = first;

		while (current != nullptr && current->value < v) {
			pred = current;
			current = current->next;
		}

		if (current == nullptr || current->value != v)
		{
			lock.unlock();
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

		delete current;
		lock.unlock();
	}

	/* count elements with value v in the list */
	std::size_t count(T v)
	{
		lock.lock();
		std::size_t cnt = 0;

		node<T> *pred = nullptr;
		node<T> *current = first;

		while (current != nullptr && current->value < v) {

			pred = current;
			current = current->next;
		}


		/* count elements */
		while (current != nullptr && current->value == v) {
			cnt++;

			pred = current;
			current = current->next;

		}
		lock.unlock();
		return cnt;
	}


};

#endif // lacpp_sorted_list_hpp
