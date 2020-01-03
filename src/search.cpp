#include "wk.hpp"
#include "utils.hpp"
#include "sql.hpp"

void wk::cmds::searchWiki(wk::TagNames keywords) {

  if (keywords.empty()) {
    LOG(ERROR) << "No keywords specified!";
    throw CLI::RuntimeError(-1);
  }
  auto dbfqn = wk::sql::findDB();
  if (dbfqn.empty()) {
    LOG(ERROR) << "No wiki database found!";
    throw CLI::RuntimeError(-1);
  }
  std::string sql;
  SQLite::Database db(dbfqn, SQLite::OPEN_READONLY);

  // First, search for tags matching the keywords
  std::vector<wk::sql::RowId> tagRowIds;
  std::string tagQueryBinding = wk::utils::commafyStrVec(keywords, "?");
  sql = fmt::sprintf(wk::sql::templateQueryTags, tagQueryBinding);
  try {
    VLOG(1) << "Querying tags for " << keywords.size() << " keywords";
    SQLite::Statement queryTags(db, sql);
    for (std::size_t i=0; i<keywords.size(); i++) {
      queryTags.bind(i+1, keywords[i]);
    }
    while (queryTags.executeStep()) {
      tagRowIds.push_back(queryTags.getColumn(0));
    }
  }
  catch (SQLite::Exception e) {
    LOG(ERROR) << "sqlite error: " << e.what();
    throw CLI::RuntimeError(-1);
  }
  VLOG(1) << "found " << tagRowIds.size();

  // Using the tag rowids, collect unique titles by entryid.
  // Use multiple queries to avoid using FTS3/4 which might not be
  // compiled into the local sqlite lib
  std::unordered_map<wk::sql::RowId, std::tuple<std::string, std::string>> entries;

  try {
    VLOG(1) << "Querying entries with matching keyword mappings";
    SQLite::Statement queryEntries(db, wk::sql::queryEntriesMatchingTags);
    for (auto rowId : tagRowIds) {
      queryEntries.bind(1, rowId);
      while (queryEntries.executeStep()) {
        entries.insert({queryEntries.getColumn(0),
                        std::make_tuple(queryEntries.getColumn(1),
                                        queryEntries.getColumn(2))});
      }
      queryEntries.clearBindings();
      queryEntries.reset();
    }
  }
  catch (SQLite::Exception e) {
    LOG(ERROR) << "sqlite error: " << e.what();
    throw CLI::RuntimeError(-1);
  }

  // Next, search for titles with the keywords in them, adding to the hashmap
  try {
    VLOG(1) << "Querying titles matching keywords";
    SQLite::Statement queryTitles(db, wk::sql::queryEntriesTitleLike);
    for (auto keyword : keywords ) {
      queryTitles.bind(1, keyword);
      while (queryTitles.executeStep()) {
        entries.insert({queryTitles.getColumn(0),
                        std::make_tuple(queryTitles.getColumn(1),
                                        queryTitles.getColumn(2))});
      }
      queryTitles.clearBindings();
      queryTitles.reset();
    }
  }
  catch (SQLite::Exception e) {
    LOG(ERROR) << "sqlite error: " << e.what();
    throw CLI::RuntimeError(-1);
  }

  // Last print the collected entries that matched
  for (auto& rec : entries) {
    fmt::printf("%s\n%s\n---\n",
                std::get<0>(rec.second),
                std::get<1>(rec.second));
  }
  throw CLI::Success();
}
