#!/bin/bash
for i in {0..99}
do
	echo "Test for $i load"
	time ./force_inherit_bench ${i} 20
done