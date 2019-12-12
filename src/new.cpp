#include "wk.hpp"
#include "sql.hpp"
#include <cstdlib>
#include <filesystem>

void WK::CMDS::newWiki(std::string filename) {
  if (filename.empty()) {
    for (auto ep : WK::UTILS::ENVPATHS) {
      if (WK::UTILS::envVarPathExists(ep[0].data())) {
        std::filesystem::path p(std::getenv(ep[0].data()));
        p /= ep[1];
        p /= ep[2];
        filename = p;
        VLOG(1) << "Will create " << filename;
        break;
      }
    }
  }
  if (filename.empty()) {
    LOG(ERROR) << "Cannot create new wiki, default locations undefined and no filename given";
    throw CLI::RuntimeError(-1);
  }
  if (std::filesystem::exists(filename)) {
    LOG(ERROR) << "Refusing to overwrite existing wiki at " << filename;
    throw CLI::RuntimeError(-1);
  }
  
  try {
    SQLite::Database db(filename, SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE);
    VLOG(1) << "opened sqlite db";
    db.exec(WK::SQL::newSchemaTagsEntries);
    VLOG(1) << "executed " << WK::SQL::newSchemaTagsEntries;
    db.exec(WK::SQL::newSchemaTaglist);
    VLOG(1) << "executed " << WK::SQL::newSchemaTaglist;
  }
  catch (SQLite::Exception e) {
    LOG(ERROR) << "SQLite error occurred: " << e.what();
    throw CLI::RuntimeError(-1);
  }
  throw CLI::Success();
}
