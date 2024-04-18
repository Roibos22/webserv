#!/usr/bin/env python3
import os
import cgitb
import sys

cgitb.enable()

res = 0
print("\r\n\r\n")  # Blank line to end headers

file_path = "./test_websites/website/cgi-bin/tmp/name.txt"
try:
	with open(file_path, 'r') as file:
		print(file.read())
except FileNotFoundError:
	print("Incognito")
