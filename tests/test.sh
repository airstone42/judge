#!/bin/bash

dir=$(dirname "$0")
cd ${dir}/..

term() {
	status=$(ps -ef | grep "$!" | grep -v grep | wc -l)
	if [[ ${status} != 0 ]]; then
		kill $!
	fi
	find "examples/" -regex "examples/[0-9].*" -delete
}

trap term SIGINT
trap term SIGTERM

judge="./Judge"
client="./Client"
input="tests/input.txt"

if [[ ! -f ${judge} || ! -f ${client} ]]; then
	cmake .
	make
fi

${judge} > /dev/null 2>&1 &
${client} < ${input}

term
