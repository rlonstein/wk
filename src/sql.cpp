#include "wk.hpp"
#include "sql.hpp"

/*
 * DB related conveniences and common routines
 */

extern template wk::Entry wk::sql::getEntry(SQLite::Database* dbptr, wk::sql::RowId entryId);
extern template wk::Entry wk::sql::getEntry(SQLite::Database* dbptr, wk::sql::RowId entryId);
extern template wk::Entry wk::sql::getEntry(wk::sql::RowId entryId);
extern template wk::Entry wk::sql::getEntry(std::string title);

std::string wk::sql::findDB() {
  VLOG(1) << "searching for db";
  // candidates are:
  // - $XDG_DATA_HOME/wk.sqlite,
  // - $HOME/.local/share/wk.sqlite,
  // - $HOME/.wk.sqlite
  namespace fs = std::filesystem;
  for (auto ep : wk::utils::ENVPATHS) {
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

std::vector<std::pair<wk::Tag, wk::sql::RowId>> wk::sql::queryTagInfo(RowId entryId) {
  auto dbfqn = wk::sql::findDB();
  if (dbfqn.empty()) {
    LOG(ERROR) << "No wiki database found!";
    throw CLI::RuntimeError(-1);
  }
  std::vector<std::pair<wk::Tag, wk::sql::RowId>> results;
  SQLite::Database db(dbfqn, SQLite::OPEN_READONLY);
  SQLite::Statement sql(db, wk::sql::queryTagsByEntryId);
  sql.bind(1, entryId);
  while (sql.executeStep()) {
    results.push_back(std::make_pair(sql.getColumn("TagId"),
                                     sql.getColumn("Tag")));
  }
  return results;
}

