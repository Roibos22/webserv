#!/usr/bin/env python3
import os
import cgitb
import sys

cgitb.enable()

res = 0

print("1'name='", file=sys.stderr)

if len(sys.argv) == 2:
	argument = sys.argv[1]
	
	# Check if the argument starts with "name="
	if not argument.startswith("name="):
		print("Error: argument does not start with 'name='", file=sys.stderr)
		sys.exit(1)  # Exit with an error code
	name_value = argument.split("name=")[1]
	if name_value == "":
		print("Error: Name is empty")
		sys.exit(1)  # Exit with an error code
	
	file_path = "./test_websites/website/cgi-bin/tmp/name.txt"
	os.makedirs(os.path.dirname(file_path), exist_ok=True)
	with open(file_path, 'w') as file:
		file.write(name_value)
	res = 1
else:
	print("Error: Incorrect number of arguments", file=sys.stderr)
	sys.exit(1)  # Exit with an error code

if res:
	print("<span class=\"checkmark\">&#10004;</span> Saved Name")