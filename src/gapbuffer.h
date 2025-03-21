#pragma once
#include "utils.h"

constexpr int ALLOC_BLOCK = 1000;
constexpr int GAP_ALLOC_BLOCK = 100;

class GapBuffer {
    public:
        GapBuffer(): string{new wchar_t[ALLOC_BLOCK]}, size{0},
                                                       capacity{ALLOC_BLOCK},
                                                       gapStart{0},
                                                       gapEnd{ALLOC_BLOCK} {}
        ~GapBuffer() {
            delete[] string;
        }
        int getSize() const {return size;}
        wchar_t getChar() const;
        void insert(wchar_t input);
        void remove();

        void resize();

        void moveCursorLeft();
        void moveCursorRight();

        class Iterator {
            using category = std::input_iterator_tag;
            using T = wchar_t;
            using Distance = std::ptrdiff_t;
            using Pointer = T*;
            using Reference = T&;
            public:
                Iterator(int _index, GapBuffer &_buffer): index{_index}, buffer{_buffer} {}
                Iterator& operator++() {++index; if (index == buffer.gapStart) index = buffer.gapEnd; return *this;}
                bool operator==(Iterator &other) const {return index == other.index;}
                bool operator!=(Iterator &other) const {return !(*this == other);}
                Reference operator*() {return buffer.string[index];}
            private:
                int index;
                GapBuffer &buffer;
        };

        Iterator begin() {return Iterator(gapStart == 0 ? gapEnd : 0, *this);}
        Iterator end() {return Iterator(gapStart <= size ? capacity : size, *this);}

    private:
        wchar_t *string;
        int size, capacity, gapStart, gapEnd;
};