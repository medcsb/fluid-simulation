#ifndef LOG_UTILS_HPP
#define LOG_UTILS_HPP


#include <iostream>
#include <stdexcept>

#include <string>

struct TextColors {
    std::string ESC = "\033[";
    std::string RESET = ESC + "0m";
    std::string BOLD = ESC + "1m";
    std::string RED = ESC + "31m";
    std::string GREEN = ESC + "32m";
    std::string YELLOW = ESC + "33m";
    std::string BLUE = ESC + "34m";
    std::string ORANGE = ESC + "35m";
    std::string BLUE_BG = ESC + "44m";
};

inline void softWarn(const std::string& message) {
    TextColors colors;
    std::cerr << colors.BOLD << colors.BLUE << "careful: " << colors.RESET << message << std::endl;
}

inline void warn(const std::string& message) {
    TextColors colors;
    std::cerr << colors.BOLD << colors.YELLOW << "Warning: " << colors.RESET << message << std::endl;
}

inline void error(const std::string& message) {
    TextColors colors;
    std::cerr << colors.BOLD << colors.RED << "Error: " << colors.RESET << message << std::endl;
}



#endif // LOG_UTILS_HPP
