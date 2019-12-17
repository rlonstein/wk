#include <string>
#include <vector>
#include <unordered_map>

#pragma once

namespace wk {
  namespace sql {
    typedef long long RowId;
    typedef std::vector<RowId> RowIds;
  }

  typedef std::string Tag;
  typedef std::vector<Tag> Tags;
  
  enum class ImportFileFormat { json, yaml };
  typedef std::pair<const char*, ImportFileFormat> ImportNameFormatPair;
  const std::vector<ImportNameFormatPair> ImportFormatNames = {
    {"json", ImportFileFormat::json},
    {"yaml", ImportFileFormat::yaml}
  };
  const std::unordered_map<std::string, ImportFileFormat> ImportFormatNameMap = {
    {"json", ImportFileFormat::json},
    {"yaml", ImportFileFormat::yaml}
  };
  
  enum class ExportFileFormat { json, yaml, markdown };
  typedef std::pair<const char*, ExportFileFormat> ExportNameFormatPair;
  const std::vector<ExportNameFormatPair> ExportFormatNames = {
    {"json", ExportFileFormat::json},
    {"yaml", ExportFileFormat::yaml},
    {"markdown", ExportFileFormat::markdown}
  };
  const std::unordered_map<std::string, ExportFileFormat> ExportFormatNameMap = {
    {"json", ExportFileFormat::json},
    {"yaml", ExportFileFormat::yaml},
    {"markdown", ExportFileFormat::markdown}
  };

  typedef struct {
    bool populated;
    std::string title;
    std::string created;
    std::string modified;
    std::string text;
    wk::Tags tags;
    wk::sql::RowIds tagIds;
    wk::sql::RowId entryId;
  } Entry;  
}



    
