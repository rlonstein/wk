#include "wk.hpp"
#include <numeric>

void WK::CMDS::addEntry(std::string title, std::vector<std::string> tags, std::string text) {
  if (VLOG_IS_ON(1)) {
    std::string tagstr = std::accumulate(
      std::begin(tags), std::end(tags), std::string(),
      [](std::string &ss, std::string &s) { return ss.empty() ? "'"+s+"'" : ss+", '"+s+"'"; } );
    VLOG(1) << "invoked add('" << title << "', [" << tagstr << "], '" << text << "')";
  }
  try {
    auto dbfqn = WK::UTILS::findDB();
    SQLite::Database db(dbfqn, SQLite::OPEN_READWRITE);
    std::string sql_tagQuery = std::accumulate(
      tags.begin(), tags.end(), std::string(),
      [](std::string &ss, std::string &s){ return ss.empty() ? "?" : ss+", ?"; });
    sql_tagQuery = "SELECT rowid, tag FROM tags WHERE tag in (" + sql_tagQuery + ")";
    SQLite::Statement queryTags(db, sql_tagQuery);
    for (std::size_t i=0; i<tags.size(); i++) {
      queryTags.bind(i+1, tags[i]);
    }
    VLOG(1) << "executing tag query '" << sql_tagQuery << "'";
    std::vector<std::string> existingTags;
    std::vector<long long> tagRowIds;
    while (queryTags.executeStep()) {
      tagRowIds.push_back(queryTags.getColumn(0));
      existingTags.push_back(queryTags.getColumn(1));
    }
    std::vector<std::string> newTags;
    if (tags.size() > existingTags.size()) {
      std::sort(tags.begin(), tags.end());
      std::sort(existingTags.begin(), existingTags.end());
      std::set_difference(tags.begin(), tags.end(),
                          existingTags.begin(), existingTags.end(),
                          std::inserter(newTags, newTags.begin()));
    }
    SQLite::Transaction transaction(db);
    db.exec("PRAGMA foreign_keys = ON;");
    SQLite::Statement insertTag(db, "INSERT INTO tags VALUES(NULL, ?)");
    for (auto const& tag : newTags) {
      VLOG(1) << "Adding tag '" << tag << "'";
      insertTag.bind(1, tag);
      insertTag.exec();
      tagRowIds.push_back(db.getLastInsertRowid());
      insertTag.clearBindings();
      insertTag.reset();
    }
    VLOG(1) << "Adding entry";
    SQLite::Statement insertEntry(db, "INSERT INTO entries VALUES (NULL, ?, ?)");
    SQLite::bind(insertEntry, std::make_tuple(title, text));
    insertEntry.exec();
    long long entryRowId = db.getLastInsertRowid();

    SQLite::Statement insertEntryTagMapping(db, "INSERT INTO taglist VALUES (NULL, ?, ?)");
    for (auto tagRowId : tagRowIds) {
      VLOG(1) << "mapping entry " << entryRowId << " with tagRowId " << tagRowId;
      SQLite::bind(insertEntryTagMapping, std::make_tuple(entryRowId, tagRowId));
      insertEntryTagMapping.exec();
      insertEntryTagMapping.clearBindings();
      insertEntryTagMapping.reset();
    }
    VLOG(1) << "Completing transaction";
    transaction.commit();
  }
  catch (SQLite::Exception e) {
    LOG(ERROR) << "SQLite error: " << e.what();
    throw CLI::RuntimeError(-1);
  }
  throw CLI::Success();
}
