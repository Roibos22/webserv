#!/usr/bin/env python3
import os
import urllib.parse
import html

query_string = query_string = os.environ['QUERY_STRING']
query_string = query_string.split("char=")[1]


print("Content-type: text/html\n")
print("\r\n\r\n")
print("<html>")
print("<head>")
print("<title>Python CGI Test Script</title>")
print("<style>")
print("@keyframes colorchange {")
print("  0%   {color: blue; font-size: 72px;}")
print("  50%  {color: red; font-size: 150px;}")
print("  100% {color: blue; font-size: 72px;}")
print("}")
print("body { display: flex; justify-content: center; align-items: center; height: 100vh; }")
print(".user-message { font-weight: bold; animation: colorchange 3s infinite; }")
print("</style>")
print("</head>")
print("<body>")
print("<h1 class='user-message'>{}</h1>".format(html.escape(query_string)))
print("</body>")
print("</html>")
# print("<style>")
# print("@keyframes colorchange {")
# print("  0%   {color: blue;}")
# print("  100% {color: red;}")
# print("}")
# print("body { display: flex; justify-content: center; align-items: center; height: 100vh; }")
# print(".user-message { color: blue; font-size: 120px; font-weight: bold; animation: colorchange 3s infinite;}")
# print("</style>")
# print("</head>")
# print("<body>")
# print("<h1 class='user-message'>{}</h1>".format(html.escape(query_string)))
# print("</body>")
# print("</html>")
