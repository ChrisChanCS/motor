#!/bin/bash

# memory and compute node
MN_NODE=("129.114.109.23" "129.114.108.84" "129.114.108.13")
CN_NODE_0="129.114.109.66"

# modify config
for node in "${MN_NODE[@]}"; do
    ssh cc@$node "
        sed -i '7s/.*/#define WORKLOAD_TPCC 0/' ~/motor/txn/flags.h
        sed -i '11s/.*/#define WORKLOAD_YCSB 1/' ~/motor/txn/flags.h
        sed -i '12s/.*/\"workload\": \"YCSB\"/' ~/motor/config/mn_config.json
        cd ~/motor/ && ./build.sh -s
    "
    echo "Memory node $node init"
done

ssh cc@$CN_NODE_0 "
    sed -i '7s/.*/#define WORKLOAD_TPCC 0/' ~/motor/txn/flags.h
    sed -i '11s/.*/#define WORKLOAD_YCSB 1/' ~/motor/txn/flags.h
    cd ~/motor/ && ./build.sh
"
echo "Compute node $CN_NODE_0 init"

# skewness and r/w ratio
for outer_params in "0.7 50" "0.7 95" "0.7 50" "0.7 100"; do
    read -r PARAM_1 PARAM_2 <<< "$outer_params"

    echo "=== start run skewness and r/w ratio: $PARAM_1 $PARAM_2 ==="

    # cross host ratio
    for ((i=0; i<=0; i++)); do
        I_VALUE=$((i * 10))

        echo "=== $PARAM_1, $PARAM_2, $I_VALUE ==="

        # start memory nodes
        for node in "${MN_NODE[@]}"; do
            ssh cc@${node} "cd ./motor/build/memory_node/server; ./motor_mempool" > /dev/null 2>&1 &
            echo "Memory node starts: $node"
        done

        # wait memory nodes run
        sleep 180

        # start compute node
        echo "start Compute Node: $CN_NODE_0，./run_ycsb 40 3 skewed $PARAM_1 $PARAM_2 0 $I_VALUE SR"
        ssh cc@$CN_NODE_0 "cd ./motor/build/compute_node/run; ./run_ycsb 40 3 skewed $PARAM_1 $PARAM_2 0 $I_VALUE SR"

	
        # kill memory nodes
        for node in "${MN_NODE[@]}"; do
            ssh cc@${node} "kill -9 \$(lsof -t -i:12346)" > /dev/null 2>&1
            echo "clean Memory Node: $node"
        done

	sleep 5

	# ensure memory nodes are killed
        for node in "${MN_NODE[@]}"; do
            ssh cc@${node} "kill -9 \$(lsof -t -i:12346)" > /dev/null 2>&1
            echo "clean Memory Node: $node"
        done
        echo
    done
    echo
done

echo "finish"
