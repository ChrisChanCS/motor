// Author: Ming Zhang
// Copyright (c) 2023

#pragma once

#include <string>

#include "base/common.h"
#include "config/table_type.h"

static constexpr auto YCSB_FIELD_SIZE = 100;

union ycsb_key_t
{
  uint64_t ycsb_id;
  uint64_t item_key;

  ycsb_key_t()
  {
    item_key = 0;
  }
};

static_assert(sizeof(ycsb_key_t) == sizeof(uint64_t), "");

enum ycsb_val_bitmap : int
{
  yf1 = 0,
  yf2,
  yf3,
  yf4,
  yf5,
  yf6,
  yf7,
  yf8,
  yf9,
  yf10,
};

struct ycsb_val_t
{
  char Y_F01[YCSB_FIELD_SIZE + 1];
  char Y_F02[YCSB_FIELD_SIZE + 1];
  char Y_F03[YCSB_FIELD_SIZE + 1];
  char Y_F04[YCSB_FIELD_SIZE + 1];
  char Y_F05[YCSB_FIELD_SIZE + 1];
  char Y_F06[YCSB_FIELD_SIZE + 1];
  char Y_F07[YCSB_FIELD_SIZE + 1];
  char Y_F08[YCSB_FIELD_SIZE + 1];
  char Y_F09[YCSB_FIELD_SIZE + 1];
  char Y_F10[YCSB_FIELD_SIZE + 1];
} __attribute__((packed));

constexpr size_t ycsb_val_t_size = sizeof(ycsb_val_t);

// static_assert(sizeof(micro_val_t) == 40, "");

// Magic numbers for debugging. These are unused in the spec.

// ***************
// TODO: how to use magic
// ***************
#define Ycsb_MAGIC 10 /* Some magic number <= 255 */
#define ycsb_magic (Ycsb_MAGIC)

// Helpers for generating workload
#define YCSB_TX_TYPES 1

enum YCSBTxType : int
{
  RMW
};

// #define MICRO_TX_TYPES 1
// enum MicroTxType : int {
//   kRWOne,
// };

// enum MicroTxType : int {
//   kTxTest1 = 1,
//   kTxTest2,
//   kTxTest3,
//   kTxTest4,
//   kTxTest5,
//   kTxTest6,
//   kTxTest7,
//   kTxTest8,
//   kTxTest9,
//   kTxTest10,
//   kTxTest11,
//   kTxTest12,
//   kTxTest100,
//   kTxTest101,
//   kRWOne,
// };

// const std::string MICRO_TX_NAME[MICRO_TX_TYPES] = {"RWOne"};

const std::string YCSB_TX_NAME[YCSB_TX_TYPES] = {"RMW"};

// Table id
enum YcsbTableType : uint64_t
{
  kYcsbTable = TABLE_YCSB,
};
const int YCSB_TOTAL_TABLES = 1;
