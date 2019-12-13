#include "wk.hpp"
#include "sql.hpp"
#include "nlohmann/json.hpp"


void exportJSON(std::string filename, std::string title, std::vector<std::string> tags) {
  auto dbfqn = WK::UTILS::findDB();
  if (dbfqn.empty()) {
    LOG(ERROR) << "No wiki database found!";
    throw CLI::RuntimeError(-1);
  }
  std::string sql;
  if (title.empty() && tags.empty()) {
    // export everything
    sql = "SELECT entries.entry_id AS EntryId, entries.title AS Title, entries.content AS Content, "
      "entries.created AS Created, entries.modified AS Modified,"
      "GROUP_CONCAT(tags.tag, ' ') AS Tags FROM entries, tags "
      "INNER JOIN taglist on tags.tag_id = taglist.tag_id "
      "AND taglist.entry_id = EntryId GROUP BY Title";
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
      std::vector<std::string> tags{std::istream_iterator<std::string>{is},
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

void WK::CMDS::exportWiki(std::string filename, std::string format, std::string title, std::vector<std::string> tags) {
  if (VLOG_IS_ON(1)) {
    std::string tagstr = std::accumulate(
      std::begin(tags), std::end(tags), std::string(),
      [](std::string &ss, std::string &s) { return ss.empty() ? "'"+s+"'" : ss+", '"+s+"'"; } );
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

