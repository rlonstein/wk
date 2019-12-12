#include "SQLiteCpp/SQLiteCpp.h"
#include "SQLiteCpp/VariadicBind.h"
#import <string>

namespace WK {
  namespace SQL {
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
    "FOREIGN KEY(entry_id) REFERENCES entries(entry_id) ON DELETE CASCADE ON UPDATE CASCADE,"
    "FOREIGN KEY(tag_id) REFERENCES tags(entry_id) ON DELETE CASCADE ON UPDATE CASCADE);";

    const std::string queryEntriesMatchingTags =
    "SELECT entries.entry_id, entries.title, entries.content "
    "FROM entries, taglist WHERE entries.entry_id = taglist.entry_id AND taglist.tag_id = ?";

    const std::string templateQueryTags = "SELECT tag_id FROM tags WHERE tag in (%s)";
    
    const std::string queryEntriesTitleLike =
      "SELECT entry_id, title, content FROM entries WHERE title LIKE ?";

    const std::string queryEntryIdsByTitle =
      "SELECT entry_id FROM entries WHERE title = ?";

    const std::string pragmaEnableFK = "PRAGMA foreign_keys = ON;";

    const std::string deleteEntryById =
      "DELETE FROM entries WHERE entry_id = ?";
  }
}
