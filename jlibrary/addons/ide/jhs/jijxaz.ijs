coclass'jijxaz'
coinsert'jhs'

jev_get=: 3 : 0
t=. html rplc '<ADDRESS>';gethv_jhs_ 'Host:'
htmlresponse hrtemplate hrplc 'TITLE CSS JS BODY';'jijxframes';'';'';t
)

html=: 0 : 0
<frameset id="frameset" rows="75%,25%">
 <frame src="http://<ADDRESS>/jijx"
  name="framea",
  scrolling='auto'>
 <frame src="http://<ADDRESS>/jhelp"
  name="framez",
  scrolling='auto'>
</frameset>
)

0 : 0
   jjs'parent.frames[1].name="fubar"'
   jjs'parent.document.getElementById("frameset").rows="75%,*"'
   jjs'parent.document.getElementById("frameset").rows="100%,*"'
)
