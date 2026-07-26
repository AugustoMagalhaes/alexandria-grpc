#include "service/CsvService.h"

#include <fstream>
#include <sstream>

namespace {

std::string escapeCsvField(const std::string& field)
{
    const bool needsQuoting = field.find(',') != std::string::npos
                              || field.find('"') != std::string::npos
                              || field.find('\n') != std::string::npos;

    if (!needsQuoting) {
        return field;
    }

    std::string escaped = "\"";
    for (char c : field) {
        if (c == '"') {
            escaped += "\"\"";
        } else {
            escaped += c;
        }
    }
    escaped += "\"";
    return escaped;
}

std::vector<std::string> parseCsvLine(const std::string& line)
{
    std::vector<std::string> fields;
    std::string current;
    bool insideQuotes = false;

    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];

        if (insideQuotes) {
            if (c == '"') {
                if (i + 1 < line.size() && line[i + 1] == '"') {
                    current += '"';
                    ++i;
                } else {
                    insideQuotes = false;
                }
            } else {
                current += c;
            }
        } else {
            if (c == '"') {
                insideQuotes = true;
            } else if (c == ',') {
                fields.push_back(current);
                current.clear();
            } else {
                current += c;
            }
        }
    }

    fields.push_back(current);
    return fields;
}

}

CsvService::CsvService(IBookRepository& repository)
    : m_repository(repository)
{
}

OperationResult CsvService::exportBooks(const std::string& filePath)
{
    std::ofstream file(filePath);

    if (!file.is_open()) {
        return OperationResult::fail("Could not open file for writing");
    }

    file << "title,author,isbn,total_copies\n";

    for (const Book& book : m_repository.findAll("")) {
        file << escapeCsvField(book.title) << ","
             << escapeCsvField(book.author) << ","
             << escapeCsvField(book.isbn) << ","
             << book.totalCopies << "\n";
    }

    return OperationResult::ok();
}

CsvImportResult CsvService::importBooks(const std::string& filePath, CsvImportMode mode)
{
    std::ifstream file(filePath);

    if (!file.is_open()) {
        CsvImportResult result;
        result.success = false;
        result.error = "Could not open file for reading";
        return result;
    }

    if (mode == CsvImportMode::Replace) {
        for (const Book& book : m_repository.findAll("")) {
            m_repository.remove(book.id);
        }
    }

    std::string line;
    std::getline(file, line);

    CsvImportResult result;
    result.success = true;

    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        std::vector<std::string> fields = parseCsvLine(line);

        if (fields.size() != 4) {
            ++result.skippedCount;
            continue;
        }

        Book book;
        book.title = fields[0];
        book.author = fields[1];
        book.isbn = fields[2];

        try {
            book.totalCopies = std::stoi(fields[3]);
        } catch (const std::exception&) {
            ++result.skippedCount;
            continue;
        }

        book.availableCopies = book.totalCopies;

        if (m_repository.create(book).has_value()) {
            ++result.importedCount;
        } else {
            ++result.skippedCount;
        }
    }

    return result;
}