#ifndef lacpp_sorted_list_fg_hpp
#define lacpp_sorted_list_fg_hpp lacpp_sorted_list_fg_hpp

#include <mutex>
#include <iostream>
/* a sorted list implementation by David Klaftenegger, 2015
 * please report bugs or suggest improvements to david.klaftenegger@it.uu.se
 */

/* struct for list node_fgs */
template <typename T>
struct node_fg
{
	T value;
	node_fg<T> *next;
	std::mutex lock;
};

/* non-concurrent sorted singly-linked list */
template <typename T>
class sorted_list_fg
{
	node_fg<T> *first = nullptr;

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
	sorted_list_fg() = default;
	sorted_list_fg(const sorted_list_fg<T> &other) = default;
	sorted_list_fg(sorted_list_fg<T> &&other) = default;
	sorted_list_fg<T> &operator=(const sorted_list_fg<T> &other) = default;
	sorted_list_fg<T> &operator=(sorted_list_fg<T> &&other) = default;
	~sorted_list_fg()
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
		node_fg<T> *pred = nullptr;
		node_fg<T> *succ = first;

		if (succ != nullptr)
		{
			succ->lock.lock();
		}

		while (succ != nullptr && succ->value < v)
		{
			if (pred != nullptr)
			{
				pred->lock.unlock();
			}

			pred = succ;
			succ = succ->next;

			if (succ != nullptr)
			{
				succ->lock.lock();
			}
		}

		/* construct new node_fg */
		node_fg<T> *current = new node_fg<T>();
		current->value = v;
		/* insert new node_fg between pred and succ */
		current->next = succ;

		if (pred == nullptr)
		{
			first = current;
		}
		else
		{
			pred->next = current;
		}
		if (succ != nullptr)
		{
			succ->lock.unlock();
		}
		if (pred != nullptr)
		{
			pred->lock.unlock();
		}
	}

	void remove(T v)
	{
		/* first find position */
		node_fg<T> *pred = nullptr;
		node_fg<T> *current = first;

		if (current != nullptr)
		{
			current->lock.lock();
		}

		while (current != nullptr && current->value < v)
		{
			if (pred != nullptr)
			{
				pred->lock.unlock();
			}

			pred = current;
			current = current->next;

			if (current != nullptr)
			{
				current->lock.lock();
			}
		}

		if (current == nullptr || current->value != v)
		{
			if (current != nullptr)
			{
				current->lock.unlock();
			}
			if (pred != nullptr)
			{
				pred->lock.unlock();
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
		current->lock.unlock();
		delete current;

		if (pred != nullptr)
		{
			pred->lock.unlock();
		}
	}

	/* count elements with value v in the list */
	std::size_t count(T v)
	{
		std::size_t cnt = 0;

		node_fg<T> *pred = nullptr;
		node_fg<T> *current = first;

		if (current != nullptr)
		{
			current->lock.lock();
		}

		while (current != nullptr && current->value < v)
		{
			if (pred != nullptr)
			{
				pred->lock.unlock();
			}

			pred = current;
			current = current->next;

			if (current != nullptr)
			{
				current->lock.lock();
			}
		}

		/* count elements */
		while (current != nullptr && current->value == v)
		{
			cnt++;
			if (pred != nullptr)
			{
				pred->lock.unlock();
			}
			pred = current;
			current = current->next;
			if (current != nullptr)
			{
				current->lock.lock();
			}
		}

		if (current != nullptr)
		{
			current->lock.unlock();
		}
		if (pred != nullptr)
		{
			pred->lock.unlock();
		}
		return cnt;
	}
};

#endif // lacpp_sorted_list_fg_hpp
