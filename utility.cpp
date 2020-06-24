#include <string>

const std::string whitespaceChars = " \n\r\t";

std::string trim(const std::string &inStr) {
    std::string str(inStr);

    std::string::size_type pos;

    pos = str.find_first_not_of(whitespaceChars);
    if (pos > 0) str.erase(0, pos);
    pos = str.find_last_not_of(whitespaceChars);
    str.erase(pos + 1);

    return str;
}

bool g_isletter(int c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool g_isspace(int c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

int countWords(const std::string &line) {
    bool inWord = false;
    int count = 0;
    for (char c : line) {
        if (g_isletter(c)) {
            if (!inWord) {
                inWord = true;
                ++count;
            }
        } else if (g_isspace(c)) {
            inWord = false;
        }
    }
    return count;
}
