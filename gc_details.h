// This class defines an element that is stored
// in the garbage collection information list.
//
template <class T>
class PtrDetails
{
public:
    unsigned refcount;       // current reference count
    T *memPtr;               // pointer to allocated memory 
    bool isArray;            // true if memPtr points to an allocated array
    unsigned arraySize;      // contains size of array memPtr is pointing to an allocated array.

    // Here, mPtr points to the allocated memory.    
    PtrDetails(T *ptr, int array_size)
    {
        // DONE: Implement PtrDetails
        refcount = 1;        
        memPtr = ptr;
        arraySize = array_size;
        if (arraySize > 0)
        {
            isArray = true;
        }
        else
        {
            isArray = false;
        }
    }
};

// Overloading operator== allows two class objects to be compared.
template <class T>
bool operator==(const PtrDetails<T> &obj1, const PtrDetails<T> &obj2)
{
    // DONE: Implement operator==
    if (obj1.memPtr == obj2.memPtr)
    {
        //check if the member variables are also matching
        if ((obj1.isArray != obj2.isArray) || (obj1.arraySize != obj2.arraySize))
        {
            throw std::runtime_error("Object members do not match..!");
        }
        return true;
    }
    else
    {
        return false;
    }
}
