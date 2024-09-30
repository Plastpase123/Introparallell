#ifndef lacpp_sorted_list_hpp
#define lacpp_sorted_list_hpp lacpp_sorted_list_hpp
#include <atomic>
/* a sorted list implementation by David Klaftenegger, 2015
 * please report bugs or suggest improvements to david.klaftenegger@it.uu.se
 */

/* struct for list node_cg_tatass */
template <typename T>
struct node_cg_tatas
{
	T value;
	node_cg_tatas<T> *next;
};

/* non-concurrent sorted singly-linked list */
template <typename T>
class sorted_list_cg_tatas
{
	node_cg_tatas<T> *first = nullptr;
	std::atomic<bool> m{false};

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
	sorted_list_cg_tatas() = default;
	sorted_list_cg_tatas(const sorted_list_cg_tatas<T> &other) = default;
	sorted_list_cg_tatas(sorted_list_cg_tatas<T> &&other) = default;
	sorted_list_cg_tatas<T> &operator=(const sorted_list_cg_tatas<T> &other) = default;
	sorted_list_cg_tatas<T> &operator=(sorted_list_cg_tatas<T> &&other) = default;
	~sorted_list_cg_tatas()
	{
		while (first != nullptr)
		{
			remove(first->value);
		}
	}
	/* insert v into the list */
	void insert(T v)
	{
		acquire_lock();
		/* first find position */
		node_cg_tatas<T> *pred = nullptr;
		node_cg_tatas<T> *succ = first;
		while (succ != nullptr && succ->value < v)
		{
			pred = succ;
			succ = succ->next;
		}

		/* construct new node_cg_tatas */
		node_cg_tatas<T> *current = new node_cg_tatas<T>();
		current->value = v;

		/* insert new node_cg_tatas between pred and succ */
		current->next = succ;
		if (pred == nullptr)
		{
			first = current;
		}
		else
		{
			pred->next = current;
		}
		release_lock();
	}

	void remove(T v)
	{
		acquire_lock();
		/* first find position */
		node_cg_tatas<T> *pred = nullptr;
		node_cg_tatas<T> *current = first;
		while (current != nullptr && current->value < v)
		{
			pred = current;
			current = current->next;
		}
		if (current == nullptr || current->value != v)
		{
			/* v not found */
			release_lock();
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
		release_lock();
	}

	/* count elements with value v in the list */
	std::size_t count(T v)
	{
		acquire_lock();
		std::size_t cnt = 0;
		/* first go to value v */
		node_cg_tatas<T> *current = first;
		while (current != nullptr && current->value < v)
		{
			current = current->next;
		}
		/* count elements */
		while (current != nullptr && current->value == v)
		{
			cnt++;
			current = current->next;
		}
		release_lock();
		return cnt;
	}

	void acquire_lock()
	{
		while (true)
		{
			while (m.load())
			{
				std::this_thread::yield();
			}
			bool expected = false;
			if (m.compare_exchange_strong(expected, true))
			{
				break;
			}
		}
	}

	void release_lock()
	{
		m.store(false);
	}
};

#endif // lacpp_sorted_list_cg_tatas_hpp
