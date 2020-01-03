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
}
#endif
