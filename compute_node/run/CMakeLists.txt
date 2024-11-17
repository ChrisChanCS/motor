# Author: Ming Zhang
# Copyright (c) 2023

set(RUN_SRC run.cc)
add_executable(run ${RUN_SRC})
target_link_libraries(run handler)

set(RUN_MICRO_SRC run_micro.cc)
add_executable(run_micro ${RUN_MICRO_SRC})
target_link_libraries(run_micro handler)

set(RUN_YCSB_SRC run_ycsb.cc)
add_executable(run_ycsb ${RUN_YCSB_SRC})
target_link_libraries(run_ycsb handler)