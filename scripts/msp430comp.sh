#!/bin/bash

echo "Script linker used"

var="$(grep '#__mspabi_func_epilog_' $1)"
#echo $var
Stanley=${var:0:26}
#echo $Stanley
sed -i "/$Stanley/a aaaaa	RET" $1
sed -i 's/aaaaa//' $1
sed -i "/$Stanley/d" $1

j=1
temp=1
while [ $temp -le 1 ]
do
  SUBS=$(echo $var| cut -d'#' -f $j)
  #echo $SUBS
  j=`expr $j + 1`
  
  uselessnum=${SUBS:21:1}
  usename=${SUBS:9:11}
  if [ -z "$SUBS" ]
  then
  temp=2
  #else
  echo $uselessnum
  #echo $usename
  #sed -i "/$usename/d" $1
  fi
done

