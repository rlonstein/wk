#include "wk.hpp"

INITIALIZE_EASYLOGGINGPP

int main(int argc, char** argv) {
  START_EASYLOGGINGPP(argc, argv);
  VLOG(1) << "started";
  
  CLI::App app("wk - a personal command line wiki");
  app.allow_extras();
  app.require_subcommand(1,1);

  std::set<std::string> formats {"json", "markdown"};  
  std::string filename;
  std::string title;
  std::string text;
  std::vector<std::string> tags;
  std::string format = "json";

  // wk new <filename>
  auto sub_new = app.add_subcommand("new", "create a new wiki")->alias("n");
  sub_new->add_option("filename", filename);
  sub_new->final_callback([&filename](void){ WK::CMDS::newWiki(filename); });

  // wk add --title <title> --tags [<tag>, ...] --text <text>
  auto sub_add = app.add_subcommand("add", "add a wiki entry")->alias("a");
  sub_add->add_option("--title", title);
  sub_add->add_option("--tags", tags)->expected(-1);
  sub_add->add_option("--text", text);
  sub_add->final_callback([&title, &tags, &text](void){ WK::CMDS::addEntry(title, tags, text); });
  
  // wk search [<tag>, ...]
  auto sub_search = app.add_subcommand("search", "search the wiki")->alias("s");
  sub_search->add_option("keywords", tags)->expected(-1);
  sub_search->final_callback([&tags](void){WK::CMDS::searchWiki(tags);});
  
  // wk edit <title>
  auto sub_edit = app.add_subcommand("edit", "edit a wiki entry")->alias("e");
  sub_edit->add_option("title", title)->required();
  sub_edit->final_callback(std::bind(&WK::CMDS::editEntry, title));

  // wk delete <title>
  auto sub_delete = app.add_subcommand("delete", "delete a wiki entry")->alias("d");
  sub_delete->add_option("title", title)->required();
  sub_delete->final_callback([&title](void){WK::CMDS::deleteEntry(title);});

  // wk import --format [json|yaml|markdown] <filename>
  auto sub_import = app.add_subcommand("import", "import one or more entries from a file")->alias("i");
  sub_import->add_option("filename", filename)->check(CLI::ExistingFile)->required();
  sub_import->add_option("--format", format, "input format", true)->check(CLI::IsMember(WK::ImportFormatNames));
  sub_import->final_callback([&filename, &format](void){WK::CMDS::importWiki(filename, format);});

  // wk export --format [json|markdown] <filename> [--tags [<tag>, ...]] [--title <title>]
  auto sub_export = app.add_subcommand("export", "export one or more wiki entries")->alias("x");
  sub_export->add_option("filename", filename)->required();
  sub_export->add_option("--format", format, "output format", true)->check(CLI::IsMember(WK::ExportFormatNames));
  auto sub_export_opt_tag = sub_export->add_option("--tags", tags)->expected(-1);
  auto sub_export_opt_title = sub_export->add_option("--title", title)->excludes(sub_export_opt_tag);
  sub_export_opt_tag->excludes(sub_export_opt_title);
  sub_export->final_callback([&filename, &format, &title, &tags](void){WK::CMDS::exportWiki(filename, format, title, tags);});

  CLI11_PARSE(app, argc, argv);

  return 0;
}
