#include "wk.hpp"

void WK::CMDS::searchWiki(std::vector<std::string> keywords) {

  if (keywords.empty()) {
    LOG(ERROR) << "No keywords specified!";
    throw CLI::RuntimeError(-1);
  }
  auto dbfqn = WK::UTILS::findDB();
  if (dbfqn.empty()) {
    LOG(ERROR) << "No wiki database found!";
    throw CLI::RuntimeError(-1);
  }
  std::string sql;
  SQLite::Database db(dbfqn, SQLite::OPEN_READONLY);

  // First, search for tags matching the keywords
  std::vector<long long> tagRowIds;
  std::string tagQueryBinding = std::accumulate(
    keywords.begin(), keywords.end(), std::string(),
    [](std::string &ss, std::string &s){ return ss.empty() ? "?" : ss+", ?"; });
  sql = "SELECT tag_id FROM tags WHERE tag in (" + tagQueryBinding + ")";
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
    LOG(ERROR) << "SQLite error: " << e.what();
    throw CLI::RuntimeError(-1);
  }
  VLOG(1) << "found " << tagRowIds.size();

  // Using the tag rowids, collect unique titles by entryid.
  // Use multiple queries to avoid using FTS3/4 which might not be
  // compiled into the local SQLite lib
  std::unordered_map<long long, std::tuple<std::string, std::string>> entries;
  sql =
    "SELECT entries.entry_id, entries.title, entries.content "
    "FROM entries, taglist WHERE entries.entry_id = taglist.entry_id AND taglist.tag_id = ?";
  try {
    VLOG(1) << "Querying entries with matching keyword mappings";
    SQLite::Statement queryEntries(db, sql);
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
    LOG(ERROR) << "SQLite error: " << e.what();
    throw CLI::RuntimeError(-1);
  }

  // Next, search for titles with the keywords in them, adding to the hashmap
  sql = "SELECT entry_id, title, content FROM entries WHERE title LIKE ?";
  try {
    VLOG(1) << "Querying titles matching keywords";
    SQLite::Statement queryTitles(db, sql);
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
    LOG(ERROR) << "SQLite error: " << e.what();
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
