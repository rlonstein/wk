#include "wk.hpp"
#include "sql.hpp"

void wk::cmds::deleteEntry(std::string title) {
  auto dbfqn = wk::sql::findDB();
  if (dbfqn.empty()) {
    LOG(ERROR) << "No wiki database found!";
    throw CLI::RuntimeError(-1);
  }
  try {
    SQLite::Database db(dbfqn, SQLite::OPEN_READWRITE);
    std::vector<wk::sql::RowId> entryIds;

    SQLite::Statement queryEntryIds(db, wk::sql::queryEntryIdsByTitle);
    queryEntryIds.bind(1, title);
    VLOG(1) << "Querying for title matching '" << title << "'";
    while (queryEntryIds.executeStep()) {
      entryIds.push_back(queryEntryIds.getColumn(0));
    }
    if (entryIds.empty()) {
      LOG(INFO) << "No entries matching '" << title << "' found.";
      throw CLI::Success();
    }

    SQLite::Transaction transaction(db);
    VLOG(1) << "Enabling foreign keys...";
    db.exec(wk::sql::pragmaEnableFK);
    SQLite::Statement deleteEntryById(db, wk::sql::deleteEntryById);
    for (auto entryId : entryIds) {
      deleteEntryById.bind(1, entryId);
      VLOG(1) << "Deleting entryId #" << entryId;
      deleteEntryById.exec();
      deleteEntryById.clearBindings();
      deleteEntryById.reset();
    }
    transaction.commit();
    VLOG(1) << "Deleted " << entryIds.size() << " records";
  }
  catch (SQLite::Exception e) {
    LOG(ERROR) << "sqlite error: " << e.what();
    throw CLI::RuntimeError(-1);
  }
  throw CLI::Success();
}
