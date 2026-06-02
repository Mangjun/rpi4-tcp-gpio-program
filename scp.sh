#!/bin/bash

make
scp ./lib/librpi.so mangjun@172.20.33.117:/home/mangjun/
scp ./lib/server mangjun@172.20.33.117:/home/mangjun/