#ifndef VECTOR_H_
#define VECTOR_H_

#include <stdlib.h>
#include <string.h>

// Does not call move constructors in its own move constructor or when growing.
template<typename X>
class mal_vector {
 public:
    X* data;
    size_t size;
    size_t capacity;

 public:
    //
    // Constructors & destructor
    //

    mal_vector() : data(0), size(0), capacity(0) { }

    mal_vector(const mal_vector& other) {
        if (other.capacity == 0) {
            data = 0;
            size = capacity = 0;
        }
        else {
            data = malloc(X, other.capacity);
            size = other.size;
            capacity = other.capacity;
            for (size_t i = 0; i < size; i++) {
                new (data + i) X(other.data[i]);
            }
        }
    }

    mal_vector(mal_vector&& other) {
        data = other.data;
        size = other.size;
        capacity = other.capacity;
        other.data = 0;
        other.size = other.capacity = 0;
    }

    ~mal_vector() {
        for (size_t i = 0; i < size; i++) {
            data[i].~X();
        }
        free(data);
    }

    //
    // Operators
    //

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
            data = malloc(X, other.capacity);
            size = other.size;
            capacity = other.capacity;
            for (size_t i = 0; i < size; i++) {
                new (data + i) X(other[i]);
            }
        }
    }

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

    X& operator[](size_t i) {
        assert(i < size);
        return data[i];
    }

    X* begin() {
        return data;
    }

    X* end() {
        return data + size;
    }

    //
    // Element additions
    //

    void push(const X& x) {
        grow();
        new (data + size) X(x);
        size++;
    }

    void push(X&& x) {
        grow();
        new (data + size) X(static_cast<X&&>(x));
        size++;
    }

    void insert(size_t i, const X& x) {
        // FIXME: Does not call move constructors.
        assert(i <= size);
        grow();
        memmove(data + i, data + i + 1, sizeof(X) * (size - i));
        new (data + i) X(x);
        size++;
    }

    void insert(size_t i, X&& x) {
        // FIXME: Does not call move constructors.
        assert(i <= size);
        grow();
        memmove(data + i, data + i + 1, sizeof(X) * (size - i));
        new (data + i) X(static_cast<X&&>(x));
        size++;
    }

    void append(size_t n, const X* xs) {
        assert(n <= size);
        reserve(size + n);  // FIXME: Choose better size.
        for (size_t i = 0; i < n; i++) {
            new (data + size + i) X(xs[i]);
        }
        size += n;
    }


    //
    // Element removals
    //

    void pop() {
        assert(size);
        data[size - 1].~X();
        size--;
    }

    void erase(size_t i) {
        assert(i < size);
        for (size_t j = i; j < size - 1; j++) {
            data[j] = static_cast<X&&>(data[j + 1]);
        }
        size--;
        data[size].~X();
    }

    //
    // Miscellaneous
    //

    // Calls move constructors (which empties the old objects), but not call
    // destructors on the just-moved objects since they are hopefully empty.
    void reserve(size_t n) {
        assert(n > capacity);
        X* newData = malloc(X, n);
        for (size_t i = 0; i < size; i++) {
            new (newData + i) X(static_cast<X&&>(data[i]));
        }
        free(data);
        data = newData;
        capacity = n;
    }

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

    void grow() {
        if (size == capacity) {
            reserve(size == 0 ? 4 : size * 2);  // FIXME: Choose better size.
        }
    }

    void clear() {
        for (size_t i = 0; i < size; i++) {
            data[i].~X();
        }
        size = 0;
    }

    void reset() {
        data = 0;
        size = capacity = 0;
    }
};

#endif
