#!/bin/bash

check_and_install_libraries(){
  local libraries=("$@")
  for library in "${libraries[@]}"; do
    dpkg -s "$library" &> /dev/null
    if [ $? -eq 0 ]; then
      echo "Library $library already installed"
    else
    
      echo "Install library $library"
      sudo apt update
      if [ ! $? -eq 0 ];then
        echo "not correct run command: sudo apt update"
        exit 1
      fi
      
      sudo apt install -y "$library"
      if [ ! $? -eq 0 ];then
       echo "not can install library:$library"
       exit 1
      fi 
      
      sudo ldconfig
      if [ ! $? -eq 0 ];then
       echo "cont correct run command: sudo ldconfig"
       exit 1
      fi
      
    fi
  done
}

start_postgresql(){
 sudo systemctl start postgresql
  if [ ! $? -eq 0 ];then
   echo "Error start start postgresql"
   exit 1
  fi
  
 sudo systemctl enable postgresql
  if [ ! $? -eq 0 ];then
   echo "Error enable postgresql"
   exit 1
  fi 
 
 sudo -u postgres psql -c "ALTER SYSTEM SET ssl=off;"
  if [ ! $? -eq 0 ];then
   echo "Error disable ssl for postgresql"
   exit 1
  fi 
  
 sudo -u postgres psql -c "ALTER USER postgres WITH PASSWORD 'postgres';"
  if [ ! $? -eq 0 ];then
   echo "Error set password for postgresql"
   exit 1
  fi  
 
 sudo -u postgres psql -c "CREATE TABLE TEST_TABLE ( id SERIAL PRIMARY KEY, rand_str varchar);"

 sudo systemctl restart postgresql
  if [ ! $? -eq 0 ];then
   echo "Error restart postgresql"
   exit 1
  fi  
}

PQXX_GIT=https://github.com/jtv/libpqxx.git
PQXX=pqxx

build_pqxx(){
 cd $PQXX
 ./configure --enable-static CXXFLAGS="-std=c++17"
 
 mkdir build
 cd build
 
 cmake -DSKIP_BUILD_TEST=on -DBUILD_DOC=off -DBUILD_SHARED_LIBS=on ..
 if [ ! $? -eq 0 ];then
  echo "can not run cmake for PQXX"
  exit 1
 fi
 
 make -j`nproc`
 if [ ! $? -eq 0 ]; then
  echo "cant build GTEST"
 fi
 
 sudo make install
 cd ../..
 sudo rm -r $PQXX
}

clone_and_build_PQXX(){
 git clone $PQXX_GIT $PQXX
 if [ $? -eq 0 ]; then
  build_pqxx
 else
  echo "PQXX git in not available"
  exit 1
 fi
}

check_PQXX(){
 if [ -f "/usr/local/lib/cmake/libpqxx/libpqxx-config.cmake" ] && [ -f "/usr/local/lib/libpqxx.so" ] && [ -d "/usr/local/include/pqxx" ]; then
    echo "pqxx library is installed"
 else
    clone_and_build_PQXX
 fi
}

check_and_install_libraries git cmake gcc g++ postgresql postgresql-contrib  postgresql-server-dev-all

start_postgresql

check_PQXX
