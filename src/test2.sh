#!/usr/bin/bash

echo "test2"

./triglav test2.triglav > test2.output

if diff test2.output test2.expected; then
	echo "OK";
else
	echo "error";
	exit -1;
fi
