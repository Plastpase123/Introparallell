#ifndef lacpp_sorted_list_cg_hpp
#define lacpp_sorted_list_cg_hpp lacpp_sorted_list_cg_hpp

#include <mutex>
/* a sorted list implementation by David Klaftenegger, 2015
 * please report bugs or suggest improvements to david.klaftenegger@it.uu.se
 */

/* struct for list node_cgs */
template <typename T>
struct node_cg
{
	T value;
	node_cg<T> *next;
};

/* non-concurrent sorted singly-linked list */
template <typename T>
class sorted_list_cg
{
	node_cg<T> *first = nullptr;
	std::mutex m;

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
	sorted_list_cg() = default;
	sorted_list_cg(const sorted_list_cg<T> &other) = default;
	sorted_list_cg(sorted_list_cg<T> &&other) = default;
	sorted_list_cg<T> &operator=(const sorted_list_cg<T> &other) = default;
	sorted_list_cg<T> &operator=(sorted_list_cg<T> &&other) = default;
	~sorted_list_cg()
	{
		while (first != nullptr)
		{
			remove(first->value);
		}
	}
	/* insert v into the list */
	void insert(T v)
	{
		std::lock_guard<std::mutex> lock(m);
		/* first find position */
		node_cg<T> *pred = nullptr;
		node_cg<T> *succ = first;
		while (succ != nullptr && succ->value < v)
		{
			pred = succ;
			succ = succ->next;
		}

		/* construct new node_cg */
		node_cg<T> *current = new node_cg<T>();
		current->value = v;

		/* insert new node_cg between pred and succ */
		current->next = succ;
		if (pred == nullptr)
		{
			first = current;
		}
		else
		{
			pred->next = current;
		}
	}

	void remove(T v)
	{
		std::lock_guard<std::mutex> lock(m);
		/* first find position */
		node_cg<T> *pred = nullptr;
		node_cg<T> *current = first;
		while (current != nullptr && current->value < v)
		{
			pred = current;
			current = current->next;
		}
		if (current == nullptr || current->value != v)
		{
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
	}

	/* count elements with value v in the list */
	std::size_t count(T v)
	{
		std::lock_guard<std::mutex> lock(m);
		std::size_t cnt = 0;
		/* first go to value v */
		node_cg<T> *current = first;
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
		return cnt;
	}
};

#endif // lacpp_sorted_list_cg_hpp
