

1. At first building run script from root project path:
   ./scripts/install_need_libs_and_start_postgresql.sh

2. For build run commands from root project path: 
    cmake -Bbuild && cd build && make 

3. For test project run sequentially two executable files from root project path: 
   ./build/src/postgresql_proxy_server/POSTGRESQL_PROXY_SERVER
   ./build/src/postgresql_clients/POSTGRESQL_CLIENTS
