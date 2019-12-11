#include "wk.hpp"
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
  
  std::string schema_tags_entries =
    "CREATE TABLE tags(tag_id INTEGER UNIQUE NOT NULL PRIMARY KEY, tag TEXT UNIQUE);"
    "CREATE UNIQUE INDEX tagidx1 on tags(tag);"
    "CREATE TABLE entries(entry_id INTEGER UNIQUE NOT NULL PRIMARY KEY, title TEXT, content TEXT);"
    "CREATE UNIQUE INDEX entryidx1 on entries(content);";
  std::string schema_taglist =
    "PRAGMA foreign_keys = ON;"
    "CREATE TABLE taglist(taglist_id INTEGER UNIQUE NOT NULL PRIMARY KEY,"
    "entry_id INTEGER NOT NULL, tag_id INTEGER NOT NULL,"
    "FOREIGN KEY(entry_id) REFERENCES entries(entry_id) ON DELETE CASCADE ON UPDATE CASCADE,"
    "FOREIGN KEY(tag_id) REFERENCES tags(entry_id) ON DELETE CASCADE ON UPDATE CASCADE);";

  try {
    SQLite::Database db(filename, SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE);
    VLOG(1) << "opened sqlite db";
    db.exec(schema_tags_entries);
    VLOG(1) << "executed " << schema_tags_entries;
    db.exec(schema_taglist);
    VLOG(1) << "executed " << schema_taglist;
  }
  catch (SQLite::Exception e) {
    LOG(ERROR) << "SQLite error occurred: " << e.what();
    throw CLI::RuntimeError(-1);
  }
  throw CLI::Success();
}
