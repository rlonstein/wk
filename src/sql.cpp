#include "wk.hpp"
#include "sql.hpp"

/*
 * DB related conveniences and common routines
 */
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

template <> wk::Entry wk::sql::getEntry(wk::sql::RowId entryId) {
  auto dbfqn = wk::sql::findDB();
  if (dbfqn.empty()) {
    LOG(ERROR) << "No wiki database found!";
    throw CLI::RuntimeError(-1);
  }
  SQLite::Database db(dbfqn, SQLite::OPEN_READONLY);
  return wk::sql::getEntry(&db, entryId);
}

template <> wk::Entry wk::sql::getEntry(SQLite::Database* dbptr, std::string title) {
  wk::sql::RowId entryId;
  SQLite::Statement queryTitle(*dbptr, wk::sql::queryEntryIdsFirstTitle);
  queryTitle.bind(1, title);
  if (! queryTitle.executeStep()) {
    LOG(ERROR) << "No title '" << title << "' found";
    throw CLI::RuntimeError(-1);
  }   
  entryId = queryTitle.getColumn(0);
  return wk::sql::getEntry(&dbptr, entryId);
}

template <> wk::Entry wk::sql::getEntry(std::string title) {
  auto dbfqn = wk::sql::findDB();
  if (dbfqn.empty()) {
    LOG(ERROR) << "No wiki database found!";
    throw CLI::RuntimeError(-1);
  }
  SQLite::Database db(dbfqn, SQLite::OPEN_READONLY);
  return wk::sql::getEntry(&db, title);
}

template <> wk::Entry wk::sql::getEntry(SQLite::Database* dbptr, wk::sql::RowId entryId) {
  wk::Entry entry;
  try {
    SQLite::Statement queryEntry(*dbptr, wk::sql::queryEntryById);
    queryEntry.bind(1, entryId);
    if (! queryEntry.executeStep()) {
      LOG(WARNING) << "Entry #" << entryId << " not found";
      entry.populated = false;
      return entry;
    }
    entry.populated = true;
    entry.title = std::string(queryEntry.getColumn("title"));
    entry.created = std::string(queryEntry.getColumn("created"));
    entry.modified = std::string(queryEntry.getColumn("modified"));
    entry.text = std::string(queryEntry.getColumn("content"));
    SQLite::Statement queryTags(*dbptr, wk::sql::queryTagsByEntryId);
    queryTags.bind(1, entryId);
    while (queryTags.executeStep()) {
      entry.tags.push_back(queryTags.getColumn(0));
      entry.tagIds.push_back(queryTags.getColumn(1));
    }
  }
  catch (SQLite::Exception e) {
    LOG(ERROR) << "SQLite Error: " << e.what();
    throw CLI::RuntimeError(-1);
  }
  return entry;  
}
