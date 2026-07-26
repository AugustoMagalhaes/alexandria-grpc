#pragma once

#include <string>

#include "repository/IBookRepository.h"
#include "service/OperationResult.h"

enum class CsvImportMode {
    Append,
    Replace
};

struct CsvImportResult {
    bool success = false;
    std::string error;
    int importedCount = 0;
    int skippedCount = 0;
};

class CsvService {
public:
    explicit CsvService(IBookRepository& repository);

    OperationResult exportBooks(const std::string& filePath);
    CsvImportResult importBooks(const std::string& filePath, CsvImportMode mode);

private:
    IBookRepository& m_repository;
};