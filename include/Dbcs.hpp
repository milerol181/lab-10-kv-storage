// Copyright 2021 Danil Postvaykin <postvaykin01@mail.ru>

#ifndef INCLUDE_DBCS_HPP_
#define INCLUDE_DBCS_HPP_

#include <picosha2.h>
#include <ThreadPool.hpp>

#include <boost/log/trivial.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/utility/setup.hpp>
#include <boost/program_options.hpp>

#include <string>
#include <iostream>
#include <rocksdb/db.h>
#include <time.h>

using namespace rocksdb;
using namespace boost::program_options;

class Dbcs
{
 public:
  Dbcs();
  void out_info_rocksdb(rocksdb::DB* db, std::string log_str);
  void create_start_db(rocksdb::DB* db, rocksdb::Status &st);
  void producer(int depth, rocksdb::DB *db, std::string output,
                std::string log_str);
  void consumer(std::string key, std::string data_to_hash, rocksdb::DB* db);
  void init_log(std::string log_str);
  void out_help();
  ~Dbcs();
};

#endif // INCLUDE_DBCS_HPP_
