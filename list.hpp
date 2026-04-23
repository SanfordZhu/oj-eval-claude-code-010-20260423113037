#ifndef SJTU_LIST_HPP
#define SJTU_LIST_HPP

#include "exceptions.hpp"
#include "algorithm.hpp"

#include <climits>
#include <cstddef>

namespace sjtu {

template<typename T>
class list {
protected:
    class node {
    public:
        T *data;
        node *prev;
        node *next;

        node() : data(nullptr), prev(nullptr), next(nullptr) {}

        node(const T &value) : prev(nullptr), next(nullptr) {
            data = new T(value);
        }

        ~node() {
            if (data != nullptr) {
                delete data;
                data = nullptr;
            }
        }
    };

    node *head;
    node *tail;
    size_t _size;

    node *insert(node *pos, node *cur) {
        cur->prev = pos->prev;
        cur->next = pos;
        pos->prev->next = cur;
        pos->prev = cur;
        return cur;
    }

    node *erase(node *pos) {
        node *ret = pos->next;
        pos->prev->next = pos->next;
        pos->next->prev = pos->prev;
        pos->prev = pos->next = nullptr;
        return ret;
    }

public:
    class const_iterator;
    class iterator {
    private:
        node *ptr;
        list *container;

    public:
        iterator() : ptr(nullptr), container(nullptr) {}
        iterator(node *p, list *c) : ptr(p), container(c) {}

        iterator operator++(int) {
            if (ptr == nullptr || ptr == container->tail) {
                throw invalid_iterator();
            }
            iterator tmp = *this;
            ptr = ptr->next;
            return tmp;
        }

        iterator &operator++() {
            if (ptr == nullptr || ptr == container->tail) {
                throw invalid_iterator();
            }
            ptr = ptr->next;
            return *this;
        }

        iterator operator--(int) {
            if (ptr == nullptr || ptr == container->head->next) {
                throw invalid_iterator();
            }
            iterator tmp = *this;
            ptr = ptr->prev;
            return tmp;
        }

        iterator &operator--() {
            if (ptr == nullptr || ptr == container->head->next) {
                throw invalid_iterator();
            }
            ptr = ptr->prev;
            return *this;
        }

        T &operator*() const {
            if (ptr == nullptr || ptr == container->tail) {
                throw invalid_iterator();
            }
            return *(ptr->data);
        }

        T *operator->() const {
            if (ptr == nullptr || ptr == container->tail) {
                throw invalid_iterator();
            }
            return ptr->data;
        }

        bool operator==(const iterator &rhs) const {
            return ptr == rhs.ptr && container == rhs.container;
        }

        bool operator==(const const_iterator &rhs) const {
            return ptr == rhs.ptr && container == rhs.container;
        }

        bool operator!=(const iterator &rhs) const {
            return !(*this == rhs);
        }

        bool operator!=(const const_iterator &rhs) const {
            return !(*this == rhs);
        }

        friend class list;
        friend class const_iterator;
    };

    class const_iterator {
    private:
        const node *ptr;
        const list *container;

    public:
        const_iterator() : ptr(nullptr), container(nullptr) {}
        const_iterator(const node *p, const list *c) : ptr(p), container(c) {}
        const_iterator(const iterator &other) : ptr(other.ptr), container(other.container) {}

        const_iterator operator++(int) {
            if (ptr == nullptr || ptr == container->tail) {
                throw invalid_iterator();
            }
            const_iterator tmp = *this;
            ptr = ptr->next;
            return tmp;
        }

        const_iterator &operator++() {
            if (ptr == nullptr || ptr == container->tail) {
                throw invalid_iterator();
            }
            ptr = ptr->next;
            return *this;
        }

        const_iterator operator--(int) {
            if (ptr == nullptr || ptr == container->head->next) {
                throw invalid_iterator();
            }
            const_iterator tmp = *this;
            ptr = ptr->prev;
            return tmp;
        }

        const_iterator &operator--() {
            if (ptr == nullptr || ptr == container->head->next) {
                throw invalid_iterator();
            }
            ptr = ptr->prev;
            return *this;
        }

        const T &operator*() const {
            if (ptr == nullptr || ptr == container->tail) {
                throw invalid_iterator();
            }
            return *(ptr->data);
        }

        const T *operator->() const {
            if (ptr == nullptr || ptr == container->tail) {
                throw invalid_iterator();
            }
            return ptr->data;
        }

        bool operator==(const iterator &rhs) const {
            return ptr == rhs.ptr && container == rhs.container;
        }

        bool operator==(const const_iterator &rhs) const {
            return ptr == rhs.ptr && container == rhs.container;
        }

        bool operator!=(const iterator &rhs) const {
            return !(*this == rhs);
        }

        bool operator!=(const const_iterator &rhs) const {
            return !(*this == rhs);
        }

