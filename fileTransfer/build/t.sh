#! /bin/bash

ips=(1 2 3 4)

for ((i=0; i < ${#ips[@]}; i++))
do
  echo ${ips[$i]}
  done
