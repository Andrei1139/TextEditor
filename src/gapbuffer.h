#pragma once
#include "utils.h"

constexpr int ALLOC_BLOCK = 100;
constexpr int GAP_ALLOC_BLOCK = 20;

template<class T>
class GapBuffer {
    public:
        GapBuffer(): elements{new T[ALLOC_BLOCK]}, size{0},
                                                       capacity{ALLOC_BLOCK},
                                                       gapStart{0},
                                                       gapEnd{ALLOC_BLOCK} {}
        ~GapBuffer() {
            delete[] elements;
        }
        GapBuffer<T>& operator=(const GapBuffer<T>& oldBuffer) {
            delete[] elements;
            elements = new T[oldBuffer.capacity];
            size = oldBuffer.size;
            capacity = oldBuffer.capacity;
            gapStart = oldBuffer.gapStart;
            gapEnd = oldBuffer.gapEnd;

            for (size_t i = 0; i < capacity; ++i) {
                elements[i] = oldBuffer.elements[i];
            }

            return *this;
        }

        int getSize() const {return size;}
        T* getElem() {
            if (size == 0)
                return nullptr;
            if (gapStart == 0) {
                return elements + gapEnd;
            }
            return elements + gapStart - 1;
        }
        void insert(const T& input) {
            if (gapStart == gapEnd) {
                resize();
            }
        
            elements[gapStart++] = input;
            ++size;
        }
        void remove() {
            if (gapStart == 0) {
                return;
            }
            --gapStart;
            --size;
        }

        void resize() {
            T *newBuffer = new T[capacity + GAP_ALLOC_BLOCK];
            if (!newBuffer) {
                throw std::runtime_error("Memory allocation failed\n");
            }
            
            capacity += GAP_ALLOC_BLOCK;
            gapEnd += GAP_ALLOC_BLOCK;
        
            for (size_t i = 0; i < capacity; ++i) {
                if (i < gapStart) {
                    newBuffer[i] = elements[i];
                } else if (i >= gapEnd) {
                    newBuffer[i] = elements[i - GAP_ALLOC_BLOCK];
                } else {
                    newBuffer[i] = elements[0];
                }
            }
        
            delete[] elements;
            elements = newBuffer;
        }

        bool moveCursorLeft() {
            if (gapStart == 0)
                return false;
        
            elements[--gapEnd] = elements[--gapStart];
            return true;
        }
        bool moveCursorRight() {
            if (gapStart == size)
                return false;
            
            elements[gapStart++] = elements[gapEnd++];
            return true;
        }
        bool setCursorPos(int newGapStart) {
            if (newGapStart > capacity - (gapEnd - gapStart)) {
                return false;
            }

            if (newGapStart == gapStart)
                return true;

            if (newGapStart > gapStart) {
                for (int i = gapStart; i < newGapStart; ++i) {
                    elements[i] = elements[i + gapEnd - gapStart];
                }
            } else {
                for (int i = gapEnd - (gapStart - newGapStart); i < gapEnd; ++i) {
                    elements[i] = elements[i - (gapEnd - gapStart)];
                }
            }

            gapEnd = newGapStart + gapEnd - gapStart;
            gapStart = newGapStart;
            return true;
        }

        class Iterator {
            using type = T;
            using Reference = T&;
            public:
                Iterator(int _index, const GapBuffer &_buffer): index{_index}, buffer{_buffer} {}
                Iterator& operator++() {++index; if (index == buffer.gapStart) index = buffer.gapEnd; return *this;}
                bool operator==(Iterator &other) const {return index == other.index;}
                bool operator!=(Iterator &other) const {return !(*this == other);}
                Reference operator*() {return buffer.elements[index];}
            private:
                int index;
                const GapBuffer &buffer;
        };

        Iterator begin() const {return Iterator(gapStart == 0 ? gapEnd : 0, *this);}
        Iterator end() const {return Iterator(gapStart <= size ? capacity : size, *this);}

        int getGapStart() const {return gapStart;}
        // friend std::wostream& operator<<(std::wostream& out, const GapBuffer& buffer) {
        //     out << "SIZE: " << buffer.size << ", CAPACITY: " << buffer.capacity
        //     << ", GAPSTART: " << buffer.gapStart << ", GAPEND: " << buffer.gapEnd << '\n';
        //     // for (auto i = 0; i < buffer.capacity; ++i) {
        //     //     if (i >= buffer.gapStart && i < buffer.gapEnd) {
        //     //         out << L'.';
        //     //     } else {
        //     //         out << buffer.elements[i];
        //     //     }
        //     // }

        //     return out;
        // }

    private:
        T *elements;
        int size, capacity, gapStart, gapEnd;
};