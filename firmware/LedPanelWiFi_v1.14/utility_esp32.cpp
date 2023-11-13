#ifdef ESP32

#include <Arduino.h>
#include "utility_esp32.h"

size_t countTokens(const std::string &str, char separator){
    size_t cnt{0};
    for (auto c : str){
        if ( std::char_traits<char>::eq(c, separator))
            ++cnt;
    }
    return ++cnt;
}

std::string getToken( std::string const& str, uint32_t index, char separator){
    // пустые строки и 0й индекс не обрабатываем
    if (!index || !str.length())
        return std::string(str);

    size_t p_start{0}, p_end{0};
    do {
        p_start = p_end;
        p_end = str.find(separator, p_start+1);
        if (std::string::npos == p_end){
            // not found
            p_end = str.length();
            break;
        }
    } while(--index);

    if (p_start) ++p_start;   // remove leading comma
    return index<2 ?  str.substr(p_start, p_end-p_start) : std::string();
}

#endif  // ESP32