// Author: Ming Zhang
// Copyright (c) 2023

#pragma once

#include <memory>

#include "ycsb/ycsb_db.h"
#include "process/txn.h"
#include "util/zipf.h"

/******************** The business logic (Transaction) start ********************/
bool TxRMW(coro_yield_t &yield,
           tx_id_t tx_id,
           TXN *txn,
           itemkey_t *key,
           bool *update);
/******************** The business logic (Transaction) end ********************/