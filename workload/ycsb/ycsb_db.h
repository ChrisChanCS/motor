// Author: Ming Zhang
// Copyright (c) 2023

#pragma once

#include <cassert>
#include <vector>

#include "memstore/hash_store.h"
#include "ycsb/ycsb_table.h"
#include "util/fast_random.h"
#include "util/json_config.h"

class YCSB
{
public:
  std::string bench_name;

  uint64_t num_keys_global;

  /* Tables */
  HashStore *ycsb_table;

  std::vector<HashStore *> primary_table_ptrs;

  std::vector<HashStore *> backup_table_ptrs;

  // For server usage: Provide interfaces to servers for loading tables
  // Also for client usage: Provide interfaces to clients for generating ids during tests
  YCSB()
  {
    bench_name = "YCSB";
    std::string config_filepath = "../../../config/ycsb_config.json";
    auto json_config = JsonConfig::load_file(config_filepath);
    auto conf = json_config.get("ycsb");
    num_keys_global = conf.get("partition_num").get_int64() * conf.get("keysPerPartition").get_int64();
    ycsb_table = nullptr;
  }

  ~YCSB()
  {
    if (ycsb_table)
      delete ycsb_table;
  }

  void LoadTable(node_id_t node_id,
                 node_id_t num_server,
                 MemStoreAllocParam *mem_store_alloc_param,
                 size_t &total_size,
                 size_t &ht_loadfv_size,
                 size_t &ht_size,
                 size_t &initfv_size,
                 size_t &real_cvt_size);

  void PopulateYcsbTable();

  void LoadRecord(HashStore *table,
                  itemkey_t item_key,
                  void *val_ptr,
                  size_t val_size,
                  table_id_t table_id);

  ALWAYS_INLINE
  std::vector<HashStore *> GetPrimaryHashStore()
  {
    return primary_table_ptrs;
  }

  ALWAYS_INLINE
  std::vector<HashStore *> GetBackupHashStore()
  {
    return backup_table_ptrs;
  }
};
