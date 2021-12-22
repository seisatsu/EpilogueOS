#ifndef VECTOR_H_
#define VECTOR_H_

#include <stdlib.h>
#include <string.h>

#define xmalloc(T, count) \
    ((T*)malloc(sizeof(T) * (count)))

/**
 * A dynamic array of contiguous memory, like std::vector.
 */
template<typename X>
class mal_vector {
 public:
    /**
     * Pointer to first element.
     */
    X* data;

    /**
     * Number of elements currently in the vector.
     */
    size_t size;

    /**
     * Number of elements for which memory is reserved.
     */
    size_t capacity;

 public:
    //
    // Constructors & destructor
    //

    /**
     * Construct an empty vector.
     */
    mal_vector() : data(0), size(0), capacity(0) { }

    /**
     * Construct a copy of another vector.
     */
    mal_vector(const mal_vector& other) {
        if (other.capacity == 0) {
            data = 0;
            size = capacity = 0;
        }
        else {
            data = xmalloc(X, other.capacity);
            size = other.size;
            capacity = other.capacity;
            for (size_t i = 0; i < size; i++) {
                new (data + i) X(other.data[i]);
            }
        }
    }

    /**
     * Construct vector by stealing another's contents.
     */
    mal_vector(mal_vector&& other) {
        data = other.data;
        size = other.size;
        capacity = other.capacity;
        other.data = 0;
        other.size = other.capacity = 0;
    }

    /**
     * Free a vector's memory and destroy its contents.
     */
    ~mal_vector() {
        for (size_t i = 0; i < size; i++) {
            data[i].~X();
        }
        free(data);
    }

    //
    // Operators
    //

    /**
     * Destroy the current contents and copy in another vector's.
     */
    void operator=(const mal_vector& other) {
        for (size_t i = 0; i < size; i++) {
            data[i].~X();
        }
        free(data);
        if (other.capacity == 0) {
            data = 0;
            size = capacity = 0;
        }
        else {
            data = xmalloc(X, other.capacity);
            size = other.size;
            capacity = other.capacity;
            for (size_t i = 0; i < size; i++) {
                new (data + i) X(other[i]);
            }
        }
    }

    /**
     * Destroy the current contents and move in another vector's.
     */
    void operator=(mal_vector&& other) {
        for (size_t i = 0; i < size; i++) {
            data[i].~X();
        }
        free(data);
        data = other.data;
        size = other.size;
        capacity = other.capacity;
        other.data = 0;
        other.size = other.capacity = 0;
    }

    //
    // Element accesses
    //

    /**
     * Access a single element.
     */
    X& operator[](size_t i) {
        return data[i];
    }

    /**
     * Get a pointer to the first element.
     *
     * Useful mostly in range-for loops.
     */
    X* begin() {
        return data;
    }

    /**
     * Get a pointer past the last element.
     *
     * Useful mostly in range-for loops.
     */
    X* end() {
        return data + size;
    }

    //
    // Element additions
    //

    /**
     * Add a element to the end by copying an existing value.
     */
    void push(const X& x) {
        grow();
        new (data + size) X(x);
        size++;
    }

    /**
     * Add a element to the end by moving an existing value.
     */
    void push(X&& x) {
        grow();
        new (data + size) X(static_cast<X&&>(x));
        size++;
    }

    /**
     * Add a element to an arbitrary position by copying an existing value.
     */
    void insert(size_t i, const X& x) {
        // FIXME: Does not call move constructors.
        grow();
        memmove(data + i, data + i + 1, sizeof(X) * (size - i));
        new (data + i) X(x);
        size++;
    }

    /**
     * Add a element to an arbitrary position by moving an existing value.
     */
    void insert(size_t i, X&& x) {
        // FIXME: Does not call move constructors.
        grow();
        memmove(data + i, data + i + 1, sizeof(X) * (size - i));
        new (data + i) X(static_cast<X&&>(x));
        size++;
    }

    /**
     * Add an array of element to the end by copying existing values.
     */
    void append(size_t n, const X* xs) {
        reserve(size + n);  // FIXME: Choose better size.
        for (size_t i = 0; i < n; i++) {
            new (data + size + i) X(xs[i]);
        }
        size += n;
    }

    //
    // Element removals
    //

    /**
     * Remove the last element.
     */
    void pop() {
        data[size - 1].~X();
        size--;
    }

    /**
     * Remove an element.
     */
    void erase(size_t i) {
        for (size_t j = i; j < size - 1; j++) {
            data[j] = static_cast<X&&>(data[j + 1]);
        }
        size--;
        data[size].~X();
    }

    //
    // Memory management
    //

    /**
     * Allocate uninitialized memory for a number of elements.
     *
     * Calls move constructors (which empties the old objects), but does not
     * call destructors on the just-moved objects since they are hopefully
     * empty.
     */
    void reserve(size_t n) {
        X* newData = xmalloc(X, n);
        for (size_t i = 0; i < size; i++) {
            new (newData + i) X(static_cast<X&&>(data[i]));
        }
        free(data);
        data = newData;
        capacity = n;
    }

    /**
     * Grow or shrink the size of the vector, constructing and destructing
     * elements as necessary.
     *
     * If necessary, additional memory will be allocated.
     */
    void resize(size_t n) {
        if (n > capacity) {
            reserve(n);
        }
        if (n > size) {
            for (size_t i = size; i < n; i++) {
                new (data + i) X;
            }
        }
        else if (n < size) {
            for (size_t i = n; i < size; i++) {
                data[i].~X();
            }
        }
        size = n;
    }

    /**
     * Grow the vector if it is full.
     */
    void grow() {
        if (size == capacity) {
            reserve(size == 0 ? 4 : size * 2);  // FIXME: Choose better size.
        }
    }

    /**
     * Resize the vector to 0 elements.
     */
    void clear() {
        for (size_t i = 0; i < size; i++) {
            data[i].~X();
        }
        size = 0;
    }

    /**
     * Drop the vector's data pointer so it won't be freed upon vector
     * destruction.
     *
     * Resets the vector to its default, empty state.
     */
    void reset() {
        data = 0;
        size = capacity = 0;
    }
};

#endif
