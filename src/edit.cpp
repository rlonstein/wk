#include "wk.hpp"
#include "utils.hpp"
#include "sql.hpp"

void wk::cmds::editEntry(std::string title) {
  auto dbfqn = wk::sql::findDB();
  if (dbfqn.empty()) {
    LOG(ERROR) << "No wiki database found!";
    throw CLI::RuntimeError(-1);
  }

  try {
    SQLite::Database db(dbfqn, SQLite::OPEN_READWRITE);
    db.exec("PRAGMA foreign_keys = ON;");

    Entry oldEntry = wk::sql::getEntry(&db, title);
    VLOG(1) << "Found entry id " << oldEntry.entryId;
    Entry modEntry = wk::utils::editEntry(oldEntry);
    if (oldEntry == modEntry) {
      VLOG(1) << "No change to entry";
      std::cout << "No changes" << std::endl;
      return;
    }
    VLOG(1) << "Starting transaction";
    SQLite::Transaction transaction(db);
    std::string sql = "UPDATE entries SET title = ?, created = ?, modified = ?, content = ? WHERE entry_id = ?";
    SQLite::Statement update(db, sql);
    update.bind(1, modEntry.title);
    update.bind(2, modEntry.created);
    update.bind(3, modEntry.modified);
    update.bind(4, modEntry.text);
    update.bind(5, oldEntry.entryId);
    VLOG(1) << "Updating existing entry id " << oldEntry.entryId;
    update.exec();

    wk::TagNames oldTagNames, modTagNames, removedTagNames, addedTagNames;
    oldTagNames = wk::utils::getTagNamesFromTags(oldEntry.tags);
    modTagNames = wk::utils::getTagNamesFromTags(modEntry.tags);
    std::sort(oldTagNames.begin(), oldTagNames.end());
    std::sort(modTagNames.begin(), modTagNames.end());
    std::set_difference(oldTagNames.begin(), oldTagNames.end(),
                        modTagNames.begin(), modTagNames.end(),
                        std::inserter(removedTagNames, removedTagNames.begin()));
    std::set_difference(modTagNames.begin(), modTagNames.end(),
                        oldTagNames.begin(), oldTagNames.end(),
                        std::inserter(addedTagNames, addedTagNames.begin()));
    if (VLOG_IS_ON(1)) {
      VLOG(1) << "original tags == [" << wk::utils::commafyStrVec(oldTagNames) << "]";
      VLOG(1) << "current tags == [" << wk::utils::commafyStrVec(modTagNames) << "]";
      VLOG(1) << "removed == [" << wk::utils::commafyStrVec(removedTagNames) << "]";
      VLOG(1) << "added == [" << wk::utils::commafyStrVec(addedTagNames) << "]";
    }

    if (removedTagNames.empty()) {
      VLOG(1) << "No tags to remove";
    } else {

      sql = "DELETE FROM taglist WHERE entry_id = ? AND tag_id = (SELECT tag_id FROM tags WHERE tag = ?)";
      SQLite::Statement removeTag(db, sql);
      for (auto tagname : removedTagNames) {
        removeTag.bind(1, oldEntry.entryId);
        removeTag.bind(2, tagname);
        VLOG(1) << "Removing mapped tag '" << tagname << "' on entryid " << oldEntry.entryId;
        int rowcount = removeTag.exec();
        VLOG(1) << rowcount << " rows altered";
        removeTag.clearBindings();
      }      
    }

    if (addedTagNames.empty()) {
      VLOG(1) << "No new tags to add";
    } else {
      VLOG(1) << "Adding tags...";
      SQLite::Statement addNewTags(db, "INSERT OR IGNORE INTO tags (tag_id, tag) VALUES (NULL, ?)");
      for (auto tagname : addedTagNames) {
        addNewTags.bind(1, tagname);
        VLOG(1) << "Inserting with ignore for tag '" << tagname << "'";
        auto rowcount = addNewTags.exec();
        VLOG(1) << rowcount << " rows updated";
        addNewTags.clearBindings();
      }

      VLOG(1) << "Applying new tag mappings to entry...";
      sql = "INSERT INTO taglist (entry_id, tag_id) SELECT ? AS entry_id, tag_id FROM tags WHERE tag = ? LIMIT 1";
      SQLite::Statement updateTagMapping(db, sql);
      for (auto tagname : addedTagNames) {
        updateTagMapping.bind(1, oldEntry.entryId);
        updateTagMapping.bind(2, tagname);
        VLOG(1) << "Updating mapping for tag '" << tagname << "'";
        auto rowcount = updateTagMapping.exec();
        VLOG(1) << rowcount << " rows updated";
        updateTagMapping.clearBindings();
      }
      VLOG(1) << "Committing transaction";
      transaction.commit();
    }

  }
  catch (SQLite::Exception e) {
    LOG(ERROR) << "SQLite error: " << e.what();
    throw CLI::RuntimeError(-1);
  }

  throw CLI::Success();
}

