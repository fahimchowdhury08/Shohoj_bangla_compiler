#ifndef UTF8UTILS_H
#define UTF8UTILS_H

#include <string>
#include <stdexcept>

// Bengali letters/digits are multi-byte in UTF-8, so we cannot lex the raw
// std::string one 'char' (byte) at a time the way the Java version lexes
// one 'char' (UTF-16 code unit) at a time. Instead we decode the whole
// source into a sequence of Unicode codepoints (char32_t) up front, lex
// over that, and re-encode individual lexemes back to UTF-8 for storage
// and printing.

inline std::u32string utf8Decode(const std::string& s) {
    std::u32string out;
    size_t i = 0;
    while (i < s.size()) {
        unsigned char c = s[i];
        char32_t cp;
        int extra;
        if ((c & 0x80) == 0x00) { cp = c; extra = 0; }
        else if ((c & 0xE0) == 0xC0) { cp = c & 0x1F; extra = 1; }
        else if ((c & 0xF0) == 0xE0) { cp = c & 0x0F; extra = 2; }
        else if ((c & 0xF8) == 0xF0) { cp = c & 0x07; extra = 3; }
        else { throw std::runtime_error("Invalid UTF-8 byte in source file."); }

        if (i + extra >= s.size() + 1 && extra > 0 && i + 1 + extra > s.size()) {
            throw std::runtime_error("Truncated UTF-8 sequence in source file.");
        }
        for (int k = 1; k <= extra; k++) {
            unsigned char cc = s[i + k];
            if ((cc & 0xC0) != 0x80) throw std::runtime_error("Malformed UTF-8 continuation byte.");
            cp = (cp << 6) | (cc & 0x3F);
        }
        out.push_back(cp);
        i += extra + 1;
    }
    return out;
}

inline void utf8EncodeAppend(std::string& out, char32_t cp) {
    if (cp <= 0x7F) {
        out.push_back(static_cast<char>(cp));
    } else if (cp <= 0x7FF) {
        out.push_back(static_cast<char>(0xC0 | (cp >> 6)));
        out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    } else if (cp <= 0xFFFF) {
        out.push_back(static_cast<char>(0xE0 | (cp >> 12)));
        out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    } else {
        out.push_back(static_cast<char>(0xF0 | (cp >> 18)));
        out.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    }
}

inline std::string utf8Encode(const std::u32string& s) {
    std::string out;
    for (char32_t cp : s) utf8EncodeAppend(out, cp);
    return out;
}

inline std::string utf8Encode(char32_t cp) {
    std::string out;
    utf8EncodeAppend(out, cp);
    return out;
}

#endif
