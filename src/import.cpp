#include "wk.hpp"
#include "utils.hpp"
#include "nlohmann/json.hpp"


void addJSONEntry(nlohmann::json js) {
  wk::Entry entry;

  if (! js.is_object()) {
    LOG(WARNING) << "Skipping element, not a json object";
    return;
  }
  if ( ! js.count("title") || js.find("title")->empty()) {
    LOG(WARNING) << "Missing/empty field 'title' in entry, skipped";
    return;
  }
  entry.title = js.find("title")->get<std::string>();
  if (entry.title.empty()) {
    LOG(WARNING) << "Missing/empty field 'title' in entry, skipped";
    return;
  }

  if (! js.count("tags")) {
    LOG(WARNING) << "Missing field 'tags' in entry, skipped";
    return;
  }
  if (! js.find("tags")->is_array()) {
    LOG(WARNING) << "Incorrect field 'tags' in entry- not an array, skipped";
  }
  wk::TagNames tagnames = js.find("tags")->get<wk::TagNames>();
  wk::Tags tags;
  for (wk::TagName tag : tagnames) {
    tags.push_back({tag, wk::sql::INVALID_ROWID});
  }
  entry.tags = (tags);

  if (! js.count("text") || js.find("text")->empty()) {
    LOG(WARNING) << "Missing/empty field 'text' in entry, skipped";
    return;
  }
  entry.text = js.find("text")->get<std::string>();
  if (entry.text.empty()) {
    LOG(WARNING) << "Missing/empty field 'text' in entry, skipped";
    return;
  }

  if (! js.count("created") || js.find("created")->empty()) {
    entry.created = wk::utils::getCurrentDatetime();
  } else {
    entry.created = js.find("created")->get<std::string>();
  }
    
  if (! js.count("modified") || js.find("modified")->empty()) {
    entry.modified = entry.created;
  } else {
    entry.modified = js.find("modified")->get<std::string>();
  }
  wk::cmds::addEntry(entry);
}


void importJSON(std::string filename) {
  using json = nlohmann::json;
  std::ifstream fh(filename);
  json js;
  fh >> js;

  if (js.is_array()) {
    VLOG(1) << "Iterating over array of entries";
    for (auto& [idx, elem] : js.items()) {
      VLOG(1) << "idx=="<< idx << ": " << elem << "\n";
      try {
        addJSONEntry(elem);
      }
      catch (CLI::RuntimeError e) {
        LOG(WARNING) << "Skipping idx# " << idx << " with malformed entry '" << elem << "'";
        continue;
      }
    }
  }
  else if (js.is_object()) {
    VLOG(1) << "Found single entry";
    addJSONEntry(js);
  } else {
    LOG(ERROR) << "Don't know how to process JSON data, aborted";
    throw CLI::RuntimeError(-1);
  }
}


void wk::cmds::importWiki(std::string filename, std::string format) {
  VLOG(1) << "invoked importWiki(" << filename << ", " << format << ")";
  if (wk::ImportFormatNameMap.find(format) == wk::ImportFormatNameMap.end()) {
    // shouldn't happen, validated by CLI11
    LOG(ERROR) << "Unknown import format '" << format << "'";
    throw CLI::RuntimeError(-1);
  }
  wk::ImportFileFormat ff = wk::ImportFormatNameMap.at(format);
  switch (ff) {
    case wk::ImportFileFormat::json:
      importJSON(filename);
      break;
    case wk::ImportFileFormat::yaml:
      LOG(ERROR) << "Not implemented yet";
      break;
    default:
      // shouldn't happen, validated by CLI11 and above
      LOG(ERROR) << "Unknown import format '" << format << "'";
      throw CLI::RuntimeError(-1);
  }
}
