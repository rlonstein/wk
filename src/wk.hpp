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
  namespace CMDS {
    void addEntry(std::string title, std::vector<std::string> tags, std::string text);
    void deleteEntry(std::string title);
    void editEntry(std::string title);
    void exportWiki(std::string filename, std::string format, std::string title, std::vector<std::string> tags);
    void importWiki(std::string filename, std::string format);
    void newWiki(std::string filename);
    void searchWiki(std::vector<std::string> keywords);
  }
}
#endif
