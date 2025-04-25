# TPCC
This script can help you run TPCC in different settings, including different cross-partition probabilities and thread numbers. Some static parameters are: 

```
"num_warehouse": 24,
"warehouse_bkt_num": 24,
"num_district_per_warehouse": 10,
"district_bkt_num": 10,
"num_customer_per_district": 3000,
"customer_bkt_num": 3500,
"num_stock_per_warehouse": 100000,
"stock_bkt_num": 200000,
"num_item": 100000,
"item_bkt_num": 100000
```

# YCSB
This script only runs YCSB with 40 threads. Some static parameters are:

```
"partition_num": 8,
"keysPerPartition": 307200
```
