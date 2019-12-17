#include "wk.hpp"
#include "defs.hpp"
#include "SQLiteCpp/SQLiteCpp.h"
#include "SQLiteCpp/VariadicBind.h"

namespace wk {
  namespace sql {
    template <> inline wk::Entry getEntry(SQLite::Database* dbptr, wk::sql::RowId entryId) {
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

    template <> inline wk::Entry getEntry(SQLite::Database* dbptr, std::string title) {
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
    
    template <> wk::Entry getEntry(wk::sql::RowId entryId) {
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
