NB. JHS - file source stuff - favicon.ico ...
coclass'jfilesrc'
coinsert'jhs'

NB. serves .htm .js etc pages from anywhere
NB. possible security issues! careful on allowing other suffixes!
jev_get=: 3 : 0
if. 0=nc<'gets_jhs_' do. gets_jhs_=: gets_jhs_,y,LF end.
if. y-:'favicon.ico' do. favicon 0 return. end.
if. (-.IFWIN)*.'usr/share/'-:10{.y do. y=. '/',y end. NB. gnuplot kludge
y=. jpath y
d=. fread y
if. _1=d do. echo 'get file does not exist: ',y return. end.
NB. Firefox 8 requires a response header
if. ('.htm'-:_4{.y)+.'.html'-:_5{.y do. htmlresponse d,~fsrchead rplc '<TYPE>';'text/html' return. end.
if. (#mimetypes) > i=. ({:"1 mimetypes) i. <@tolower@}.(}.~ i:&'.') y do.
 t=. i{:: {."1 mimetypes
else. echo 'will not get file ',y return. end.
t gsrcf d
)

NB. common mime types
mimetypes=: <;._1@(' '&,)@deb;._2 (0 : 0)
application/javascript         js
application/javascript         map
application/msword             doc
application/msword             docx
application/pdf                pdf
application/postscript         eps
application/postscript         ps
application/rtf                rtf
application/vnd.ms-excel       xls
application/vnd.ms-excel       xlsx
application/x-gnumeric         gnumeric
application/x-gtar-compressed  tgz
application/x-sc               sc
application/zip                zip
audio/mp3                      mp3
image/gif                      gif
image/jpeg                     jpeg
image/jpeg                     jpg
image/png                      png
image/tiff                     tif
image/tiff                     tiff
image/x-icon                   ico
image/x-ms-bmp                 bmp
text/css                       css
text/csv                       csv
text/html                      htm
text/html                      html
text/plain                     text
text/plain                     txt
video/x-ms-wmv                 wmv
video/x-msvideo                avi

)

favicon=: 3 : 0
htmlresponse htmlfav,1!:1 <jpath'~addons/ide/jhs/favicon.ico'
)

htmlfav=: toCRLF 0 : 0
HTTP/1.1 200 OK
Server: J
Accept-Ranges: bytes
Content-Length: 1150
Keep-Alive: timeout=15, max=100
Connection: Keep-Alive
Content-Type: image/x-icon

)

