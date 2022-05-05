#!/usr/bin/bash

echo "test3"

./triglav test3.triglav > test3.output

if diff test3.output test3.expected; then
	echo "OK";
else
	echo "error";
	exit -1;
fi
