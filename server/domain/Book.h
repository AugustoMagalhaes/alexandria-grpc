#pragma once

#include <string>

struct Book {
    int id = 0;
    std::string title;
    std::string author;
    std::string isbn;
    int totalCopies = 0;
    int availableCopies = 0;
};