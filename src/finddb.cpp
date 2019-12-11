#include "wk.hpp"
#include <cstdlib>
#include <filesystem>

namespace WK {
  namespace UTILS {
    bool envVarPathExists(std::string envvarname) {
      namespace fs = std::filesystem;
      const char* envvar = std::getenv(envvarname.c_str());
      if (envvar) {
        fs::path path(envvar);
        if (fs::exists(path)) {
          return true;
        }
      }
      return false;
    }
    
    std::string findDB() {
      VLOG(1) << "searching for db";
      // candidates are:
      // - $XDG_DATA_HOME/wk.sqlite,
      // - $HOME/.local/share/wk.sqlite,
      // - $HOME/.wk.sqlite
      namespace fs = std::filesystem;
      for (auto ep : WK::UTILS::ENVPATHS) {
        const char* envvar = std::getenv(ep[0].data());
        if (! envvar) {
          VLOG(1) << "skipping " << ep[0] << ", unset";
          continue;
        }
        VLOG(1) << "envvar " << ep[0] << " has value " << envvar;
        fs::path path(envvar);
        path /= ep[1];
        path /= ep[2];
        VLOG(1) << "checking for " << path;
        if (fs::is_regular_file(path)) {
          VLOG(1) << "Located db file at " << path;
          return path;
        }
      }
      LOG(ERROR) << "No wiki database found!";
      return "";
    }
  }
}
