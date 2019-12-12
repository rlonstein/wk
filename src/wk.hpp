#include <vector>
#include <set>
#include <string>
#include <sstream>

#include "fmt/format.h"
#include "fmt/printf.h"
#include "fmt/ostream.h"
#include "CLI/CLI.hpp"
#include "easylogging++.h"

#ifndef H_WK
#define H_WK
#pragma once

namespace WK {
  enum class ImportFileFormats { json, yaml };
  const std::vector<std::pair<const char*, ImportFileFormats>> ImportFormatNames = {
    {"json", ImportFileFormats::json}, {"yaml", ImportFileFormats::yaml}
  };
  enum class ExportFileFormats { json, yaml, markdown };
  const std::vector<std::pair<const char*, ExportFileFormats>> ExportFormatNames = {
    {"json", ExportFileFormats::json}, {"yaml", ExportFileFormats::yaml}, {"markdown", ExportFileFormats::markdown}
  };

  namespace CMDS {
    void addEntry(std::string title, std::vector<std::string> tags, std::string text);
    void deleteEntry(std::string title);
    void editEntry(std::string title);
    void exportWiki(std::string filename, std::string format, std::string title, std::vector<std::string> tags);
    void importWiki(std::string filename, std::string format);
    void newWiki(std::string filename);
    void searchWiki(std::vector<std::string> keywords);
  }
  namespace UTILS {
    constexpr std::string_view ENVPATHS[3][3] {
      {"XDG_DATA_HOME", "", "wk.sqlite"},
      {"HOME", ".local/share/", "wk.sqlite"},
      {"HOME", "", ".wk.sqlite"}
    };

    bool envVarPathExists(std::string envvarname);
    std::string findDB();
  }
}
#endif
