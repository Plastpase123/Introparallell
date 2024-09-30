#ifndef lacpp_sorted_list_fg_tatas_hpp
#define lacpp_sorted_list_fg_tatas_hpp lacpp_sorted_list_fg_tatas_hpp

#include <atomic>
#include <thread>  // for std::this_thread::yield

/* struct for list node_fg_tatass */
template <typename T>
struct node_fg_tatas
{
    T value;
    node_fg_tatas<T> *next;
    std::atomic<bool> lock{false};  // TATAS lock for each node_fg_tatas

    // Helper function to acquire the node_fg_tatas lock
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

    // Helper function to release the node_fg_tatas lock
    void release_lock()
    {
        lock.store(false, std::memory_order_release);  // Release the lock
    }
};

/* non-concurrent sorted singly-linked list with fine-grained TATAS lock */
template <typename T>
class sorted_list_fg_tatas
{
	std::atomic<bool> list_head_lock{false};
	node_fg_tatas<T> *first = nullptr;

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
	sorted_list_fg_tatas() = default;
	sorted_list_fg_tatas(const sorted_list_fg_tatas<T> &other) = default;
	sorted_list_fg_tatas(sorted_list_fg_tatas<T> &&other) = default;
	sorted_list_fg_tatas<T> &operator=(const sorted_list_fg_tatas<T> &other) = default;
	sorted_list_fg_tatas<T> &operator=(sorted_list_fg_tatas<T> &&other) = default;
	~sorted_list_fg_tatas()
	{
		while (first != nullptr)
		{
			remove(first->value);
		}
	}
	/* insert v into the list */
	void insert(T v)
	{
		// lock head in order to acquire the first node_fg_tatass
		acquire_lock(&list_head_lock);
		bool list_head_locked = true;
		/* first find position */
		node_fg_tatas<T> *pred = nullptr;
		node_fg_tatas<T> *succ = first;

		if (succ != nullptr) {
			acquire_lock(&succ->lock);
		}

		while (succ != nullptr && succ->value < v) {
			if (pred != nullptr) {
				release_lock(&pred->lock);
			}

			if (list_head_locked) {
				list_head_locked = false;
				release_lock(&list_head_lock);
			}

			pred = succ;
			succ = succ->next;

			if (succ != nullptr) {
				acquire_lock(&succ->lock);
			}
		}

		/* construct new node_fg_tatas */
		node_fg_tatas<T> *current = new node_fg_tatas<T>();
		current->value = v;
		/* insert new node_fg_tatas between pred and succ */
		current->next = succ;


		if (pred == nullptr) {
			first = current;
		} else {
			pred->next = current;
		}

		if (succ != nullptr) {
			release_lock(&succ->lock);
		}

		if (pred != nullptr) {
			release_lock(&pred->lock);
		}
		if (list_head_locked) {
			list_head_locked = false;
			release_lock(&list_head_lock);
		}
	}

	void remove(T v)
	{
		acquire_lock(&list_head_lock);
		bool list_head_locked = true;
		/* first find position */
		node_fg_tatas<T> *pred = nullptr;
		node_fg_tatas<T> *current = first;

		if (current != nullptr) {
			acquire_lock(&current->lock);
		}

		while (current != nullptr && current->value < v) {
			if (pred != nullptr) {
				release_lock(&pred->lock);
			}

			if (list_head_locked) {
				list_head_locked = false;
				release_lock(&list_head_lock);
			}

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
			if (list_head_locked) {
				list_head_locked = false;
				release_lock(&list_head_lock);
			}
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
		if (pred != nullptr) {
			release_lock(&pred->lock);
		}
		if (list_head_locked) {
			list_head_locked = false;
			release_lock(&list_head_lock);
		}
	}

	/* count elements with value v in the list */
	std::size_t count(T v)
	{
		acquire_lock(&list_head_lock);
		bool list_head_locked = true;
		std::size_t cnt = 0;

		node_fg_tatas<T> *pred = nullptr;
		node_fg_tatas<T> *current = first;

		if (current != nullptr) {
			acquire_lock(&current->lock);
		}

		while (current != nullptr && current->value <= v) {
			if (pred != nullptr) {
				release_lock(&pred->lock);
			}
			if (list_head_locked) {
				list_head_locked = false;
				release_lock(&list_head_lock);
			}
			cnt++;
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
		if (list_head_locked){
			list_head_locked = false;
			release_lock(&list_head_lock);
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

#endif // lacpp_sorted_list_fg_tatas_hpp
