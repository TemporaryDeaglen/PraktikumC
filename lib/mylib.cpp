#include "mylib.h"
#include <algorithm>

void SortAndReplaceEvenCharsToKB(std::string& str) {
    std::sort(str.begin(), str.end(), std::greater<char>());
    std::string subString = "KB";
    for (size_t i = 1; i < str.length(); i += subString.length()) {
        str.replace(i, subString.length(), subString);
    }
}

int SumOfNumbers(const std::string& str) {
    int sum = 0;
    for (char c : str) {
        if (c >= '0' && c <= '9') {
            sum += c - '0';
        }
    }
    return sum;
}

bool isStringValid(const std::string& str) {
    int length = str.length();
    return length > 2 && length % 32 == 0;
}
