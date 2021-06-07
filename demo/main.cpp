#include <Dbcs.hpp>

int main(int argc, char *argv[]) {
  Dbcs new_dbcs;

  std::string db_path = "../test_rocksdb.db";
  Options options;
  options.create_if_missing = true;
  DB* db;

  std::string log_level;
  int thread_count;
  std::string output;

  options_description desc("General options");
  std::string task_type;

  desc.add_options()
      ("help", "")
      ("log-level", boost::program_options::value<std::string>()->
          default_value("error"))
      ("thread-count", boost::program_options::value<int>()->
          default_value(std::thread::hardware_concurrency()))
      ("output", boost::program_options::value<std::string>()
          ->default_value("path/to/input/dbcs-storage.db"));

  variables_map vm;
  try
  {
    parsed_options parsed = command_line_parser(argc, argv).options(desc).
        allow_unregistered().run();
    store(parsed, vm);
    if (vm.count("help")) {
      new_dbcs.out_help();
      return 1;
    }

    log_level = vm["log-level"].as<std::string>();
    thread_count = vm["thread-count"].as<int>();
    output = vm["output"].as<std::string>();

    Status s = DB::Open(options, db_path, &db);
    new_dbcs.create_start_db(db, s);
    new_dbcs.init_log(log_level);
    new_dbcs.producer(thread_count, db, output, log_level);
  }
  catch (std::exception& ex)
  {
    std::cout << desc << std::endl;
  }
}