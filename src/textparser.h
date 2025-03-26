#pragma once
#include "text.h"
#include "utils.h"

namespace TextParser {
    Text readText(std::wistream& input);
    void writeText(Text& text, std::wostream& output);
};