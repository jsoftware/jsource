coclass'app1'
coinsert'jhs'

HBS=: 0 : 0
     jhh1'app1 - HBS - html'
     jhhr
 'b1'jhb'b 1'
 't1'jhtext'text field 1';10
     desc
)

desc=: 0 : 0
<pre>
JHS app is built from 5 parts:
   HBS - html elements - buttons, input fields, text, ...
   JS - javascript event handlers
   J event handlers (called by javascript event handlers)
   CSS - colors, margins, borders, ...
   INC - css/js library files

HBS - J sentences that define html for the document

skim HTML resources (web or books) to get a basic familiarity
no need to be an expert, but you need to know a bit
cut/paste and following patterns can get you a long ways
search for a well phrased question often gives all the answer you need

study app1.ijs to see how it creates app1 document

try some HBS sentences to see the HTML they create - note defined in jhs locale
   jhh1_jhs_'an h1 header'

try editing HBS to change the text on the b1 button - run script and F5 refresh app

pressing b1 gets an alert as a handler is not defined
</pre>
)

NB. verb that responds to browser request for app1 page
NB. (tab title) jhrx (browser source to create the page)
jev_get=: 3 : 0
'app1'jhrx(getcss''),(getjs''),gethbs''
)
