#include "wk.hpp"
#include "defs.hpp"
#include "SQLiteCpp/SQLiteCpp.h"
#include "SQLiteCpp/VariadicBind.h"
#include <string>

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
      "SELECT tags.tag_id as TagId, tags.tag as Tag FROM tags, taglist WHERE tags.tag = taglist.tag AND taglist.entry_id = ?";

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
    template <> wk::Entry getEntry(RowId rowId);
    template <> wk::Entry getEntry(std::string title);
    template <> wk::Entry getEntry(SQLite::Database* dbptr, RowId rowId);
    template <> wk::Entry getEntry(SQLite::Database* dbptr, std::string title);
  }
}
