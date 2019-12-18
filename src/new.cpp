#include "wk.hpp"
#include "sql.hpp"
#include <cstdlib>

void wk::cmds::newWiki(std::string filename) {
#ifndef USE_ALT_STDFS
  namespace fs = std::filesystem;
#else
  namespace fs = ghc::filesystem;
#endif
  if (filename.empty()) {
    for (auto ep : wk::utils::ENVPATHS) {
      std::string candidate = ep[0].data();
      VLOG(1) << "Checking envvar path " << candidate;
      if (wk::utils::envVarPathExists(candidate)) {
        fs::path p(std::getenv(ep[0].data()));
        p /= std::string(ep[1]);
        VLOG(1) << "Checking for " << p;
        if (fs::exists(p)) {
          p /= std::string(ep[2]);
          filename = p;
          VLOG(1) << "Will create " << filename;
          break;
        }
      }
    }
  }
  if (filename.empty()) {
    LOG(ERROR) << "Cannot create new wiki, default locations undefined and no filename given";
    throw CLI::RuntimeError(-1);
  }
  if (fs::exists(filename)) {
    LOG(ERROR) << "Refusing to overwrite existing wiki at " << filename;
    throw CLI::RuntimeError(-1);
  }
  
  try {
    SQLite::Database db(filename, SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE);
    VLOG(1) << "opened sqlite db";
    db.exec(wk::sql::newSchemaTagsEntries);
    VLOG(1) << "executed " << wk::sql::newSchemaTagsEntries;
    db.exec(wk::sql::newSchemaTaglist);
    VLOG(1) << "executed " << wk::sql::newSchemaTaglist;
  }
  catch (SQLite::Exception e) {
    LOG(ERROR) << "sqlite error occurred: " << e.what();
    throw CLI::RuntimeError(-1);
  }
  throw CLI::Success();
}
