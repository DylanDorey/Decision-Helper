#ifndef LINKED_LIST_HPP
#define LINKED_LIST_HPP

#include <sstream>
#include <stdexcept>

template <typename T>
class LinkedList
{
    private:

    struct Node
    {
        Node* next;
        T data;

        Node(T d, Node* n) : data(d), next(n){}
    
    }* head_;

    public:

    // Constructor
    LinkedList() : head_(nullptr){}


    // Copy constructor
    LinkedList(LinkedList const &other) : head_(nullptr)
    {
        Node* iter = other.head_;
        while (iter != nullptr)
        {
            pushBack(iter->data);
            iter = iter->next;
        }
    }


    // Assignment operator
    LinkedList & operator=(LinkedList const &rhs)
    {
        if (this != &rhs)
        {
            clear();

            Node* iter = rhs.head_;
            while (iter != nullptr)
            {
                pushBack(iter->data);
                iter = iter->next;
            }
        }

        return *this;
    }

    // Ruturn true if the list is empty, otherwise false
    bool empty()
    {
        return head_ == nullptr;
    }
    

    // Return the number of nodes in the list
    size_t size()
    {
        size_t counter(0);
        Node* iter = head_;
        while (iter != nullptr)
        {
            iter = iter->next;
            ++counter;
        }

        return counter;
    }

    // Create a new node to contain value and insert 
    // the node to the front of the list
    void pushFront(T value)
    {
        head_ = new Node(value, head_);
    }

    // Create a new node to conatin value and insert
    // the node to the back of the list
    void pushBack(T value)
    {
        if (head_ == nullptr)
        {
            pushFront(value);
        }
        else
        {
            Node* iter = head_;
            while (iter->next != nullptr)
            {
                iter = iter->next;
            }

            iter->next = new Node(value, nullptr);
        }
    }

    // Retrieve the node value at the specified list index
    T at(size_t index)
    {
        // Throw exception if index is out of range
        if (index >= size())
        {
            std::ostringstream error;
            error<<"LinkedList::at - List Size: "<<size()<<", Supplied index ("<<index<<") out of range";

            throw std::out_of_range(error.str());
        }

        Node* iter = head_;
        for (size_t i(0); i < index; ++i)
        {
            iter = iter->next;
        }

        return iter->data;
    }

    // Remove the head node in the list such that 
    // the second node in the list should become 
    // the new head node. Then return the previous 
    // head node's value.
    T popFront()
    {
        if (head_ == nullptr)
        {
            throw std::logic_error("LinkedList::popFront - List is empty");
        }

        T returnData = head_->data;

        Node* temp = head_->next;
        delete head_;
        head_ = temp;

        return returnData;
    }

    // Remove the node at the specified list index.
    // Return true if the node at the specified index
    // could be removed, otherwise false.
    bool removeAt(size_t index)
    {
        if (index >= size())
        {
            return false;
        }

        if (index == 0)
        {
            popFront();
        }
        else
        {
            Node* iter = head_;
            for (size_t i(0); i < index-1; ++i)
            {
                iter = iter->next;
            }

            Node* nodeToRemove = iter->next;
            iter->next = nodeToRemove->next;
            delete nodeToRemove;
        }

        return true;
    }




    // Remove all nodes in the list
    void clear()
    {
        while (head_ != nullptr)
        {
            popFront();
        }
    }


    // Destructor
    ~LinkedList()
    {
        clear();
    }




    // Linked list (forward) iterator
    class Iterator
    {
        private:
        Node* current_;

        public:
        
        Iterator(Node* c) : current_(c){}

        // Get address of the current node's data
        T* current()
        {
            if (current_ == nullptr)
            {
                return nullptr;
            }

            return &current_->data;
        }

        // Move to the next node
        void next()
        {
            if (current_ != nullptr)
            {
                current_ = current_->next;
            }
        }

        // Return true if there is a next node, otherwise false
        bool hasNext()
        {
            return current_ != nullptr;
        }

    };


    Iterator generateIterator()
    {
        return Iterator(head_);
    }

};

#endif