from ubuntu:14.04

run apt-get update
run apt-get upgrade -y

run apt-get install -y build-essential
run apt-get install -y valgrind 

env CFLAGS -Wno-packed-bitfield-compat
