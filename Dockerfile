from ubuntu:14.04

run apt-get update
run apt-get upgrade -y

run apt-get install -y build-essential
run apt-get install -y valgrind 

run mkdir /ice
add src /ice/src
add Makefile /ice/Makefile
add gdb /ice/gdb
add valgrind /ice/valgrind

workdir /ice
run make -j4

env PATH $PATH:/ice/build
