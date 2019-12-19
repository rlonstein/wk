#include "wk.hpp"
#include "defs.hpp"
#include "SQLiteCpp/SQLiteCpp.h"
#include "SQLiteCpp/VariadicBind.h"
#include <string>
#ifndef USE_ALT_STDFS
 #include <filesystem>
#else
 #include "ghc/filesystem.hpp"
#endif

namespace wk {
  namespace sql {
    const std::string newSchemaTagsEntries =
      "CREATE TABLE tags(tag_id INTEGER UNIQUE NOT NULL PRIMARY KEY, tag TEXT UNIQUE NOT NULL);"
      "CREATE UNIQUE INDEX tagidx1 on tags(tag);"
      "CREATE TABLE entries(entry_id INTEGER UNIQUE NOT NULL PRIMARY KEY,"
      "title TEXT NOT NULL, content TEXT NOT NULL,"
      "created TEXT NOT NULL, modified TEXT NOT NULL);"
      "CREATE UNIQUE INDEX entryidx1 on entries(content);";

    const std::string newSchemaTaglist =
    "PRAGMA foreign_keys = ON;"
    "CREATE TABLE taglist(taglist_id INTEGER UNIQUE NOT NULL PRIMARY KEY,"
    "entry_id INTEGER NOT NULL, tag_id INTEGER NOT NULL,"
    "FOREIGN KEY(entry_id) REFERENCES entries(entry_id) ON DELETE CASCADE,"
    "FOREIGN KEY(tag_id) REFERENCES tags(entry_id) ON DELETE CASCADE);";

    const std::string queryEntriesMatchingTags =
    "SELECT entries.entry_id, entries.title, entries.content "
    "FROM entries, taglist WHERE entries.entry_id = taglist.entry_id AND taglist.tag_id = ?";

    const std::string templateQueryTags = "SELECT tag_id FROM tags WHERE tag in (%s)";
    
    const std::string queryEntriesTitleLike =
      "SELECT entry_id, title, content FROM entries WHERE title LIKE ?";

    const std::string queryEntryIdsByTitle =
      "SELECT entry_id FROM entries WHERE title = ?";

    const std::string queryEntryIdsFirstTitle = 
      "SELECT entry_id FROM entries WHERE title = ? LIMIT 1";

    const std::string queryEntryById =
      "SELECT title, created, modified, content FROM entries WHERE entry_id = ?";

    const std::string queryTagsByEntryId =
      "SELECT tags.tag_id as TagId, tags.tag as Tag FROM tags, taglist WHERE tags.tag_id = taglist.tag_id AND taglist.entry_id = ?";

    const std::string pragmaEnableFK = "PRAGMA foreign_keys = ON;";

    const std::string deleteEntryById =
      "DELETE FROM entries WHERE entry_id = ?";

    const std::string queryExportAll =
      "SELECT entries.entry_id AS EntryId, entries.title AS Title, entries.content AS Content, "
      "entries.created AS Created, entries.modified AS Modified,"
      "GROUP_CONCAT(tags.tag, ' ') AS Tags FROM entries, tags "
      "INNER JOIN taglist on tags.tag_id = taglist.tag_id "
      "AND taglist.entry_id = EntryId GROUP BY Title";

    std::string findDB();
    std::vector<std::pair<wk::Tag, RowId>> queryTagInfo(RowId entryId);
    
    template <typename T> wk::Entry getEntry(T rowId);
    template <typename T, typename U> wk::Entry getEntry(T db, U rowId);
    template <> inline wk::Entry getEntry(SQLite::Database* dbptr, wk::sql::RowId entryId) {
      wk::Entry entry;
      VLOG(1) << "getEntry() searching for entryId #" << entryId;
      try {
        SQLite::Statement queryEntry(*dbptr, wk::sql::queryEntryById);
        queryEntry.bind(1, entryId);
        if (! queryEntry.executeStep()) {
          LOG(WARNING) << "EntryId #" << entryId << " not found";
          entry.populated = false;
          return entry;
        }
        entry.populated = true;
        entry.title = std::string(queryEntry.getColumn("title"));
        entry.created = std::string(queryEntry.getColumn("created"));
        entry.modified = std::string(queryEntry.getColumn("modified"));
        entry.text = std::string(queryEntry.getColumn("content"));
        VLOG(1) << "Found entry, querying tags...";
        SQLite::Statement queryTags(*dbptr, wk::sql::queryTagsByEntryId);
        queryTags.bind(1, entryId);
        while (queryTags.executeStep()) {
          entry.tags.push_back(queryTags.getColumn("Tag"));
          entry.tagIds.push_back(queryTags.getColumn("TagId"));
        }
        VLOG(1) << "Have " << entry.tags.size() << " tags for entry";
      }
      catch (SQLite::Exception e) {
        LOG(ERROR) << "SQLite Error: " << e.what();
        throw CLI::RuntimeError(-1);
      }
      return entry;
    }    

    template <> inline wk::Entry getEntry(SQLite::Database* dbptr, std::string title) {
      wk::sql::RowId entryId;
      SQLite::Statement queryTitle(*dbptr, wk::sql::queryEntryIdsFirstTitle);
      queryTitle.bind(1, title);
      if (! queryTitle.executeStep()) {
        LOG(ERROR) << "No title '" << title << "' found";
        throw CLI::RuntimeError(-1);
      }
      entryId = queryTitle.getColumn(0);
      return wk::sql::getEntry(dbptr, entryId);
    }
    
    template <> inline wk::Entry getEntry(wk::sql::RowId entryId) {
      auto dbfqn = wk::sql::findDB();
      if (dbfqn.empty()) {
        LOG(ERROR) << "No wiki database found!";
        throw CLI::RuntimeError(-1);
      }
      SQLite::Database db(dbfqn, SQLite::OPEN_READONLY);
      return wk::sql::getEntry(&db, entryId);
    }

    template <> inline wk::Entry getEntry(std::string title) {
      auto dbfqn = wk::sql::findDB();
      if (dbfqn.empty()) {
        LOG(ERROR) << "No wiki database found!";
        throw CLI::RuntimeError(-1);
      }
      SQLite::Database db(dbfqn, SQLite::OPEN_READONLY);
      return wk::sql::getEntry(&db, title);
    }
    
  }
}
