#include <iostream>
#include <list>
#include <typeinfo>
#include <cstdlib>
#include "gc_details.h"
#include "gc_iterator.h"
/*
    Pointer implements a pointer type that uses
    garbage collection to release unused memory.
    A Pointer must only be used to point to memory
    that was dynamically allocated using new.
    When used to refer to an allocated array,
    specify the array size.
    */
template <class T, int size = 0>
class Pointer{
private:
    static std::list<PtrDetails<T> > refContainer;                        // refContainer maintains the garbage collection list.    
    T *addr;                                                              // addr points to the allocated memory        
    bool isArray;                                                         // isArray is true if this Pointer points to an allocated array. It is false otherwise.    
    unsigned arraySize;                                                   // size of the array
    static bool first;                                                    // true when first Pointer is created    
    typename std::list<PtrDetails<T> >::iterator findPtrInfo(T *ptr);     // Return an iterator to pointer details in refContainer.

public:

    typedef Iter<T> GCiterator;                                           // Define an iterator type for Pointer<T>.
    // Empty constructor
    // NOTE: templates aren't able to have prototypes with default arguments, this is why constructor is designed like this:
    //Pointer()  { Pointer(NULL); }
    Pointer(T* t=NULL);

    // Copy constructor.
    Pointer(const Pointer &);

    // Destructor for Pointer.
    ~Pointer();

    // Collect garbage. Returns true if at least one object was freed.
    static bool collect();

    // Overload assignment of pointer to Pointer.
    T *operator=(T *t);

    // Overload assignment of Pointer to Pointer.
    Pointer &operator=(Pointer &rv);

    // Return a reference to the object pointed to by this Pointer.
    T &operator*()
    {
        return *addr;
    }

    // Return the address being pointed to.
    T *operator->() { return addr; }

    // Return a reference to the object at the index specified by i.
    T &operator[](int i){ return addr[i]; }

    // Conversion function to T *.
    operator T *() { return addr; }

    // Return an Iter to the start of the allocated memory.
    Iter<T> begin()
    {
        int _size;
        if (isArray)
        {
            _size = arraySize;
        }
        else
        {
            _size = 1;
        }

        return Iter<T>(addr, addr, addr + _size);
    }

    // Return an Iter to one past the end of an allocated array.
    Iter<T> end()
    {
        int _size;
        if (isArray)
        {
            _size = arraySize;
        }
        else
        {
            _size = 1;
        }

        return Iter<T>(addr + _size, addr, addr + _size);
    }

    // Return the size of refContainer for this type of Pointer.
    static int refContainerSize() { return refContainer.size(); }

    // A utility function that displays refContainer.
    static void showlist();

    // Clear refContainer when program exits.
    static void shutdown();
};

// STATIC INITIALIZATION
// Creates storage for the static variables
template <class T, int size>
std::list<PtrDetails<T> > Pointer<T, size>::refContainer;

template <class T, int size>
bool Pointer<T, size>::first = true;

// Constructor for both initialized and uninitialized objects. -> see class interface
template<class T, int size>
Pointer<T, size>::Pointer(T *t = NULL)
{
    // Register shutdown() as an exit function.
    if (first)
    {
        atexit(shutdown);
    }
    first = false;

    // DONE: Implement Pointer constructor   

    // find if the input pointer (*t) is inside the refContainer
    typename std::list<PtrDetails<T> >::iterator p;
    p = findPtrInfo(t);

    // increment ref count if *t is already in the refContainer. Otherwise, add it to the list.
    if (p != refContainer.end())
    {
        p->refcount++;
    }
    else
    {
        PtrDetails<T> gc_obj(t, size);  // Create and store this entry. 
        refContainer.push_front(gc_obj);
    }

    addr = t;
    arraySize = size;
    if (size > 0)
    {
        isArray = true;
    }
    else
    {
        isArray = false;
    }
}

// Copy constructor.
template< class T, int size>
Pointer<T, size>::Pointer(const Pointer &ob)
{
    // DONE: Implement Pointer constructor
    typename std::list<PtrDetails<T>>::iterator p;
    p = findPtrInfo(ob.addr);
    p->refcount++; // increment ref count
    addr = ob.addr;
    arraySize = ob.arraySize;
    if (arraySize > 0) 
    {
        isArray = true;
    }
    else 
    {
        isArray = false;
    }

}

