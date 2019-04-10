#!/bin/bash

dir=$(dirname "$0")
cd ${dir}/..

input=("1:examples/example" "2:examples/example:cpp" "examples/example:in" "3:examples/example"\
		"4:examples/example:cpp" "5:examples/example:cxx" "6:examples/example:cc")

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