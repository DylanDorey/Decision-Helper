#ifndef _HASHTABLE_HPP_
#define _HASHTABLE_HPP_

#include <array>
#include "LinkedList.hpp"
using std::string;
using std::pair;


// Templated hash table data type definition
// Uses separate chaining for collision resolution
template <typename T, size_t TABLE_SIZE=11>
class HashTable
{
    private:

    // Array of linked lists to hold all data
    std::array<LinkedList<pair<string, T>>, TABLE_SIZE> table_;

    // Hashing function
    size_t hash(string const &key) const
    {
        size_t sum(0);
        for (char c : key)
        {
            sum += c;
        }
        return sum % TABLE_SIZE;
    }

    // Create alias for linked list iterator to make life easier ;)
    using listIter = typename LinkedList<pair<string, T>>::Iterator;

    public:

    // Add new key/value entry in hash table.
    // Will return true if successfully added, will return
    // false if key value already exists.
    bool add(string const &key, T const &value)
    {
        if (get(key) == nullptr)
        {
            table_.at(hash(key)).pushFront(pair<string, T>(key, value));
            return true;
        }

        return false;
    }

    // Remove entry with matching key from hash table.
    // Do nothing if no entry has matching key.
    void remove(string const &key)
    {
        // Make sure to make a reference or you will be
        // removing item from copy
        LinkedList<pair<string, T>> &vals = table_.at(hash(key));

        size_t counter(0);
        for (listIter iter = vals.generateIterator(); iter.hasNext(); iter.next())
        {
            if (iter.current()->first == key)
            {
                vals.removeAt(counter);
                break;
            }

            ++counter;
        }   
    }

    // Retrieve pointer to value associated with key.
    // Will return null pointer if no matching key is found.  
    T* get(string const &key)
    {
        LinkedList<pair<string, T>> &vals = table_.at(hash(key));

        if (vals.size() > 0)
        {
            for (listIter iter = vals.generateIterator(); iter.hasNext(); iter.next())
            {
                if (iter.current()->first == key)
                {
                    return &iter.current()->second;
                }
            }
        }

        return nullptr;
    }

    // Return the number of entries in hash table
    size_t size()
    {
        size_t size(0);

        for (LinkedList<pair<string, T>> &list : table_)
        {
            for (listIter iter = list.generateIterator(); iter.hasNext(); iter.next())
            {
                ++size;
            }
        }

        return size;
    }

    // Remove all entries from the hash table
    void clear()
    {
        for (LinkedList<pair<string, T>> &list : table_)
        {
            list.clear();
        }
    }



    // HashTable iterator
    class Iterator
    {
        private:
        std::array<LinkedList<pair<string, T>>, TABLE_SIZE>* tablePtr_;
        listIter tableIndexIter_;
        size_t index_;

        public:
        
        // Constructor
        Iterator(std::array<LinkedList<pair<string, T>>, TABLE_SIZE>* tablePtr) 
            : tablePtr_(tablePtr), tableIndexIter_(tablePtr_->at(0).generateIterator()), index_(0)
        {
            while (!tableIndexIter_.hasNext())
            {
                if (++index_ >= TABLE_SIZE)
                {
                    break;
                }

                tableIndexIter_ = tablePtr_->at(index_).generateIterator();
            }
        }

        // Get current value
        pair<string, T>* current()
        {
            return tableIndexIter_.current();
        }

        // Advance the iterator
        void next()
        {
            tableIndexIter_.next();
            if (tableIndexIter_.hasNext())
            {
                return;
            }

            while (++index_ < TABLE_SIZE)
            {
                if (!tablePtr_->at(index_).empty())
                {
                    tableIndexIter_ = tablePtr_->at(index_).generateIterator();
                    break;
                }
            }
        }

        // Check if iterator has reached end of hash table
        bool hasNext()
        {
            size_t tempIndex = index_;
            if (tableIndexIter_.hasNext())
            {
                return true;
            }

            while (++tempIndex < TABLE_SIZE)
            {
                if (!tablePtr_->at(tempIndex).empty())
                {
                    return true;
                }
            }

            return false;
        }
    };

    // Generate iterator for hash table
    Iterator generateIterator()
    {
        return Iterator(&table_);
    }

};

#endif