// Author: Ming Zhang
// Copyright (c) 2023

#include "ycsb/ycsb_db.h"

#include "unistd.h"
#include "util/json_config.h"
#include "Random.h"

/* Called by main. Only initialize here. The worker threads will populate. */
void YCSB::LoadTable(node_id_t node_id,
                     node_id_t num_server,
                     MemStoreAllocParam *mem_store_alloc_param,
                     size_t &total_size,
                     size_t &ht_loadfv_size,
                     size_t &ht_size,
                     size_t &initfv_size,
                     size_t &real_cvt_size)
{
  // Initiate + Populate table for primary role
  {
    RDMA_LOG(DBG) << "Loading YCSB table";
    std::string config_filepath = "../../../config/ycsb_config.json";
    auto json_config = JsonConfig::load_file(config_filepath);
    auto table_config = json_config.get("ycsb");
    ycsb_table = new HashStore((table_id_t)YcsbTableType::kYcsbTable,
                               table_config.get("partition_num").get_uint64() *
                                   table_config.get("keysPerPartition").get_uint64(),
                               mem_store_alloc_param);
    PopulateYcsbTable();
    total_size += ycsb_table->GetTotalSize();
    ht_loadfv_size += ycsb_table->GetHTInitFVSize();
    ht_size += ycsb_table->GetHTSize();
    initfv_size += ycsb_table->GetInitFVSize();
    real_cvt_size += ycsb_table->GetLoadCVTSize();
  }

  std::cout << "----------------------------------------------------------" << std::endl;
  // Assign primary

  if ((node_id_t)YcsbTableType::kYcsbTable % num_server == node_id)
  {
    RDMA_LOG(EMPH) << "[Primary] YCSB table ID: " << (node_id_t)YcsbTableType::kYcsbTable;
    std::cerr << "Number of initial records: " << std::dec << ycsb_table->GetInitInsertNum() << std::endl;
    primary_table_ptrs.push_back(ycsb_table);
  }

  std::cout << "----------------------------------------------------------" << std::endl;
  // Assign backup

  if (BACKUP_NUM < num_server)
  {
    for (node_id_t i = 1; i <= BACKUP_NUM; i++)
    {
      if ((node_id_t)YcsbTableType::kYcsbTable % num_server == (node_id - i + num_server) % num_server)
      {
        RDMA_LOG(EMPH) << "[Backup] YCSB table ID: " << (node_id_t)YcsbTableType::kYcsbTable;
        std::cerr << "Number of initial records: " << std::dec << ycsb_table->GetInitInsertNum() << std::endl;
        backup_table_ptrs.push_back(ycsb_table);
      }
    }
  }
}

void YCSB::PopulateYcsbTable()
{
  RDMA_LOG(DBG) << "NUM KEYS TOTAL: " << num_keys_global;
  star::Random random;

  for (uint64_t id = 0; id < num_keys_global; id++)
  {
    ycsb_key_t ycsb_key;
    ycsb_key.ycsb_id = (uint64_t)id;

    ycsb_val_t ycsb_val;
    strcpy(ycsb_val.Y_F01, random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE).c_str());
    strcpy(ycsb_val.Y_F02, random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE).c_str());
    strcpy(ycsb_val.Y_F03, random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE).c_str());
    strcpy(ycsb_val.Y_F04, random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE).c_str());
    strcpy(ycsb_val.Y_F05, random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE).c_str());
    strcpy(ycsb_val.Y_F06, random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE).c_str());
    strcpy(ycsb_val.Y_F07, random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE).c_str());
    strcpy(ycsb_val.Y_F08, random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE).c_str());
    strcpy(ycsb_val.Y_F09, random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE).c_str());
    strcpy(ycsb_val.Y_F10, random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE).c_str());

    LoadRecord(ycsb_table, ycsb_key.item_key,
               (void *)&ycsb_val, sizeof(ycsb_val_t),
               (table_id_t)YcsbTableType::kYcsbTable);
  }
}

void YCSB::LoadRecord(HashStore *table,
                      itemkey_t item_key,
                      void *val_ptr,
                      size_t val_size,
                      table_id_t table_id)
{
  assert(val_size <= MAX_VALUE_SIZE);
  table->LocalInsertTuple(item_key, (char *)val_ptr, val_size);
}
