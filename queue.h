#pragma once
#include <stdexcept>

template <typename T>
class QueueItem {
public:
	QueueItem(T data) {
		this->data = data;
		this->next = nullptr;
	}
	T data;
	QueueItem<T>* next;
};


template <typename T>
class Queue {
private:
    int _length = 0;
    int _capacity;
    QueueItem<T>* _head = nullptr;

public:
    Queue(int capacity) : _capacity(capacity) {}

    void set_cap(int new_cap) { _capacity = new_cap; }

    int len() { return _length; }
    int cap() { return _capacity; }

    bool empty() { return _length == 0; }
    bool full() { return _length >= _capacity; }

    bool push(T item) {
        if (_length == _capacity) {
            return false;
        }

        _length++;
        QueueItem<T>* new_item = new QueueItem<T>(item);

        if (_head == nullptr) {
            _head = new_item;
            return true;
        }

        QueueItem<T>* cursor = _head;
        while (cursor->next != nullptr) {
            cursor = cursor->next;
        }
        cursor->next = new_item;

        return true;
    }

    T pop() {
        if (_head == nullptr) {
            throw std::invalid_argument("queue is empty");
        }

        _length--;
        QueueItem<T>* item = _head;

        _head = item->next;

        T data = item->data;
        delete item;
        return data;
    }
};
