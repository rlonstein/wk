#include "wk.hpp"
#include "sql.hpp"
#include <iterator>
#include <set>

void WK::CMDS::addEntry(Entry entry) {
  if (VLOG_IS_ON(1)) {
    std::string tagstr = WK::UTILS::commafyStrVec(entry.tags, std::string());
    VLOG(1) << "invoked add('" << entry.title << "', [" << tagstr << "], '" << entry.text
            << "', '" << entry.created << "', '" << entry.modified << "')";
  }

  if (entry.created.empty()) {
    entry.created = WK::UTILS::getCurrentDatetime();
  }
  if (entry.modified.empty()) {
    entry.modified = WK::UTILS::getCurrentDatetime();
  }
  
  int tries = 1;
  while (tries-- > 0 && (entry.title.empty() || entry.tags.empty() || entry.text.empty())) {
    LOG(WARNING) << "Missing parameters";
    Entry e = WK::UTILS::editEntry(entry);
  }
  auto dbfqn = WK::UTILS::findDB();
  if (dbfqn.empty()) {
    LOG(ERROR) << "No wiki database found!";
    throw CLI::RuntimeError(-1);
  }

  try {
    SQLite::Database db(dbfqn, SQLite::OPEN_READWRITE);
    std::string sql_tagQuery = WK::UTILS::commafyStrVec(entry.tags, "?");
    sql_tagQuery = "SELECT rowid, tag FROM tags WHERE tag in (" + sql_tagQuery + ")";
    SQLite::Statement queryTags(db, sql_tagQuery);
    for (std::size_t i=0; i<entry.tags.size(); i++) {
      queryTags.bind(i+1, entry.tags[i]);
    }
    VLOG(1) << "executing tag query '" << sql_tagQuery << "'";
    Tags existingTags;
    std::vector<long long> tagRowIds;
    while (queryTags.executeStep()) {
      tagRowIds.push_back(queryTags.getColumn(0));
      existingTags.push_back(queryTags.getColumn(1));
    }
    VLOG(1) << "found " << existingTags.size() << " existing tags";
    Tags newTags;
    if (entry.tags.size() > existingTags.size()) {
      std::sort(entry.tags.begin(), entry.tags.end());
      std::sort(existingTags.begin(), existingTags.end());
      std::set_difference(entry.tags.begin(), entry.tags.end(),
                          existingTags.begin(), existingTags.end(),
                          std::inserter(newTags, newTags.begin()));
    }
    VLOG(1) << "adding " << newTags.size() << " new tags";
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
    SQLite::Statement insertEntry(db, "INSERT INTO entries VALUES (NULL, ?, ?, ?, ?)");
    SQLite::bind(insertEntry, std::make_tuple(entry.title, entry.text, entry.created, entry.modified));
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
}
