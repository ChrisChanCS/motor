#!/bin/bash

# memory and compute node
MN_NODE=("129.114.109.131" "129.114.109.166" "129.114.109.83")
CN_NODE_0="129.114.108.225"

# define order_remote_ratio and payment_remote_ratio
ORDER_REMOTE_RATIO=(0 10 20 30 40 50 60)
PAYMENT_REMOTE_RATIO=(0 15 30 45 60 75 90)

CONFIG_PATH="/home/cc/motor/config/tpcc_config.json"

THREAD_VALUES=(8 12 16 20 24 28 32 36)
# THREAD_VALUES=(16)

# traverse THREAD_VALUES
for THREAD_VALUE in "${THREAD_VALUES[@]}"; do
    echo "=== 开始运行 THREAD_VALUE=${THREAD_VALUE} ==="

    # modify config
    for ((i=0; i<=6; i++)); do
        echo "=== the $i time, modify config ==="

        # modify tpcc_config.json
        ssh cc@$CN_NODE_0 "sed -i '3s/\"order_remote_ratio\": [0-9]*,/\"order_remote_ratio\": ${ORDER_REMOTE_RATIO[i]},/' $CONFIG_PATH"
        ssh cc@$CN_NODE_0 "sed -i '4s/\"payment_remote_ratio\": [0-9]*,/\"payment_remote_ratio\": ${PAYMENT_REMOTE_RATIO[i]},/' $CONFIG_PATH"
        echo "set to: order_remote_ratio=${ORDER_REMOTE_RATIO[i]}, payment_remote_ratio=${PAYMENT_REMOTE_RATIO[i]}"

        # start memory nodes
        for node in "${MN_NODE[@]}"; do
            ssh cc@${node} "cd ./motor/build/memory_node/server; ./motor_mempool" > /dev/null 2>&1 &
            echo "Memory node start: $node"
        done

        # wait memory nodes run
        sleep 120

        # start compute node，use THREAD_VALUE
        echo "start Compute Node: $CN_NODE_0, ./run tpcc ${THREAD_VALUE} 3 SR"
        ssh cc@$CN_NODE_0 "cd ./motor/build/compute_node/run; ./run tpcc ${THREAD_VALUE} 3 SR"

        # kill memory nodes
        for node in "${MN_NODE[@]}"; do
            ssh cc@${node} "kill -9 \$(lsof -t -i:12346)" > /dev/null 2>&1
            echo "clean Memory Node: $node"
        done

        sleep 5

        # kill memory nodes
        for node in "${MN_NODE[@]}"; do
            ssh cc@${node} "kill -9 \$(lsof -t -i:12346)" > /dev/null 2>&1
            echo "clean Memory Node: $node"
        done

        echo
    done

    echo "=== all THREAD_VALUE=${THREAD_VALUE} finish ==="
    echo
done
