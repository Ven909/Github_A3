//FILE: sequence.cpp
//CLASS IMPLEMENTED: sequence (see sequence.h for documentation)
//INVARIANT for the sequence class
//  1. The number of items in the sequence is stored in the member variable "many_nodes"
//
//  2. For an empty sequence, we do not care what is stored in any of data and head_ptr
//      and tail_ptr point to NULL; for a non-empty sequence, the items stored in the
//      sequence can be accessed using head_ptr and tail_ptr and we don't care what is stored in the rest of the data.
//
//  3. If there is a current item, then it lies in *cursor; if there is no
//     current item, then cursor* equals NULL.
//
//  4. If there is a previous item, then it lies in precursor*.  If there is no previous
//       item, then precursor equals NULL.

#include <iostream>
#include <algorithm>//Provides copy function
#include <cassert>//For assert function
#include <fstream>//For files
//#include "sequence4.h"//Header file for class
#include "node2.h" // Provides node class

using namespace std;//For copy function

namespace scu_coen79_8B
{
    // Default private member variable initialization function.
    template <class Item>
    void sequence<Item>::init()
    {
        //Initializing pointers to NULL
        head_ptr = NULL;
        tail_ptr = NULL;
        cursor = NULL;
        precursor = NULL;
        //Initializing many_nodes (or our counter) to 0
        many_nodes = 0;
    }

    //CONSTRUCTOR IMPLEMENTATION for default constructor
    template <class Item>
    sequence<Item> :: sequence ()
    {
        init();
    }

    //Copy Constructor
    template <class Item>
    sequence<Item> :: sequence(const sequence& source)
    {
        init();
        *this = source;
    }

    //Destructor
    template <class Item>
    sequence<Item>:: ~sequence()
    {
        list_clear(head_ptr);//Returning data to the freestore
        many_nodes = 0;

        return;
    }

    template <class Item>
    void sequence<Item> :: start()
    {
        cursor = head_ptr;
        precursor = NULL;

        return;
    }

    template <class Item>
    void sequence<Item> :: advance()
    {
        //Testing that is_item returns true
        assert(is_item());

        precursor = cursor;
        cursor = cursor -> link();

        return;
    }

    //Inserting data before the current index value
    template <class Item>
    void sequence<Item> :: insert(const Item& entry)
    {
        //Testing precursor precondition, essentially at front of sequence if this is true
        if ((precursor == NULL) || (!is_item()))
        {
            //Inserts a value at the head if no current item
            list_head_insert(head_ptr, entry);
            cursor = head_ptr;
            precursor = NULL;

            //Setting tail pointer if adding a link to an empty list
            if (many_nodes == 0)
                tail_ptr = head_ptr;
        }
        else
        {
            //Inserts a value before the current item, if there is a current item
            list_insert(precursor, entry);
            //Setting current item to the new item
            cursor = precursor -> link();//Setting cursor to one link past precursor which should be the new item
        }

        ++many_nodes;

        return;
    }

    //Adding a value after the current item
    template <class Item>
    void sequence<Item> :: attach(const Item& entry)
    {
        if (is_item())
        {
            // Check if tail_ptr needs to be updated
            bool wasAtEnd = (cursor == tail_ptr);
            //Attaches a value after the current item, if there is a current item
            precursor = cursor;
            list_insert(precursor, entry);
            cursor = cursor -> link();
            // Update tail_ptr if necessary
            if (wasAtEnd)
                tail_ptr = tail_ptr -> link();
        }
        // !is_item()
        else
        {
            //If list is empty, insert node at front of sequence
            if ((head_ptr == NULL) && (tail_ptr == NULL))
            {
                list_head_insert (head_ptr, entry);
                cursor = head_ptr;
                precursor = NULL;
                tail_ptr = head_ptr;
            }
            //if no current item, attach at end of list
            else
            {
                // Not necessary: precursor = list_locate (head_ptr, list_length (head_ptr));
                list_insert (precursor, entry);
                cursor = precursor -> link();
                tail_ptr = tail_ptr -> link();
            }
        }

        ++many_nodes;

        return;
    }

    //Removes the current_index value from the sequence
    template <class Item>
    void sequence<Item> :: remove_current()
    {
        assert (is_item());//Checking precondition

        //Removing item from list if at head of list
        if (cursor == head_ptr)
        {
            if (many_nodes > 1)
            {
                list_head_remove(head_ptr);
                cursor = head_ptr;
                precursor = NULL;
            }
            else
            {
                list_head_remove(head_ptr);
                cursor = NULL;
                precursor = NULL;
                head_ptr = NULL;
                tail_ptr = NULL;
            }
        }

        //Removing item if not at head of list
        else
        {
            if (cursor == tail_ptr)
                tail_ptr = precursor;
            cursor = cursor -> link();//Advancing the cursor away from the node to be deleted
            list_remove(precursor);//Deleting target
        }

        --many_nodes;

        return;
    }

    template <class Item>
    void sequence<Item> :: operator =(const sequence& source)
    {
        //Checking for self assignment
        if (this == &source)
            return;

        // Clear the list to free the memory and avoid memory leaks!
        list_clear(head_ptr);
        init();

        //If head_ptr = NULL implementation, copying using the null class member list_copy
        if (source.head_ptr == NULL)
        {
            list_copy(source.head_ptr, head_ptr, tail_ptr);
            cursor = NULL;
            precursor = NULL;
        }
        //If the cursor is on the first item on the list
        if (source.many_nodes == 1 || source.cursor == source.head_ptr)
        {
            list_copy(source.head_ptr, head_ptr, tail_ptr);
            // Figure out which cursor-precursor state the original, was in.
            if (source.cursor == source.head_ptr)
            {
                cursor = head_ptr;
                precursor = NULL;
            }
            else
            {
                cursor = NULL;
                precursor = head_ptr;
            }

        }
        //If the source cursor is not at the first item in the list and greater than 0
        else if (source.many_nodes > 1)
        {
            //Copying values in two pieces
            list_piece(source.head_ptr, source.cursor, head_ptr, precursor);
            if (source.cursor != NULL)
                list_piece(source.cursor, (coen79_lab8::node<Item>*) NULL, cursor, tail_ptr);
            else
            {
                cursor = NULL;
                tail_ptr = precursor;
            }
            //Putting the two separate pieces together
            precursor->set_link(cursor);
        }

        //Setting many_nodes variable
        many_nodes = source.many_nodes;

        return;
    }

    //CONSTANT MEMBER FUNCTIONS
    template <class Item>
    typename sequence<Item> :: size_type sequence<Item> :: size() const
    {
        return many_nodes;
    }

    template <class Item>
    bool sequence<Item> :: is_item() const
    {
        return (cursor != NULL);
    }

    template <class Item>
    typename sequence<Item> :: value_type sequence<Item> :: current() const
    {
        assert (is_item());//Testing precondition of function

        return cursor -> data();
    }
}
