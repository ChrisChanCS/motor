# Procedure to reproduce Tigon's experiment
This document explains how to reproduce Tigon's results, from how to establish the environment to how to run scripts.

# Requirement
- Hardware
  - BCM57414 NetXtreme-E 10Gb/25Gb RDMA Ethernet Controller
- Software
  - Operating System: Ubuntu 20.04 LTS
- Machines
  - 4 machines: 1 compute node and 3 memory nodes (including 1 primary and 2 backups)

# Establish RDMA environment in Ubuntu 20.04
Motor uses old-version library. To compile Motor, we try the MLNX_OFED_LINUX-4.9-0.1.7.0 and it works.

- First, please download MLNX_OFED_LINUX-4.9-0.1.7.0-ubuntu20.04-x86_64.tgz in the official websit.

- Ensure you have correct vm environment.

```sh 
$ sudo apt update
$ sudo apt install net-tools
$ sudo apt remove --purge qemu-system-common qemu-utils libguestfs0 libiscsi7 librados2 librdmacm1 libguestfs-tools librbd1 libguestfs-perl qemu-block-extra
$ sudo apt install -y chrpath libgfortran4 tk automake pkg-config autotools-dev m4 dpatch debhelper autoconf libltdl-dev quilt swig bison tcl graphviz gfortran flex
```

- Install MLNX_OFED_LINUX.

```sh 
$ sudo ./mlnxofedinstall --with-mlnx-ofed-kernel --without-fw-update
$ sudo /etc/init.d/openibd restart
```

- Then, you should activate RDMA device.

```sh 
$ sudo ip link set ens2 up # check your device name, here is ens2
$ sudo ip addr add 10.31.1.152/28 dev ens2 # use an network interface for RDMA
```

- Enable mutual network connection, e.g., firewall-cmd.

```sh 
$ sudo firewall-cmd --zone=trusted --add-source=10.31.1.151
$ sudo firewall-cmd --zone=trusted --add-source=10.31.1.152
```

- At last, you should past the client/server test.

```sh 
$ ib_send_bw # server
$ ib_send_bw 10.31.1.152 # client
```

# Run
Before the experiment, please see Motor's original instructions to setup ip configuration of your cluster. We provide an example below and scripts to run Motor in the ```/script/``` folder.

- Config compute node.

```
{
  "local_compute_node": {
    "machine_num": 1,
    "machine_id": 0,
"thread_num_per_machine": 16,
"coroutine_num": 3,
    "local_port": 12345,
    "comment": "1 is snapshot isolation, 2 is serializability",
"iso_level": 2,
    "crash_tnum": 20,
    "crash_time_ms": 6000,
    "tp_probe_interval_ms": 1
  },
  "remote_mem_nodes": {
    "remote_ips": [
      "10.31.1.147", # ip of memory node 1
      "10.31.1.148", # ip of memory node 2
      "10.31.1.157" # ip of memory node 3
    ],
    "remote_ports": [
      12346,
      12346,
      12346
    ],
    "remote_meta_ports": [
      12347,
      12347,
      12347
    ]
  }
}
```

- Config memory node.

```
{
  "local_memory_node": {
    "machine_num": 3, # memory count
    "machine_id": 0, # index of this memory node, here is 0
    "local_port": 12346,
    "local_meta_port": 12347,
    "use_pm": 0,
    "pm_root": "/dev/dax0.1",
    "reserve_GB": 60, # please adjust this number if program reports not enough space
    "max_client_num_per_mn": 50,
    "per_thread_delta_size_MB": 400, # please adjust this number if program reports not enough delta space
    "workload": "TPCC"
  },
  "remote_compute_nodes": {
    "compute_node_ips": [
      "10.0.0.7" # ip of compute node
    ],
    "compute_node_ports": [
      12345
    ]
  },
  "other_memory_nodes": {
    "memory_node_ips": [
      "10.0.0.3", # ip of memory node 1
      "10.0.0.5" # ip of memory node 2
    ],
    "memory_node_ids": [
      1, # index of memory node 1
      2 # index of memory node 2
    ],
    "memory_node_ports": [
      12346,
      12346
    ]
  }
}
```




