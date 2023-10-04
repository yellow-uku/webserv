#!/usr/local/bin/python3
import re
import sys
import os
# from pathlib import Path

# print(f'Content Type: {os.environ.get("CONTENT_TYPE", "None")}\n')

def parse_file():

	# content_type = "CONTENT_TYPE=multipart/form-data; boundary=------WebKitFormBoundaryd1xz0AI76g1urKJw"

	# print("BEGIN PARSING POST REQUEST...\n")

	content_type = os.environ["CONTENT_TYPE"]

	# print("content_type: ", content_type)

	boundary = None
	match = re.match(r"multipart/form-data;\s*boundary=(.*)", content_type)
	if match:
		boundary = match.group(1).strip().encode()
		# print("boundary: ", boundary)

	# print(boundary)
	if not boundary:
		print("Error: boundary is not set")
		exit(1)

	file_content = sys.stdin.buffer.read()

	# print("file_content: ", file_content)
	#with open(SERVER_READ_PATH / FILENAME, "rb") as f:
	#    file_content = f.read()

	parts = file_content.split(boundary)
	if len(parts) < 3:
		print("Error: no boundary in file")
		exit(1)

	is_lf_mode = False

	file_content = parts[1]
	parts = file_content.split(b"\r\n\r\n", 1)
	if len(parts) != 2:
		is_lf_mode = True
		parts = file_content.split(b"\n\n", 1)
		if len(parts) != 2:
			print("Bad file content")
			exit(1)

	raw_headers, body = parts
	headers = [x.strip() for x in raw_headers.split(b"\n")]
	headers = [x for x in headers if x]
	for header in headers:
		header_parts = header.split(b": ", 1)
		if len(header_parts) != 2:
			continue
		key, value = header_parts
		if key.lower() == b"content-disposition":
			# content_type_name = None
			content_type_filename = None
			match = re.match(rb'form-data;\s*name="(.*)";\s*filename="(.*)"', value)
			if match:
				# content_type_name = match.group(1)
				content_type_filename = match.group(2)

	if not content_type_filename:
		print("Error: filename is not set in Content-Type header")
		exit(1)

	if is_lf_mode:
		if body.endswith(b"\n"):
			body = body[:-1]
	else:
		if body.endswith(b"\r\n"):
			body = body[:-2]

	if is_lf_mode:
		if body.endswith(b"\n--"):
			body = body[:-3]
	else:
		if body.endswith(b"\r\n--"):
			body = body[:-4]

	# if body.endswith(b"--"):
	#     body = body[:-4]

	# print("BODY UPLOAD:\n", body)
	with open(str(os.environ.get("SERVER_WRITE_PATH", "None")).encode() + b"/" +  content_type_filename, "wb") as f:
	    f.write(body)

try:
	parse_file()
	# print(f'Content Type: {os.environ.get("CONTENT_TYPE", "None")}')
	print("")
	print("<center>")
	print("<h1>Success!</h1>")
	print("</center>")
except Exception as e:
	# print(f'Content Type: {os.environ.get("CONTENT_TYPE", "None")}')
	print("")
	print("<center>")
	print("<h1>Fail...</h1>")
	print("</center>")
	print("Exception: ", e)
	exit(1)
