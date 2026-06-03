#!/bin/bash

make
scp ./bin/librpi.so mangjun@172.20.33.117:/home/mangjun/
scp ./bin/server mangjun@172.20.33.117:/home/mangjun/