// Destructor for Pointer.
template <class T, int size>
Pointer<T, size>::~Pointer()
{
    // DONE: Implement Pointer destructor
    typename std::list<PtrDetails<T>>::iterator p;
    p = findPtrInfo(addr);
    if (p->refcount)
    {
        p->refcount--;
    }

    //collect garbage
    collect();
}

// Collect garbage. Returns true if at least one object was freed.
template <class T, int size>
bool Pointer<T, size>::collect()
{
    // DONE: Implement collect function    
    bool memfreed = false;
    typename std::list<PtrDetails<T> >::iterator p;
    do{
        // Scan refContainer looking for unreferenced pointers.
        for (p = refContainer.begin(); p != refContainer.end(); p++)
        {
            // DONE: Implement collect()
            // If in-use, skip.
            if (p->refcount > 0)
            {
                continue;
            }


            //set memfreed to true
            memfreed = true;

            // Remove unused entry from refContainer.
            refContainer.remove(*p);

            // Free memory unless the Pointer is null.
            if (p->memPtr)
            {
                if (p->isArray)
                {
                    delete[] p->memPtr;  //delete array type memory
                }
                else
                {
                    delete p->memPtr;   //delete non-array type memory
                }
            }

            // Restart the search.
            break;
        }
    } while (p != refContainer.end());

    return memfreed;
}

// Overload assignment of pointer to Pointer.
template <class T, int size>
T *Pointer<T, size>::operator=(T *t)
{
    // DONE: Implement operator==

    //step 1: find the current address and decrement its refcount.
    typename std::list<PtrDetails<T>>::iterator p;
    p = findPtrInfo(addr);
    p->refcount--;

    //step 2: if the new pointer is in the list then increment its refcount.
    p = findPtrInfo(t);
    if (p != refContainer.end())
    {
        p->refcount++;
    }
    else
    {
        PtrDetails<T>gc_obj(t, size);
        refContainer.push_front(gc_obj);
    }

    addr = t;
    return t;
}

// Overload assignment of Pointer to Pointer.
template <class T, int size>
Pointer<T, size> &Pointer<T, size>::operator=(Pointer &rv)
{
    // DONE: Implement operator==

    //step 1: find the current address and decrement its refcount.
    typename std::list<PtrDetails<T>>::iterator p;
    p = findPtrInfo(addr);
    p->refcount--;

    //step 2: if the new pointer is in the list then increment its refcount.
    p = findPtrInfo(rv.addr);
    p->refcount++;

    addr = rv.addr;
    return rv;

}

// A utility function that displays refContainer.
template <class T, int size>
void Pointer<T, size>::showlist()
{
    typename std::list<PtrDetails<T> >::iterator p;
    std::cout << "refContainer<" << typeid(T).name() << ", " << size << ">:\n";
    std::cout << "memPtr refcount value\n ";
    if (refContainer.begin() == refContainer.end())
    {
        std::cout << " Container is empty!\n\n ";
    }
    for (p = refContainer.begin(); p != refContainer.end(); p++)
    {
        std::cout << "[" << (void *)p->memPtr << "]" << " " << p->refcount << " ";
        if (p->memPtr)
        {
            std::cout << " " << *p->memPtr;
        }
        else
        {
            std::cout << "---";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

// Find a pointer in refContainer.
template <class T, int size>
typename std::list<PtrDetails<T> >::iterator Pointer<T, size>::findPtrInfo(T *ptr)
{
    typename std::list<PtrDetails<T> >::iterator p;
    // Find ptr in refContainer.
    for (p = refContainer.begin(); p != refContainer.end(); p++)
    {
        if (p->memPtr == ptr)
        {
            return p;
        }
    }
    return p;
}

// Clear refContainer when program exits.
template <class T, int size>
void Pointer<T, size>::shutdown()
{
    if (refContainerSize() == 0)
    {
        return; // list is empty
    }
    typename std::list<PtrDetails<T> >::iterator p;
    for (p = refContainer.begin(); p != refContainer.end(); p++)
    {
        // Set all reference counts to zero
        p->refcount = 0;
    }
    collect();
}
