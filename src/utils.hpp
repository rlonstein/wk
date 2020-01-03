#include "wk.hpp"
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <sstream>
#include "yaml-cpp/yaml.h"

#ifndef USE_ALT_STDFS
 #include <filesystem>
#else
 #include "ghc/filesystem.hpp"
#endif

namespace wk {
  namespace utils {
    constexpr std::string_view ENVPATHS[3][3] {
      {"XDG_DATA_HOME", "", "wk.sqlite"},
      {"HOME", ".local/share/", "wk.sqlite"},
      {"HOME", "", ".wk.sqlite"}
    };

    std::string getCurrentDatetime();
    bool envVarPathExists(std::string envvarname);
    wk::Entry editEntry(wk::Entry);
    wk::TagNames getTagNamesFromTags(wk::Tags);

    template <typename T> std::string commafyStrVec(T vec);
    template <typename T, typename U> std::string commafyStrVec(T vec, U substitute);
    template <> inline std::string commafyStrVec(std::vector<std::string> vec, std::string substitute) {
      std::string commafied = std::accumulate(
        std::begin(vec), std::end(vec), std::string(),
        [&substitute](std::string &ss, std::string &s) {
          return ss.empty() ? (substitute.empty() ? s : substitute) :
            ss+", "+ (substitute.empty() ? s : substitute);
        });
      return commafied;
    }
    template <> inline std::string commafyStrVec(std::vector<std::string> vec, const char* substitute) {
      return commafyStrVec(vec, std::string(substitute));
    }
    template <> inline std::string commafyStrVec(std::vector<std::string> vec) {
      return commafyStrVec(vec, std::string());
    }
  }
}
