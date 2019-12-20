#include "wk.hpp"

INITIALIZE_EASYLOGGINGPP

int main(int argc, char** argv) {
  START_EASYLOGGINGPP(argc, argv);
  VLOG(1) << "started";
  
  CLI::App app("wk - a personal command line wiki");
  app.allow_extras();
  app.require_subcommand(1,1);

  std::string filename;
  std::string title;
  std::string text;
  std::vector<wk::TagName> tagnames;
  std::string format = "json";

  // wk new <filename>
  auto sub_new = app.add_subcommand("new", "create a new wiki")->alias("n");
  sub_new->add_option("filename", filename);
  sub_new->final_callback([&filename](void){ wk::cmds::newWiki(filename); });

  // wk add --title <title> --tags [<tag>, ...] --text <text>
  auto sub_add = app.add_subcommand("add", "add a wiki entry")->alias("a");
  sub_add->add_option("--title", title);
  sub_add->add_option("--tags", tagnames)->expected(-1);
  sub_add->add_option("--text", text);
  sub_add->final_callback([&title, &tagnames, &text](void){
                            wk::Tags tags;
                            for (wk::TagName tagname : tagnames) {
                              tags.push_back({tagname, wk::sql::INVALID_ROWID});
                            }
                            wk::cmds::addEntry({false, title, "", "", text, tags, {}});
                          });

  // wk search [<tag>, ...]
  auto sub_search = app.add_subcommand("search", "search the wiki")->alias("s");
  sub_search->add_option("keywords", tagnames)->expected(-1);
  sub_search->final_callback([&tagnames](void){wk::cmds::searchWiki(tagnames);});
  
  // wk edit <title>
  auto sub_edit = app.add_subcommand("edit", "edit a wiki entry")->alias("e");
  sub_edit->add_option("title", title)->required();
  sub_edit->final_callback([&title](void){ wk::cmds::editEntry(title); });

  // wk delete <title>
  auto sub_delete = app.add_subcommand("delete", "delete a wiki entry")->alias("d");
  sub_delete->add_option("title", title)->required();
  sub_delete->final_callback([&title](void){wk::cmds::deleteEntry(title);});

  // wk import --format [json|yaml|markdown] <filename>
  auto sub_import = app.add_subcommand("import", "import one or more entries from a file")->alias("i");
  sub_import->add_option("filename", filename)->check(CLI::ExistingFile)->required();
  sub_import->add_option("--format", format, "input format", true)->check(CLI::IsMember(wk::ImportFormatNames));
  sub_import->final_callback([&filename, &format](void){wk::cmds::importWiki(filename, format);});

  // wk export --format [json|markdown] <filename> [--tags [<tag>, ...]] [--title <title>]
  auto sub_export = app.add_subcommand("export", "export one or more wiki entries")->alias("x");
  sub_export->add_option("filename", filename)->required();
  sub_export->add_option("--format", format, "output format", true)->check(CLI::IsMember(wk::ExportFormatNames));
  auto sub_export_opt_tag = sub_export->add_option("--tags", tagnames)->expected(-1);
  auto sub_export_opt_title = sub_export->add_option("--title", title)->excludes(sub_export_opt_tag);
  sub_export_opt_tag->excludes(sub_export_opt_title);
  sub_export->final_callback([&filename, &format, &title, &tagnames](void){wk::cmds::exportWiki(filename, format, title, tagnames);});

  CLI11_PARSE(app, argc, argv);

  return 0;
}
