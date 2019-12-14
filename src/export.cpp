#include "wk.hpp"
#include "sql.hpp"
#include "nlohmann/json.hpp"


void exportJSON(std::string filename, std::string title, WK::Tags tags) {
  auto dbfqn = WK::UTILS::findDB();
  if (dbfqn.empty()) {
    LOG(ERROR) << "No wiki database found!";
    throw CLI::RuntimeError(-1);
  }
  std::string sql;
  if (title.empty() && tags.empty()) {
    sql = WK::SQL::queryExportAll;
  } else {
    // FIXME
  }

  using json = nlohmann::json;
  json js;

  try {
    SQLite::Database db(dbfqn, SQLite::OPEN_READONLY);
    SQLite::Statement queryEntriesAndTags(db, sql);
    while (queryEntriesAndTags.executeStep()) {
      json entry;
      entry.emplace("title", queryEntriesAndTags.getColumn("Title"));
      std::istringstream is(queryEntriesAndTags.getColumn("Tags"));
      WK::Tags tags{std::istream_iterator<std::string>{is},
                    std::istream_iterator<std::string>{}};
      entry.emplace("tags", tags);
      entry.emplace("text", queryEntriesAndTags.getColumn("Content"));
      entry.emplace("created", queryEntriesAndTags.getColumn("Created"));
      entry.emplace("modified", queryEntriesAndTags.getColumn("Modified"));
      js.push_back(entry);
    }
  }
  catch (SQLite::Exception e) {
    LOG(ERROR) << "SQLite error: " << e.what();
    throw CLI::RuntimeError(-1);
  }
  std::ofstream fsout(filename);
  fsout << js;
}

// FIXME: exportYAML

// FIXME: exportMarkdown

void WK::CMDS::exportWiki(std::string filename, std::string format, std::string title, WK::Tags tags) {
  if (VLOG_IS_ON(1)) {
    std::string tagstr = WK::UTILS::commafyStrVec(tags, std::string());
    VLOG(1) << "invoked export('" << filename << "', '" << format << "', '" << title << "', [" << tagstr << "])";
  }

  WK::ExportFileFormat ff = WK::ExportFormatNameMap.at(format);
  switch (ff) {
    case WK::ExportFileFormat::json:
      exportJSON(filename, title, tags);
      break;
    case WK::ExportFileFormat::yaml:
      LOG(ERROR) << "Not implemented yet";
      break;
    case WK::ExportFileFormat::markdown:
      LOG(ERROR) << "Not implemented yet";
      break;
    default:
      // shouldn't happen
      LOG(ERROR) << "Unknown import format '" << format << "'";
      throw CLI::RuntimeError(-1);
  }
}

