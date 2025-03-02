#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

#do all the pervious one command only tests
@test "do not remove space from middle of quote" {
	run "./dsh" <<EOF
echo "hello  world" 
EOF
	# Strip all whitespace (spaces, tabs, newlines) from the output
	stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v')
	
	# Expected output with all whitespace removed for easier matching
	expected_output="hello  worlddsh3> dsh3> cmd loop returned 0"
	
	# These echo commands will help with debugging and will only print
	#if the test fails
	echo "Captured stdout:" 
	echo "Output: $output"
	echo "Exit Status: $status"
	echo "${stripped_output} -> ${expected_output}"
	
	# Check exact match
	[ "$stripped_output" = "$expected_output" ]
}

@test "do not remove space from front of quote" {	run "./dsh" <<EOF
echo " hello world"
EOF
	# Strip all whitespace (spaces, tabs, newlines) from the output
	stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v')
	
	# Expected output with all whitespace removed for easier matching
	expected_output=" hello worlddsh3> dsh3> cmd loop returned 0"
	
	# These echo commands will help with debugging and will only print
	#if the test fails
	echo "Captured stdout:"
	echo "Output: $output"
	echo "Exit Status: $status"
	echo "${stripped_output} -> ${expected_output}"
	
	# Check exact match
	[ "$stripped_output" = "$expected_output" ]
	
}

@test "do not remove space from end of quote" {
	run "./dsh" <<EOF
echo "hello world "
EOF

	# Strip all whitespace (spaces, tabs, newlines) from the output
	stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v')
	
	# Expected output with all whitespace removed for easier matching
	expected_output="hello world dsh3> dsh3> cmd loop returned 0"
	
	# These echo commands will help with debugging and will only print
	#if the test fails
	echo "Captured stdout:"
	echo "Output: $output"
	echo "Exit Status: $status"
	echo "${stripped_output} -> ${expected_output}"
	
	# Check exact match
	[ "$stripped_output" = "$expected_output" ]
}


@test "remove extra spaces when no quotes" {
	run "./dsh" <<EOF
echo hello  world
EOF

	# Strip all whitespace (spaces, tabs, newlines) from the output
	stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v')
	
	# Expected output with all whitespace removed for easier matching
	expected_output="hello worlddsh3> dsh3> cmd loop returned 0"
	
	# These echo commands will help with debugging and will only print
	#if the test fails
	echo "Captured stdout:"
	echo "Output: $output"
	echo "Exit Status: $status"
	echo "${stripped_output} -> ${expected_output}"
	
	# Check exact match
	[ "$stripped_output" = "$expected_output" ]
}


#check that cd is working properly
@test "Change directory" {
	current=$(pwd)
	
	cd /tmp
	mkdir -p foo
	
	run "${current}/dsh" <<EOF
cd foo
pwd
EOF

	# Strip all whitespace (spaces, tabs, newlines) from the output
	stripped_output=$(echo "$output" | tr -d '[:space:]')
	
	# Expected output with all whitespace removed for easier matching
	expected_output="/tmp/foodsh3>dsh3>dsh3>cmdloopreturned0"
	
	# These echo commands will help with debugging and will only print
	#if the test fails
	echo "Captured stdout:" 
	echo "Output: $output"
	echo "Exit Status: $status"
	echo "${stripped_output} -> ${expected_output}"
	
	
	# Check exact match
	[ "$stripped_output" = "$expected_output" ]
	
	# Assertions
	[ "$status" -eq 0 ]
}

@test "Change directory without arguments does not move you" {

	current=$(pwd)
	cd /tmp
	mkdir -p foo
	
	run "${current}/dsh" <<EOF
cd foo
cd
pwd
EOF
	
	# Strip all whitespace (spaces, tabs, newlines) from the output
	stripped_output=$(echo "$output" | tr -d '[:space:]')
	
	# Expected output with all whitespace removed for easier matching
	expected_output="/tmp/foodsh3>dsh3>dsh3>dsh3>cmdloopreturned0"
	
	# These echo commands will help with debugging and will only print
	#if the test fails
	echo "Captured stdout:"
	echo "Output: $output"
	echo "Exit Status: $status"
	echo "${stripped_output} -> ${expected_output}"
	
	
	# Check exact match
	[ "$stripped_output" = "$expected_output" ]
	
	# Assertions
	[ "$status" -eq 0 ]
}

#does Piping work
@test "Basic Pipe Test" {
	run "./dsh" <<EOF
echo "hello  world" | grep "hello"
EOF
	# Strip all whitespace (spaces, tabs, newlines) from the output
	stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v')
	
	# Expected output with all whitespace removed for easier matching
	expected_output="hello  worlddsh3> dsh3> cmd loop returned 0"
	
	# These echo commands will help with debugging and will only print
	#if the test fails
	echo "Captured stdout:"
	echo "Output: $output"
	echo "Exit Status: $status"
	echo "${stripped_output} -> ${expected_output}"
	
	# Check exact match
	[ "$stripped_output" = "$expected_output" ]
}

@test "Multiple Pipes Test" {
    run "./dsh" <<EOF
echo "hello world" | tr 'a-z' 'A-Z' | grep "HELLO"
EOF

	# Strip all whitespace (spaces, tabs, newlines) from the output
	stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v')
	
	# Expected output with all whitespace removed for easier matching
	expected_output="HELLO WORLDdsh3> dsh3> cmd loop returned 0"
	
	# These echo commands will help with debugging and will only print
	#if the test fails
	echo "Captured stdout:"
	echo "Output: $output"
	echo "Exit Status: $status"
	echo "${stripped_output} -> ${expected_output}"
	
	# Check exact match
	[ "$stripped_output" = "$expected_output" ]
}

@test "Pipes No Output Test" {
    run "./dsh" <<EOF
echo "hello" | grep "world"
EOF

	# Strip all whitespace (spaces, tabs, newlines) from the output
	stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v')
	
	# Expected output with all whitespace removed for easier matching
	expected_output="dsh3> dsh3> cmd loop returned 0"
	
	# These echo commands will help with debugging and will only print
	#if the test fails
	echo "Captured stdout:"
	echo "Output: $output"
	echo "Exit Status: $status"
	echo "${stripped_output} -> ${expected_output}"
	
	# Check exact match
	[ "$stripped_output" = "$expected_output" ]
}