        friend class list;
    };

    list() : _size(0) {
        head = new node();
        tail = new node();
        head->next = tail;
        tail->prev = head;
    }

    list(const list &other) : _size(0) {
        head = new node();
        tail = new node();
        head->next = tail;
        tail->prev = head;

        for (const_iterator it = other.cbegin(); it != other.cend(); ++it) {
            push_back(*it);
        }
    }

    virtual ~list() {
        clear();
        delete head;
        delete tail;
    }

    list &operator=(const list &other) {
        if (this == &other) {
            return *this;
        }

        clear();
        for (const_iterator it = other.cbegin(); it != other.cend(); ++it) {
            push_back(*it);
        }

        return *this;
    }

    const T &front() const {
        if (empty()) {
            throw container_is_empty();
        }
        return *(head->next->data);
    }

    const T &back() const {
        if (empty()) {
            throw container_is_empty();
        }
        return *(tail->prev->data);
    }

    iterator begin() {
        return iterator(head->next, this);
    }

    const_iterator cbegin() const {
        return const_iterator(head->next, this);
    }

    iterator end() {
        return iterator(tail, this);
    }

    const_iterator cend() const {
        return const_iterator(tail, this);
    }

    virtual bool empty() const {
        return _size == 0;
    }

    virtual size_t size() const {
        return _size;
    }

    virtual void clear() {
        node *curr = head->next;
        while (curr != tail) {
            node *tmp = curr;
            curr = curr->next;
            delete tmp;
        }
        head->next = tail;
        tail->prev = head;
        _size = 0;
    }

    virtual iterator insert(iterator pos, const T &value) {
        if (pos.container != this) {
            throw invalid_iterator();
        }

        node *new_node = new node(value);
        node *insert_pos = pos.ptr;
        insert(insert_pos, new_node);
        _size++;

        return iterator(new_node, this);
    }

    virtual iterator erase(iterator pos) {
        if (pos.container != this) {
            throw invalid_iterator();
        }
        if (pos.ptr == tail || pos.ptr == head) {
            throw invalid_iterator();
        }

        node *next_node = erase(pos.ptr);
        delete pos.ptr;
        _size--;

        return iterator(next_node, this);
    }

    void push_back(const T &value) {
        insert(end(), value);
    }

    void pop_back() {
        if (empty()) {
            throw container_is_empty();
        }
        erase(--end());
    }

    void push_front(const T &value) {
        insert(begin(), value);
    }

    void pop_front() {
        if (empty()) {
            throw container_is_empty();
        }
        erase(begin());
    }

    void sort() {
        if (_size <= 1) {
            return;
        }

        T **arr = new T *[_size];
        node *curr = head->next;
        size_t idx = 0;

        while (curr != tail) {
            arr[idx++] = new T(*(curr->data));
            curr = curr->next;
        }

        sjtu::sort<T *>(arr, arr + _size, [](const T *a, const T *b) { return *a < *b; });

        curr = head->next;
        idx = 0;
        while (curr != tail) {
            *(curr->data) = *(arr[idx]);
            delete arr[idx];
            idx++;
            curr = curr->next;
        }

        delete[] arr;
    }

    void merge(list &other) {
        if (this == &other) {
            return;
        }

        node *p1 = head->next;
        node *p2 = other.head->next;

        while (p1 != tail && p2 != other.tail) {
            if (!(*(p2->data) < *(p1->data))) {
                p1 = p1->next;
            } else {
                node *tmp = p2;
                p2 = p2->next;
                other.head->next = p2;
                if (p2 != nullptr) {
                    p2->prev = other.head;
                }

                insert(p1, tmp);
                _size++;
            }
        }

        while (p2 != other.tail) {
            node *tmp = p2;
            p2 = p2->next;
            other.head->next = p2;
            if (p2 != nullptr) {
                p2->prev = other.head;
            }

            insert(tail, tmp);
            _size++;
        }

        other.head->next = other.tail;
        other.tail->prev = other.head;
        other._size = 0;
    }

    void reverse() {
        if (_size <= 1) {
            return;
        }

        node *curr = head;
        while (curr != nullptr) {
            node *tmp = curr->next;
            curr->next = curr->prev;
            curr->prev = tmp;
            curr = tmp;
        }

        node *tmp = head;
        head = tail;
        tail = tmp;
    }

    void unique() {
        if (_size <= 1) {
            return;
        }

        node *curr = head->next->next;
        while (curr != tail) {
            if (*(curr->prev->data) == *(curr->data)) {
                node *to_delete = curr;
                curr = curr->next;
                erase(to_delete);
                delete to_delete;
                _size--;
            } else {
                curr = curr->next;
            }
        }
    }
};

}

#endif //SJTU_LIST_HPP
