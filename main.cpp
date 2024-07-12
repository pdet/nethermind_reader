#include <iostream>
#include <rocksdb/db.h>
#include <rocksdb/options.h>
#include <rocksdb/slice.h>
#include <rocksdb/status.h>
#include <rocksdb/iterator.h>
#include "include/rlp.h"

int main() {
    rocksdb::DB* db;
    rocksdb::Options options;
    options.create_if_missing = false;

    std::string db_path = "/Users/holanda/Documents/Projects/ethereum/data/nethermind_db/mainnet/blocks";

    rocksdb::Status status = rocksdb::DB::Open(options, db_path, &db);
    if (!status.ok()) {
        std::cerr << "Unable to open RocksDB: " << status.ToString() << std::endl;
        return 1;
    }

    rocksdb::Iterator* it = db->NewIterator(rocksdb::ReadOptions());
    for (it->SeekToFirst(); it->Valid(); it->Next()) {
        std::string key = it->key().ToString();
        std::string value = it->value().ToString();
        dev::RLP rlp_key(key, dev::RLP::LaissezFaire);
        dev::RLP rlp_value(value, dev::RLP::LaissezFaire);
        auto values = rlp_value.convert<std::vector<std::vector<std::string>>>(dev::RLP::LaissezFaire);
      // dev::RLP rlp_value_01(values[0], dev::RLP::LaissezFaire);
  rlp_value.isData()
      // auto value_01 = rlp_value_01.convert<std::vector<std::vector<std::string>>>(dev::RLP::LaissezFaire);
      std::cout << "---" << std::endl;
      for (auto&v : values) {
        for (auto&v_2: v) {
           dev::RLP rlp_v_2(v_2, dev::RLP::LaissezFaire);
          if (rlp_v_2.isInt()) {
            std::cout << rlp_v_2.convert<dev::u256>(dev::RLP::LaissezFaire) << std::endl;
          }else if(rlp_v_2.isList()) {
            std::cout << "list" << std::endl;
          }

        }
      }
        // std::cout << "Key: " << rlp_key.convert<dev::u256>(dev::RLP::LaissezFaire) << ", Value: " << rlp_value.itemCount()  << std::endl;
    }

    // Clean up
    delete it;
    delete db;

    return 0;
}
