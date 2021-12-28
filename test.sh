#!/bin/bash

cd tc
passed=0
failed=0
for i in $(ls 0*)
do
	#echo "Testing $i"
	cat $i | ../main > ./tmp
	diff ./tmp EXPECT_$i
	if [ $? -eq 0 ]
	then
		echo -e "$i: \e[32m[PASSED]\e[0m"
		passed=$[passed+1]
	else
		echo -e "$i: \e[31m[FAILED]\e[0m"
		failed=$[failed+1]
	fi
done
cd -
echo -e "\e[33m $passed/$[passed+failed] tests passed.\e[0m"
echo -e "\e[33m Success rate: $[passed*100/(passed+failed)]%\e[0m"
