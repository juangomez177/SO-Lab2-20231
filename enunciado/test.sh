#!/bin/bash

for i in {1..22}
do
  # if [[ $i == 17 ]]
  # then
  #   continue
  # fi
  ../tester/run-tests.sh -t $i tests >> test_out.txt
done

cat test_out.txt