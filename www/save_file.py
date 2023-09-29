#!/usr/bin/python

while 1:
	pass

# import cgi, os
# import cgitb; cgitb.enable()
# form = cgi.FieldStorage()
# # Get filename here.

# print(form)

# fileitem = form['filename']
# # Test if the file was uploaded
# if fileitem.filename:
#    # strip leading path from file name to avoid
#    # directory traversal attacks
#    fn = os.path.basename(fileitem.filename)
#    open('/tmp/' + fn, 'wb').write(fileitem.file.read())
#    message = 'The file "' + fn + '" was uploaded successfully'
 
# else:
#    message = 'No file was uploaded'
 
# print ("""\
# Content-Type: text/html\n
# <html>
# <body>
#    <p>{message}</p>
# </body>
# </html>
# """.format("message"))