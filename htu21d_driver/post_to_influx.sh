#!/bin/bash
epoch=$(date +'%s')

curl -i -XPOST http://localhost:8086/query --data-urlencode "q=CREATE DATABASE environment"


value="weather,sensor=$2,region=$1"
output=""
while read x
do
    IFS=$'\t'
    y=($x)
    xcol=$(echo ${y[0]}|tr " " "_"|awk '{$1=$1};1')
    xval=$(echo ${y[1]}|awk '{$1=$1};1')
    unit=$(echo ${y[2]}|awk '{$1=$1};1')
    unset IFS
    value="weather,sensor=$2,region=$1,variable=$xcol,unit=$unit value=$xval $epoch"
    curl -i -XPOST 'http://localhost:8086/write?db=environment&precision=s' --data-binary "$value"
done < <(/etc/htu21d/htu21d)
#curl -i -XPOST 'http://localhost:8086/write?db=environment&precision=s' --data-binary 
