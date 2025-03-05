#!/bin/bash
cd ../../build
n=1
un=1
for i in {4..30..2}
do
    for k in {1..5}
    do
        echo $var
        ./InitInstances d "trial/instances_$n" s $i
        n=$(($n+$un))
    done
done