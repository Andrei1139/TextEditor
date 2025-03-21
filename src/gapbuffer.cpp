#include "gapbuffer.h"

wchar_t GapBuffer::getChar() const {
    return string[gapStart];
}

void GapBuffer::insert(wchar_t input) {
    if (gapStart == gapEnd) {
        resize();
        for (size_t i = gapStart; i < capacity - GAP_ALLOC_BLOCK; ++i) {
            string[i + GAP_ALLOC_BLOCK] = string[i];
        }
        gapEnd = capacity;
    }

    string[gapStart++] = input;
    ++size;
}

void GapBuffer::remove() {
    if (gapStart == 0) {
        return;
    }
    --gapStart;
    --size;
};

void GapBuffer::resize() {
    wchar_t *newBuffer = new wchar_t[capacity + GAP_ALLOC_BLOCK];
    if (!newBuffer) {
        throw std::runtime_error("Memory allocation failed\n");
    }

    for (size_t i = 0; i < capacity; ++i) {
        newBuffer[i] = string[i];
    }

    string = newBuffer;
    capacity += GAP_ALLOC_BLOCK;
}

void GapBuffer::moveCursorLeft() {
    if (gapStart == 0)
        return;

    string[--gapEnd] = string[--gapStart];
}

void GapBuffer::moveCursorRight() {
    if (gapStart == size)
        return;
    
    string[gapStart++] = string[gapEnd++];
}