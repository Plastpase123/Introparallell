#ifndef lacpp_sorted_list_hpp
#define lacpp_sorted_list_hpp lacpp_sorted_list_hpp
#include <atomic>
#include <iostream>
/* a sorted list implementation by David Klaftenegger, 2015
 * please report bugs or suggest improvements to david.klaftenegger@it.uu.se
 */

// MCS Lock node structure
    struct QNode {
        std::atomic<bool> locked{true};
        std::atomic<QNode*> next{nullptr};
    };

// MCS Lock class for fine-grained locking
    class MCSLock {
    std::atomic<QNode*> tail{nullptr};

    public:

        void lock(QNode* qnode) {
            qnode->next.store(nullptr, std::memory_order_relaxed);
			qnode->locked.store(true, std::memory_order_relaxed);
            QNode* predNode = tail.exchange(qnode, std::memory_order_acquire);

            if (predNode != nullptr) {
                predNode->next.store(qnode, std::memory_order_release);
				//qnode->locked.store(true, std::memory_order_relaxed);
                while (qnode->locked.load(std::memory_order_acquire)) {
                    // Busy wait
                }
            }
        }

        void unlock(QNode* qnode) {
            QNode* successor = qnode->next.load(std::memory_order_acquire);
            if (successor == nullptr) {
                if (tail.compare_exchange_strong(qnode, nullptr, std::memory_order_release)) {
                    return;
                }
                while (qnode->next.load(std::memory_order_acquire) == nullptr) {
                    // Busy wait
                }
            }
            successor->locked.store(false, std::memory_order_release);
        }
    };


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

		QNode predQNode, succQNode;


		if (succ != nullptr) {
			// Lock current node if non-empty
			succ->lock.lock(&succQNode);
		}

		while (succ != nullptr && succ->value < v)
		{
			if (pred != nullptr) {
				pred->lock.unlock(&predQNode);
			}

			succ->lock.unlock(&succQNode);
			pred = succ;
			pred->lock.lock(&predQNode);


			succ = succ->next;

			if (succ != nullptr) {
				succ->lock.lock(&succQNode);
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

		// Unlock the remaining nodes
        if (succ != nullptr) {
            succ->lock.unlock(&succQNode);
        }
        if (pred != nullptr) {
            pred->lock.unlock(&predQNode);
        }

	}

	void remove(T v)
	{
		/* first find position */
		node<T> *pred = nullptr;
		node<T> *current = first;

		QNode currentQNode, predQNode;


		if (current != nullptr) {
			// Lock current node if non-empty
			current->lock.lock(&currentQNode);
		}

		while (current != nullptr && current->value < v)
		{
			if (pred != nullptr) {
				pred->lock.unlock(&predQNode);
			}
			current->lock.unlock(&currentQNode);

			pred = current;
			pred->lock.lock(&predQNode);


			current = current->next;

			if (current != nullptr) {
				current->lock.lock(&currentQNode);
			}

		}

		if (current == nullptr || current->value != v)
		{
			/* v not found */
			if (pred != nullptr) {
				pred->lock.unlock(&predQNode);
			}
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
		current->lock.unlock(&currentQNode);
		delete current;

		if (pred != nullptr) {
			pred->lock.unlock(&predQNode);
		}
	}

	/* count elements with value v in the list */
	std::size_t count(T v)
	{
		std::size_t cnt = 0;
		/* first go to value v */
		node<T> *current = first;
		node<T> *pred = nullptr;

		QNode currentQNode, predQNode;

		if (current != nullptr) {
			// Lock current node if non-empty
			current->lock.lock(&currentQNode);
		}

		while (current != nullptr && current->value < v)
		{
			if (pred != nullptr) {
				pred->lock.unlock(&predQNode);
			}
			std::cout << "unlocking current \n";

			current->lock.unlock(&currentQNode);

			std::cout << "locking pred\n";

			pred = current;
			pred->lock.lock(&predQNode);


			current = current->next;

			if (current != nullptr) {
				current->lock.lock(&currentQNode);
			}

		}

		/* count elements */
		while (current != nullptr && current->value == v)
		{
			cnt++;

			if (pred != nullptr) {
				pred->lock.unlock(&predQNode);
			}

			pred = current;
			std::cout << "locking pred2\n";
			pred->lock.lock(&predQNode);

			std::cout << "unlocking current2\n";
			current->lock.unlock(&currentQNode);
			current = current->next;

			if (current != nullptr) {
				// Lock current node if non-empty
				current->lock.lock(&currentQNode);
			}
		}

		if (current != nullptr) {
			current->lock.unlock(&currentQNode);
		}

		if (pred != nullptr) {
			pred->lock.unlock(&predQNode);
		}

		return cnt;
	}
};

#endif // lacpp_sorted_list_hpp
