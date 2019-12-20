#include <string>
#include <vector>
#include <unordered_map>

#pragma once

namespace wk {
  namespace sql {
    typedef long long RowId;
    typedef std::vector<RowId> RowIds;
    // Magic number. Not invalid for SQLite, which are 64-bit signed
    // ints (though start at zero by default), just invalid to us.
    constexpr RowId INVALID_ROWID = -1;
  }

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

  typedef std::string TagName;
  typedef std::vector<TagName> TagNames;
  
  typedef struct Tag {
    TagName name;
    wk::sql::RowId id;
    bool operator==(const Tag& rhs) const {
      return (name == rhs.name && id == rhs.id);
    }

    bool operator!=(const Tag& rhs) const {
      return !(name == rhs.name && id == rhs.id);
    }

    bool operator<(const Tag& rhs) const {
      return (name < rhs.name);
    }
  } Tag;

  typedef std::vector<Tag> Tags;

  typedef struct Entry {
    bool populated;
    std::string title;
    std::string created;
    std::string modified;
    std::string text;
    wk::Tags tags;
    wk::sql::RowId entryId;

    bool operator==(const Entry& rhs) const {
      return std::tie(title, created, modified, text, tags) ==
        std::tie(rhs.title, rhs.created, rhs.modified, rhs.text, rhs.tags);
    }

    bool operator!=(const Entry& rhs) const {
      return !(*this == rhs);
    }

  } Entry;  
}
