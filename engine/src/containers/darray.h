#pragma once

#include "defines.h"
#include "core/asserts.h"

namespace siren {
    template <typename T>
    class DArray {
    public:
        DArray(uint32_t capacity = 1, uint32_t scale_factor = 2) {
            this->capacity = capacity;
            this->scale_factor = scale_factor;
            _size = 0;
            data = (T*)malloc(sizeof(T) * capacity);
        }

        ~DArray() {
            free(data);
        }

        DArray(const DArray& other) {
            capacity = other.capacity;
            scale_factor = other.scale_factor;
            _size = other._size;
            data = (T*)malloc(sizeof(T) * capacity);
            memcpy(data, other.data, sizeof(T) * capacity);
        }

        DArray& operator=(const DArray& other) {
            if (data != nullptr) {
                free(data);
            }

            capacity = other.capacity;
            scale_factor = other.scale_factor;
            _size = other._size;
            data = (T*)malloc(sizeof(T) * capacity);
            memcpy(data, other.data, sizeof(T) * capacity);

            return *this;
        }

        DArray(DArray&& other) {
            capacity = other.capacity;
            scale_factor = other.scale_factor;
            _size = other._size;
            data = other.data;
            other.data = nullptr;
        }

        DArray& operator=(DArray&& other) {
            if (data != nullptr) {
                free(data);
            }

            capacity = other.capacity;
            scale_factor = other.scale_factor;
            _size = other._size;
            data = other.data;
            other.data = nullptr;

            return *this;
        }

        uint32_t size() const {
            return _size;
        }

        bool empty() const {
            return _size == 0;
        }

        T& operator[](uint32_t index) {
            SIREN_ASSERT_MESSAGE(index < _size, "DArray::operator[] index out of bounds");
            return data[index];
        }

        const T& operator[](uint32_t index) const {
            SIREN_ASSERT_MESSAGE(index < _size, "const DArray::operator[] index out of bounds");
            return data[index];
        }

        void reserve(uint32_t capacity) {
            this->capacity = capacity;

            T* temp = data;
            data = (T*)malloc(sizeof(T) * capacity);
            if (_size != 0) {
                memcpy(data, temp, sizeof(T) * _size);
            }
            free(temp);
        }

        void push(T element) {
            if (_size == capacity) {
                reserve(capacity * scale_factor);
            }
            data[_size] = element;
            _size++;
        }

        void pop() {
            if (_size == 0) {
                return;
            }
            _size--;
        }

        void remove_at(uint32_t index) {
            SIREN_ASSERT_MESSAGE(index < _size, "DArray::remove_at() index out of bounds");
            _size--;
            if (index != _size) {
                memcpy((void*)(data + index), (void*)(data + index + 1), sizeof(T) * (_size - index));
            }
        }
    private:
        T* data;
        uint32_t _size;
        uint32_t capacity;
        uint32_t scale_factor;
    };
}