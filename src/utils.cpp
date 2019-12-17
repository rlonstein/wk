#include "wk.hpp"
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include "yaml-cpp/yaml.h"


namespace wk {
  namespace utils {

    std::string commafyStrVec(std::vector<std::string> vec, std::string substitute) {
      std::string commafied = std::accumulate(
        std::begin(vec), std::end(vec), std::string(),
        [&substitute](std::string &ss, std::string &s) {
          return ss.empty() ? (substitute.empty() ? s : substitute) :
            ss+", "+ (substitute.empty() ? s : substitute);
        });
      return commafied;
    }
    
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
        out << YAML::Key << "created" << YAML::Value << (entry.created.empty() ? wk::utils::getCurrentDatetime() : entry.created);
        out << YAML::Key << "modified" << YAML::Value << (entry.modified.empty() ? wk::utils::getCurrentDatetime() : entry.modified);
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
          entry.tags = doc["tags"].as<wk::Tags>();
          entry.tagIds = wk::sql::RowIds(); // null it out
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
