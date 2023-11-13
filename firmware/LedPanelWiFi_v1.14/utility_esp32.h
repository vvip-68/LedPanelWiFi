#pragma once
#include <string>
// esp32 specific functions

/**
 * @brief считает число символов-разделителей в строке, возвращает число разделенных блоков
 * 
 * @param str строка
 * @param separator - символ разделитель
 * @return size_t - число разделенных блоков (число разделителей +1)
 */
size_t countTokens(const std::string &str, char separator);

/**
 * @brief ищет подстроку в строке с разделителями
 * 
 * @param str исходная строка
 * @param index номер блока между разделителями
 * @param separator разделитель
 * @return std::string подстрока
 */
std::string getToken(const std::string &str, uint32_t index, char separator);