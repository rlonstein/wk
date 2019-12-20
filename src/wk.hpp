#include <algorithm>
#include <iterator>
#include <numeric>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "defs.hpp"

#include "fmt/format.h"
#include "fmt/printf.h"
#include "fmt/ostream.h"
#include "CLI/CLI.hpp"
#include "easylogging++.h"

#ifndef H_wk
#define H_wk
#pragma once

namespace wk {
  namespace cmds {
    void addEntry(wk::Entry entry);
    void deleteEntry(std::string title);
    void editEntry(std::string title);
    void exportWiki(std::string filename, std::string format, std::string title, wk::TagNames tags);
    void importWiki(std::string filename, std::string format);
    void newWiki(std::string filename);
    void searchWiki(wk::TagNames keywords);
  }
  namespace utils {
    constexpr std::string_view ENVPATHS[3][3] {
      {"XDG_DATA_HOME", "", "wk.sqlite"},
      {"HOME", ".local/share/", "wk.sqlite"},
      {"HOME", "", ".wk.sqlite"}
    };

    std::string commafyStrVec(std::vector<std::string> vec, std::string substitute);
    std::string getCurrentDatetime();
    bool envVarPathExists(std::string envvarname);
    wk::Entry editEntry(wk::Entry);
    wk::TagNames getTagNamesFromTags(wk::Tags);
  }
}
#endif
