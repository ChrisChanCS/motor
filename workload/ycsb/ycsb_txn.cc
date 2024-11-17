// Author: Ming Zhang
// Copyright (c) 2023

#include "ycsb/ycsb_txn.h"
#include "Random.h"
// #include "ycsb/ycsb_db.h"

#include <set>

bool TxRMW(coro_yield_t &yield,
           tx_id_t tx_id,
           TXN *txn,
           itemkey_t *key,
           bool *update)
{
    txn->Begin(tx_id, TXN_TYPE::kRWTxn);

    DataSetItemPtr ycsb_record[10];
    for (int i = 0; i < 10; i++)
    {
        ycsb_key_t ycsb_key;
        ycsb_key.item_key = key[i];
        if (update[i] == false)
        {
            ycsb_record[i] = std::make_shared<DataSetItem>((table_id_t)YcsbTableType::kYcsbTable,
                                                           ycsb_val_t_size,
                                                           ycsb_key.item_key,
                                                           UserOP::kRead);
            txn->AddToReadOnlySet(ycsb_record[i]);
        }
        else
        {
            ycsb_record[i] = std::make_shared<DataSetItem>((table_id_t)YcsbTableType::kYcsbTable,
                                                           ycsb_val_t_size,
                                                           ycsb_key.item_key,
                                                           UserOP::kUpdate);
            txn->AddToReadWriteSet(ycsb_record[i]);
        }
    }
    if (!txn->Execute(yield))
    {
        return false;
    }
    for (int i = 0; i < 10; i++)
    {
        ycsb_val_t *ycsb_val = (ycsb_val_t *)ycsb_record[i]->Value();
        /*
        // TODO: how to use magic?????
        if (micro_val->d1 != micro_magic + 1)
        {
            RDMA_LOG(FATAL) << "[FATAL] Read unmatch, tid-cid-txid: " << txn->t_id << "-" << txn->coro_id << "-" << tx_id;
        }
        */
        if (update[i] == true)
        {
            star::Random random;
            ycsb_record[i]->SetUpdate(ycsb_val_bitmap::yf1, &ycsb_val->Y_F01, sizeof(ycsb_val->Y_F01));
            ycsb_record[i]->SetUpdate(ycsb_val_bitmap::yf2, &ycsb_val->Y_F02, sizeof(ycsb_val->Y_F02));
            ycsb_record[i]->SetUpdate(ycsb_val_bitmap::yf3, &ycsb_val->Y_F03, sizeof(ycsb_val->Y_F03));
            ycsb_record[i]->SetUpdate(ycsb_val_bitmap::yf4, &ycsb_val->Y_F04, sizeof(ycsb_val->Y_F04));
            ycsb_record[i]->SetUpdate(ycsb_val_bitmap::yf5, &ycsb_val->Y_F05, sizeof(ycsb_val->Y_F05));
            ycsb_record[i]->SetUpdate(ycsb_val_bitmap::yf6, &ycsb_val->Y_F06, sizeof(ycsb_val->Y_F06));
            ycsb_record[i]->SetUpdate(ycsb_val_bitmap::yf7, &ycsb_val->Y_F07, sizeof(ycsb_val->Y_F07));
            ycsb_record[i]->SetUpdate(ycsb_val_bitmap::yf8, &ycsb_val->Y_F08, sizeof(ycsb_val->Y_F08));
            ycsb_record[i]->SetUpdate(ycsb_val_bitmap::yf9, &ycsb_val->Y_F09, sizeof(ycsb_val->Y_F09));
            ycsb_record[i]->SetUpdate(ycsb_val_bitmap::yf10, &ycsb_val->Y_F10, sizeof(ycsb_val->Y_F10));
            strcpy(ycsb_val->Y_F01, random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE).c_str());
            strcpy(ycsb_val->Y_F02, random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE).c_str());
            strcpy(ycsb_val->Y_F03, random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE).c_str());
            strcpy(ycsb_val->Y_F04, random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE).c_str());
            strcpy(ycsb_val->Y_F05, random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE).c_str());
            strcpy(ycsb_val->Y_F06, random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE).c_str());
            strcpy(ycsb_val->Y_F07, random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE).c_str());
            strcpy(ycsb_val->Y_F08, random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE).c_str());
            strcpy(ycsb_val->Y_F09, random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE).c_str());
            strcpy(ycsb_val->Y_F10, random.a_string(YCSB_FIELD_SIZE, YCSB_FIELD_SIZE).c_str());
        }
    }
    bool commit_status = txn->Commit(yield);
    return commit_status;
}