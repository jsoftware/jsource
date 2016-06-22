coclass'jdemo4'
coinsert'jhs'

HBS=: 0 : 0
jhma''                    NB. menu start
'abc'    jhmg'Abc';1;8    NB. menu group
 'foo'   jhmab'do foo'    NB. menu item
'def'    jhmg'Def';1;8
 'goo'   jhmab'do goo'
'ghi'    jhmg'More stuff';1;8
 'moo'   jhmab'do moo d^' NB. d keyboard shortcut 
 'koo'   jhmab'do koo'
jhmz''                    NB. menu end  

jhh1'Controls with javascript and CSS'
'javascript:'
'which' jhspan''
jhbr,jhbr
'cb0'   jhcheckbox'checkbox 0';0
'cb1'   jhcheckbox'checkbox 1';1
jhbr
'rad0'  jhradio'radio one';1;'radgroup'
'rad1'  jhradio'radio two';0;'radgroup'
jhbr
'sel'   jhselect ('zero';'one';'two';'three';'four');1;0
jhbr
text
jhbr
'blue'  jhb'color blue'
'red'   jhb'color red'
jhbr
desc  
jhdemo''
)

text=: 0 : 0
Now is the <span id="one" class="mark">time</span>
for all <span id="two" class="mark">good</span>
folk to come to the party.
)

create=: 3 : 0
'jdemo4'jhr''
)

jev_get=: create

desc=: 0 : 0
<hr>This page has a few controls with simple javascript event handlers.
)

CSS=: 0 :0
span.mark{color:green;}
)

JS=: 0 : 0
function show(){jbyid("which").innerHTML= JEV;}

function ev_foo_click(){show();}
function ev_goo_click(){show();}
function ev_moo_click(){show();}
function ev_koo_click(){show();}

function ev_d_shortcut(){jbyid("which").innerHTML= "ev_d_shortcut";}

function color(c)
{
 show();
 jbyid("one").style.color=c;
 jbyid("two").style.color=c;
}

// set text span elements blue
function ev_blue_click(){color("blue");}

// set text span elements red
function ev_red_click(){color("red");}

// radio button handlers
function ev_rad0_click(){show();return true;}
function ev_rad1_click(){show();return true;}

// checkbox button handlers
function ev_cb0_click(){show();return true;}
function ev_cb1_click(){show();return true;}

function ev_sel_change()
{
 jbyid("which").innerHTML= JEV+" : "+jbyid("sel").selectedIndex;
}
)