# Motor
An open-source repository for our paper in [OSDI 2024](https://www.usenix.org/conference/osdi24).

> **Ming Zhang**, Yu Hua, Zhijun Yang. "Motor: Enabling Multi-Versioning for Distributed Transactions on Disaggregated Memory". In 18th USENIX Symposium on Operating Systems Design and Implementation, OSDI 2024, Santa Clara, California, USA, July 10 - 12, 2024.


# Brief Introduction
In modern datacenters, memory disaggregation unpacks monolithic servers to build network-connected distributed compute and memory pools to improve resource utilization and deliver high performance. The compute pool leverages distributed transactions to access remote data in the memory pool and provide atomicity and strong consistency. Existing single-versioning designs have been constrained due to limited system concurrency and high logging overheads. Although the multi-versioning design in the conventional monolithic servers is promising to offer high concurrency and reduce logging overheads, which however fails to work in the disaggregated memory. In order to bridge the gap between the multi-versioning design and the disaggregated memory, we propose Motor that holistically redesigns the version structure and transaction protocol to enable multi-versioning for fast distributed transaction processing on the disaggregated memory. To efficiently organize different versions of data in the memory pool, Motor leverages a new consecutive version tuple (CVT) structure to store the versions together in a continuous manner, which allows the compute pool to obtain the target version in a single network round trip. On top of CVT, Motor leverages a fully one-sided RDMA-based MVCC protocol to support fast distributed transactions with flexible isolation levels. Experimental results demonstrate that Motor improves the throughput and reduces the latency compared with state-of-the-art systems. To learn more, please read our paper.

# Requirements
- Hardware
  - Mellanox InfiniBand NIC (e.g., ConnectX-5) that supports RDMA
  - Mellanox InfiniBand Switch
- Software
  - Operating System: Ubuntu 18.04 LTS
  - Programming Language: C++ 11
  - CMake: 3.3 or above
  - Compiler: g++ 7.5.0 or above
  - Libraries: ibverbs, pthread, boost_coroutine, boost_context, boost_system
- Machines
  - 4 machines: 1 compute node and 3 memory nodes (including 1 primary and 2 backups)

# Configurations
- Change the workload to run in ```txn/flags.h```.
- Configure compute nodes and memory nodes respectively in ```config/cn_config.json``` and ```config/mn_config.json```.
- If you change the number (MUST > 0) of backup replicas, please change the value of ```BACKUP_NUM``` in ```txn/flags.h```.

# Build
We provide a shell script for easy building.

```sh
$ git clone https://github.com/minghust/motor.git
$ cd motor
```

- For each cn in compute pool (boost required)

```sh 
$ ./build.sh
```

- For each mn in memory pool

```sh 
$ ./build.sh -s
```


Add ```-d``` option if you want Debug mode.

PS. If you are familiar with our open-source repository [FORD](https://github.com/minghust/ford), you would become more easier to understand the building process and code structure of Motor.


# Run
- For each MN, initialize RDMA, allocate memory, and load database tables.
```sh
$ cd motor
$ cd ./build/memory_node/server
$ ./motor_mempool
```

- For each CN, run a benchmark after all database tables loaded.
```sh
$ cd motor
$ cd ./build/compute_node/run
$ ./run tpcc 28 3 SR # run tpcc at the serializability isolation level using 28 threads in which each thread generates 3 coroutines. The CPUs of MNs are not involved during transaction execution.
```

Notice. If you want to play with a different benchmark in your next run, please do not forget to change the option in ```txn/flags.h``` and rebuild Motor :)

# Results
After running, a ```bench_results``` directory is automatically generated to store the results according to the specific benchmark being evaluated. The summarized attempted and committed throughputs (K txn/sec), and the P50 and P99 latencies are reported in ```bench_results/<benchmark>/result.txt```.

# Acknowledgments

We sincerely thank the following open-source repositories (in ```thirdparty```) that help us shorten the developing process.

- [rlib](https://github.com/wxdwfc/rlib): We use rlib to handle RDMA connections. This is a convinient and easy-to-understand library.

- [rapidjson](https://github.com/Tencent/rapidjson): We use rapidjson to read configurations from json files. This is an easy-to-use library.

# LICENSE

```text
Copyright [2023] [Ming Zhang]

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
```
