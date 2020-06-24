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

std::string& replaceAll(std::string &text, const std::string &from, const std::string &to) {
    std::string::size_type pos = text.find(from);
    while (pos != std::string::npos) {
        text.replace(pos, from.size(), to);
        pos = text.find(from);
    }
    return text;
}