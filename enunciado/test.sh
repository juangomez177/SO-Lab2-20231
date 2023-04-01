#!/bin/bash

if [ -f test_out.txt ];
then
  rm test_out.txt
fi

for i in {1..22}
do
  ../tester/run-tests.sh -t $i tests >> test_out.txt
done

cat test_out.txt

