#include "wk.hpp"
#include "sql.hpp"
#include "sql_impl.cpp"

// extern template wk::Entry wk::sql::getEntry(SQLite::Database* dbptr, wk::sql::RowId entryId);
// extern template wk::Entry wk::sql::getEntry(SQLite::Database* dbptr, std::string title);
// extern template wk::Entry wk::sql::getEntry(wk::sql::RowId entryId);
// extern template wk::Entry wk::sql::getEntry(std::string title);

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
    Entry modEntry = wk::utils::editEntry(oldEntry);
    if (oldEntry == modEntry) {
      VLOG(1) << "No change to entry";
      std::cout << "No changes" << std::endl;
      return;
    }
    VLOG(1) << "Starting transaction";
    SQLite::Transaction transaction(db);    
    std::string sql = "UPDATE entries SET title = ?, created = ?, modified = ? content = ? WHERE entryid = ?";
    SQLite::Statement update(db, sql);
    update.bind(1, modEntry.title);
    update.bind(2, modEntry.created);
    update.bind(3, modEntry.modified);
    update.bind(4, modEntry.text);
    update.bind(5, oldEntry.entryId);
    VLOG(1) << "Updating existing entry";
    update.exec();
    // FIXME: apply changes to tags
    VLOG(1) << "Committing transaction";
    transaction.commit();
  }
  catch (SQLite::Exception e) {
    LOG(ERROR) << "SQLite error: " << e.what();
    throw CLI::RuntimeError(-1);
  }

  throw CLI::Success();
}
