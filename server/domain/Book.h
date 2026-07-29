#pragma once

#include <string>

struct Book {
    int id = 0;
    std::string title;
    std::string author;
    std::string isbn;
    std::string schoolCode;
    std::string category;
    std::string keywords;
    bool borrowable = true;
    int totalCopies = 0;
    int availableCopies = 0;
};