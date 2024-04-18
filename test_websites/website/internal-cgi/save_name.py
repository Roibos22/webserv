#!/usr/bin/env python3
import os
import cgitb
import sys

cgitb.enable()

print("'name='", file=sys.stderr)

# Check if the QUERY_STRING environment variable is set
if 'QUERY_STRING' in os.environ:
    query_string = os.environ['QUERY_STRING']
    
    # Check if the query string starts with "name="
    if not query_string.startswith("name="):
        print("Error: query string does not start with 'name='", file=sys.stderr)
        sys.exit(1)  # Exit with an error code
    name_value = query_string.split("name=")[1]
    if name_value == "":
        print("Error: Name is empty")
        sys.exit(1)  # Exit with an error code

    file_path = "./test_websites/website/cgi-bin/tmp/name.txt"
    os.makedirs(os.path.dirname(file_path), exist_ok=True)
    with open(file_path, 'w') as file:
        file.write(name_value)
    print("<span class=\"checkmark\">&#10004;</span> Saved Name")
else:
    print("Error: QUERY_STRING environment variable is not set", file=sys.stderr)
    sys.exit(1)  # Exit with an error code

