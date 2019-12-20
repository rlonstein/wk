#include "wk.hpp"
#include "sql.hpp"
#include "nlohmann/json.hpp"


void exportJSON(std::string filename, std::string title, wk::TagNames tags) {
  auto dbfqn = wk::sql::findDB();
  if (dbfqn.empty()) {
    LOG(ERROR) << "No wiki database found!";
    throw CLI::RuntimeError(-1);
  }
  std::string sql;
  if (title.empty() && tags.empty()) {
    sql = wk::sql::queryExportAll;
  } else {
    // FIXME
  }

  using json = nlohmann::json;
  json js;

  try {
    SQLite::Database db(dbfqn, SQLite::OPEN_READONLY);
    SQLite::Statement queryEntriesAndTags(db, sql);
    while (queryEntriesAndTags.executeStep()) {
      json jsentry;
      jsentry.emplace("title", queryEntriesAndTags.getColumn("Title"));
      std::string tagstr = queryEntriesAndTags.getColumn("Tags");
      std::istringstream is(tagstr);
      wk::TagNames tags{std::istream_iterator<std::string>{is},
                        std::istream_iterator<std::string>{}};
      jsentry.emplace("tags", tags);
      jsentry.emplace("text", queryEntriesAndTags.getColumn("Content"));
      jsentry.emplace("created", queryEntriesAndTags.getColumn("Created"));
      jsentry.emplace("modified", queryEntriesAndTags.getColumn("Modified"));
      js.push_back(jsentry);
    }
  }
  catch (SQLite::Exception e) {
    LOG(ERROR) << "sqlite error: " << e.what();
    throw CLI::RuntimeError(-1);
  }
  std::ofstream fsout(filename);
  fsout << js;
}

// FIXME: exportYAML

// FIXME: exportMarkdown

void wk::cmds::exportWiki(std::string filename, std::string format, std::string title, wk::TagNames tags) {
  if (VLOG_IS_ON(1)) {
    std::string tagstr = wk::utils::commafyStrVec(tags, std::string());
    VLOG(1) << "invoked export('" << filename << "', '" << format << "', '" << title << "', [" << tagstr << "])";
  }

  wk::ExportFileFormat ff = wk::ExportFormatNameMap.at(format);
  switch (ff) {
    case wk::ExportFileFormat::json:
      exportJSON(filename, title, tags);
      break;
    case wk::ExportFileFormat::yaml:
      LOG(ERROR) << "Not implemented yet";
      break;
    case wk::ExportFileFormat::markdown:
      LOG(ERROR) << "Not implemented yet";
      break;
    default:
      // shouldn't happen
      LOG(ERROR) << "Unknown import format '" << format << "'";
      throw CLI::RuntimeError(-1);
  }
}

