#!/usr/bin/env python3
import os
import html
import urllib.parse
import cgitb
import sys

cgitb.enable()
directory = '/test_websites/website/upload/tmp/'

print("\r\n\r\n")  # Blank line to end headers

print("<html><head><title>List Files</title>")
# Include a CDN link for the FontAwesome icons if not already included
print('<link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.15.1/css/all.min.css">')
print("</head><body>")
print("<ul>")

try:
    files = [f for f in os.listdir('.' + directory) if not f.startswith('.')]

    if not files:
        print("<p>No files uploaded yet</p>")

    for filename in os.listdir('.' + directory):
        if filename.startswith('.'):
            continue

        escaped_filename = html.escape(filename)
        url_encoded_filename = urllib.parse.quote(escaped_filename)

        # Print the file delete button
        print(f"""<li>
        <button onclick="if(confirm('Are you sure you want to delete {escaped_filename}?')) 
        {{ fileDeleteButtonPressed('{directory + url_encoded_filename}'); }}" 
        style="color: red; border: none; background: none; cursor: pointer;">
        <i class="fas fa-times"></i></button>
        {escaped_filename}
        """)
        #print("appended file", file=sys.stderr)

except Exception as e:
    print(f"<p>Error: {e}</p>")
print("</ul>")
print("</body></html>")