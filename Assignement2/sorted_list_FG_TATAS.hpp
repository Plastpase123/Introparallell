#ifndef lacpp_sorted_list_hpp
#define lacpp_sorted_list_hpp lacpp_sorted_list_hpp

#include <mutex>
#include <iostream>
#include <atomic>
/* a sorted list implementation by David Klaftenegger, 2015
 * please report bugs or suggest improvements to david.klaftenegger@it.uu.se
 */

/* struct for list nodes */
template <typename T>
struct node
{
	T value;
	node<T> *next;
	std::atomic<bool> lock{false};
};

/* non-concurrent sorted singly-linked list */
template <typename T>
class sorted_list
{
	std::atomic<bool> head_lock{false};
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
		// lock head in order to acquire the first nodes
		acquire_lock(&head_lock);
		/* first find position */
		node<T> *pred = nullptr;
		node<T> *succ = first;

		if (succ != nullptr) {
			acquire_lock(&succ->lock);
		}

		while (succ != nullptr && succ->value < v) {
			if (pred != nullptr) {
				release_lock(&pred->lock);
			}

			release_lock(&head_lock);

			pred = succ;
			succ = succ->next;

			if (succ != nullptr) {
				acquire_lock(&succ->lock);
			}
		}

		/* construct new node */
		node<T> *current = new node<T>();
		current->value = v;
		/* insert new node between pred and succ */
		current->next = succ;


		if (pred == nullptr) {
			first = current;
			release_lock(&head_lock);
		} else {
			pred->next = current;
		}

		if (succ != nullptr) {
			release_lock(&succ->lock);
		}

		if (pred != nullptr) {
			release_lock(&pred->lock);
		}
	}

	void remove(T v)
	{
		acquire_lock(&head_lock);
		/* first find position */
		node<T> *pred = nullptr;
		node<T> *current = first;

		if (current != nullptr) {
			acquire_lock(&current->lock);
		}

		while (current != nullptr && current->value < v) {
			if (pred != nullptr) {
				release_lock(&pred->lock);
			}

			release_lock(&head_lock);

			pred = current;
			current = current->next;

			if (current != nullptr) {
				acquire_lock(&current->lock);
			}
		}

		if (current == nullptr || current->value != v) {
			if (current != nullptr) {
				release_lock(&current->lock);
			}
			if (pred != nullptr) {
				release_lock(&pred->lock);
			}
			release_lock(&head_lock);
			/* v not found */
			return;
		}
		/* remove current */
		if (pred == nullptr) {
			first = current->next;
		} else {
			pred->next = current->next;
		}

		delete current;
		release_lock(&head_lock);
		if (pred != nullptr) {
			release_lock(&pred->lock);
		}
	}

	/* count elements with value v in the list */
	std::size_t count(T v)
	{
		acquire_lock(&head_lock);
		std::size_t cnt = 0;

		node<T> *pred = nullptr;
		node<T> *current = first;

		if (current != nullptr) {
			acquire_lock(&current->lock);
		}

		while (current != nullptr && current->value < v) {
			if (pred != nullptr) {
				release_lock(&pred->lock);
			}
			release_lock(&head_lock);
			pred = current;
			current = current->next;

			if (current != nullptr) {
				acquire_lock(&current->lock);
			}
		}
		release_lock(&head_lock);
		/* count elements */
		while (current != nullptr && current->value == v) {
			cnt++;
			if (pred != nullptr) {
				release_lock(&pred->lock);
			}

			pred = current;
			current = current->next;

			if (current != nullptr) {
				acquire_lock(&current->lock);
			}
		}

		if (current != nullptr) {
			release_lock(&current->lock);
		}
		if (pred != nullptr) {

			release_lock(&pred->lock);
		}
		return cnt;
	}

	void acquire_lock(std::atomic<bool> *lock)
	{
		while (true)
		{
			while (lock->load())
			{
				std::this_thread::yield();
			}

			bool expected = false;
			if (lock->compare_exchange_strong(expected, true))
			{
				break;
			}
		}
	}

	void release_lock(std::atomic<bool> *lock)
	{
		lock->store(false);
	}
};

#endif // lacpp_sorted_list_hpp
