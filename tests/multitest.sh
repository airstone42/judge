#!/bin/bash

dir=$(dirname "$0")
cd ${dir}/..

input=("examples/test" "examples/test:cpp" "examples/test:in" "1:examples/test"\
		"2:examples/test:cpp" "3:examples/test:cxx" "4:examples/test:cc")

term() {
	find "examples/" -regex "examples/[0-9].*" -delete
}

trap term SIGINT
trap term SIGTERM

judge="./Judge"
client="./Client"

if [[ ! -f ${judge} || ! -f ${client} ]]; then
	cmake .
	make
fi

for (( i = 0; i < 10; i++ )); do
	r1=$(( RANDOM % ${#input[@]} ))
	r2=$(( RANDOM % ${#input[@]} ))
	r3=$(( RANDOM % ${#input[@]} ))
	r4=$(( RANDOM % ${#input[@]} ))
	${client} <<EOF > tests/log/${i}.log 2>&1 &
${input[r1]}
${input[r2]}
${input[r3]}
${input[r4]}
EOF
done

${judge}

term
