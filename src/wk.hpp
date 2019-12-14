#include <vector>
#include <string>
#include <sstream>
#include <utility>
#include <numeric>
#include "fmt/format.h"
#include "fmt/printf.h"
#include "fmt/ostream.h"
#include "CLI/CLI.hpp"
#include "easylogging++.h"

#ifndef H_WK
#define H_WK
#pragma once

namespace WK {

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

  typedef std::vector<std::string> Tags;
  typedef struct {
    std::string title;
    std::string created;
    std::string modified;
    std::string text;
    Tags tags;
  } Entry;

  namespace CMDS {
    void addEntry(Entry entry);
    void deleteEntry(std::string title);
    void editEntry(std::string title);
    void exportWiki(std::string filename, std::string format, std::string title, Tags tags);
    void importWiki(std::string filename, std::string format);
    void newWiki(std::string filename);
    void searchWiki(Tags keywords);
  }
  namespace UTILS {
    constexpr std::string_view ENVPATHS[3][3] {
      {"XDG_DATA_HOME", "", "wk.sqlite"},
      {"HOME", ".local/share/", "wk.sqlite"},
      {"HOME", "", ".wk.sqlite"}
    };

    std::string getCurrentDatetime();
    bool envVarPathExists(std::string envvarname);
    std::string findDB();
    Entry editEntry(Entry);
  }
}
#endif
