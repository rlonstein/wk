#include "wk.hpp"
#include "sql.hpp"

void WK::CMDS::deleteEntry(std::string title) {
  auto dbfqn = WK::UTILS::findDB();
  if (dbfqn.empty()) {
    LOG(ERROR) << "No wiki database found!";
    throw CLI::RuntimeError(-1);
  }
  try {
    SQLite::Database db(dbfqn, SQLite::OPEN_READWRITE);
    std::vector<long long> entryIds;

    SQLite::Statement queryEntryIds(db, WK::SQL::queryEntryIdsByTitle);
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
    db.exec(WK::SQL::pragmaEnableFK);
    SQLite::Statement deleteEntryById(db, WK::SQL::deleteEntryById);
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
    LOG(ERROR) << "SQLite error: " << e.what();
    throw CLI::RuntimeError(-1);
  }
  throw CLI::Success();
}
