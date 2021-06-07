// Copyright 2021 milerol <milerol181@yandex.ru>

#include <Dbcs.hpp>

Dbcs::Dbcs() {}

void Dbcs::out_help()
{
  std::string out_info;
  out_info = R"(Options:
  --log-level <string>              = "info"|"warning"|"error"
                                    = default: "error"
  --thread-count <number>           =
                                    = default: count of logical core
  --output <path>                   = <path/to/output/storage.db>
                                    = default: <path/to/input/dbcs-storage.db>)";
  std::cout << out_info << std::endl;
}

void Dbcs::init_log(std::string log_str)
{
  const std::string format = "[%TimeStamp%]: %Message%";

  auto sink_console = boost::log::add_console_log(std::cout,
      boost::log::keywords::format = format);
  if (log_str == "info")
  {
    sink_console->set_filter(
        boost::log::trivial::severity >= boost::log::trivial::info);
  } else if (log_str == "error") {
    sink_console->set_filter(
        boost::log::trivial::severity >= boost::log::trivial::error);
  } else if (log_str == "warning") {
    sink_console->set_filter(
        boost::log::trivial::severity >= boost::log::trivial::warning);
  } else {
    std::runtime_error("Invalid trivial");
  }
  boost::log::add_common_attributes();

  srand(time(nullptr));
}

void Dbcs::producer(int depth, rocksdb::DB *db, std::string output,
                    std::string log_str)
{
  std::string db_path = output;
  Options options;
  options.create_if_missing = true;
  DB* db_hash;
  Status s = DB::Open(options, db_path, &db_hash);
  ThreadPool tools(depth);
  rocksdb::Iterator* it = db->NewIterator(rocksdb::ReadOptions());
  for (it->SeekToFirst(); it->Valid(); it->Next()) {
    tools.enqueue(&Dbcs::consumer, std::ref(*this),
                  it->key().ToString(), it->value().ToString(), db_hash);
  }
  sleep(3);
  std::cout << std::endl;
  this->out_info_rocksdb(db_hash, log_str);
}

void Dbcs::consumer(std::string key, std::string data_to_hash, rocksdb::DB* db)
{
  std::string hash = picosha2::hash256_hex_string(data_to_hash);
  rocksdb::WriteOptions write_options;
  write_options.sync = false;
  db->Put(write_options, key, hash);
}

void Dbcs::create_start_db(rocksdb::DB* db, rocksdb::Status &st)
{
  rocksdb::WriteOptions options;
  options.sync = false;

  for (int i = 0; i < 30; ++i)
  {
    db->Put(options, std::to_string(std::rand()),
            std::to_string(std::rand()));
  }
}

void Dbcs::out_info_rocksdb(rocksdb::DB* db, std::string log_str)
{
  rocksdb::Iterator* it = db->NewIterator(rocksdb::ReadOptions());

  if (log_str == "info")
  {
    for (it->SeekToFirst(); it->Valid(); it->Next())
    {
      BOOST_LOG_TRIVIAL(info) << std::endl
                              << "key: [" << it->key().ToString()
                              << "] data: [" << it->value().ToString() << "]"
                              << std::endl;
    }
  } else if (log_str == "error") {
    for (it->SeekToFirst(); it->Valid(); it->Next())
    {
      BOOST_LOG_TRIVIAL(error) << std::endl
                              << "key: [" << it->key().ToString()
                              << "] data: [" << it->value().ToString() << "]"
                              << std::endl;
    }
  } else if (log_str == "warning") {
    for (it->SeekToFirst(); it->Valid(); it->Next())
    {
      BOOST_LOG_TRIVIAL(warning) << std::endl
                              << "key: [" << it->key().ToString()
                              << "] data: [" << it->value().ToString() << "]"
                              << std::endl;
    }
  }
}

Dbcs::~Dbcs() {}
