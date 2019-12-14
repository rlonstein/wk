#include "wk.hpp"
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include "yaml-cpp/yaml.h"


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

    std::string getCurrentDatetime() {
      std::time_t t = std::time(nullptr);
      std::tm tm = *std::localtime(&t);
      std::stringstream timestr;
      timestr << std::put_time(&tm, "%FT%T%z");
      return timestr.str();
    }

    Entry editEntry(Entry entry) {
      VLOG(1) << "editEntry";
      const char* editor = std::getenv("EDITOR");
      if (editor) {
        /*
          This is *nix portable and uses only the std library but is
          unsafe against a hostile race condition where an attacker
          tries to guess the tmp file name between creation of the
          filename and opening of the file itself. mkstemp, etc. (at
          least on Linux) don't preserve the file and let the editor
          to open it after templating.
        */
        std::string tmpfn = std::tmpnam(nullptr);
        std::fstream tmpf(tmpfn, std::ios::binary|std::ios::out|std::ios::in|std::ios::trunc);
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "title" << YAML::Value << (entry.title.empty() ? "" : entry.title);
        out << YAML::Key << "created" << YAML::Value << (entry.created.empty() ? WK::UTILS::getCurrentDatetime() : entry.created);
        out << YAML::Key << "modified" << YAML::Value << (entry.modified.empty() ? WK::UTILS::getCurrentDatetime() : entry.modified);
        out << YAML::Key << "tags" << YAML::Value << YAML::BeginSeq;
        if (entry.tags.empty()) {
          out << "";
        } else {
          for (auto tag : entry.tags) {
            out << tag;
          }
        }
        out << YAML::EndSeq;
        out << YAML::Key << "text" << YAML::Value << (entry.text.empty() ? "" : entry.text);
        out << YAML::EndMap;
        VLOG(1) << "Writing to " << tmpfn << " YAML template:\n" << out.c_str();
        tmpf << "---\n";
        tmpf << out.c_str();
        tmpf.flush();
        tmpf.sync();
        tmpf.close();
        std::string cmd = fmt::sprintf("%s %s", editor, tmpfn);
        auto rc = std::system(cmd.c_str());
        if (rc) {
          LOG(ERROR) << "Editor failed (" << rc << ")";
          throw CLI::RuntimeError(rc);
        }
        try {
          YAML::Node doc = YAML::LoadFile(tmpfn);
          std::remove(tmpfn.c_str());
          entry.title = doc["title"].as<std::string>();
          entry.created = doc["created"].as<std::string>();
          entry.modified = doc["modified"].as<std::string>();
          entry.tags = doc["tags"].as<WK::Tags>();
          entry.text = doc["text"].as<std::string>();
        }
        catch (YAML::Exception e) {
          LOG(ERROR) << "YAML Exception: " << e.what();
        }
      }
      return entry;
    }
  }
}
