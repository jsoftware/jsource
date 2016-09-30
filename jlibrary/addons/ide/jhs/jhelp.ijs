NB. J HTTP Server - jhelp app
coclass'jhelp'
coinsert'jhs'

HBS=: 0 : 0
jhma''
jhjmlink''
jhmz''
jumps
jhresize''
text hrplc 'PAREN CONFIG JVERSION';')';'~addons/ide/jhs/config/jhs_default.ijs';JVERSION
)

jev_get=: 3 : 0
'jhelp'jhr''
)

jumps=: 0 : 0
<a href="#highlights">highlights</a>&nbsp;
<a href="#help">help</a>&nbsp;
<a href="#changes">changes</a>&nbsp;
<a href="#jhs">JHS</a>&nbsp;
<a href="#config">config</a>&nbsp;
<a href="#ix">iX</a>&nbsp;
<a href="#ide">IDE</a>&nbsp;
<a href="#plot">plot</a>&nbsp;
<a href="#gui">GUI</a>&nbsp;
<a href="#jum">JUM</a>&nbsp;
<a href="#console">console</a>&nbsp;
<a href="#pop-up">pop-up</a>&nbsp;
<a href="#about">about</a>
)

text=: 0 : 0
<div>
<a name="highlights"><h1>highlights</h1>
<b>simple plot</b>
<p>Verb jd3 provides a simple way to plot your data.</p>

<pre class="jcode">
   jd3'help'
</pre>
<p>See <a href=#jd3>jd3</a> for details and
<a href="#plot">plot</a>&nbsp; for other plot facilities.</p>

<b>simple project</b>
<p>The simple project utilities (defined in ~addons/ide/jhs/sp.ijs) are an important part of JHS.
They can be used in any front end.</p>

<p>See noun sphelp for an easy way to manage projects and scripts.</p>

<p>See noun spxhelp for managed execution of scripts and tutorials.</p>

<a name="help"><h1>help</h1>
This document links to lots of information, but is itself quite short.
A bit of time here will pay off down the road.<br><br>

For complete documentation see:<br>
<a href="http://www.jsoftware.com/docs/help802/index.htm" target="_blank">www.jsoftware.com help</a><br>
<a href="http://www.jsoftware.com/docs/help802/dictionary/vocabul.htm" target="_blank">www.jsoftware.com vocabulary</a><br>
<a href="http://www.jsoftware.com/help/user/library.htm" target="_blank">www.jsoftware.com standard library</a></br>
<a href="http://www.jsoftware.com/jwiki/NuVoc" target="_blank">www.jsoftware.com wiki NuVoc</a><br>

<br>if you have installed local help with jal see:<br>
<a href="~addons/docs/help/index.htm" target="_blank">~addons/docs/help/index.htm</a>

<a name="changes"><h1>changes</h1>
<b>September 2016 update</b>
<ul>
<li>simpler way to start JHS - see ~addons/ide/jhs/config/jhs.cfg</li>
<li>config PC_FONTFIXED and PC_FONTVARIABLE</li>
<li>jd3 plot simplified and improved</li>
</ul>
<b>December 2015 update</b>
<ul>
<li>simple project simplifications and improvements</li>
<li>jfile lists windows drives (c: etc) and unix root (/)</li>
</ul>

<b>July 2014 update</b>
<ul>
<li>echo/smoutput displays immediately - http chunked transfer encoding</li>
<li>demo 10 Ajax chunks - http chunked transfer encoiding</li>
<li>demo 11 Ajax interval timer - similar to J602 wd timer</li>
<li>demo 12 WebGl 3d graphics</li>
<li>iX section has info on iPhone/iPad/... client support</li>
<li><i>server busy - event ignored</i> for new request when waiting for response</li>
<li>Config section expanded and an easier way to run a server configured for port etc.</li>
<li>codemirror 4.2</li>
<li> IE 11 HTML 5 support adequate as JHS client</li>
</ul>

<b>Oct 2013 update</b>
<ul>
<li>jd3 support added (see plot section for details)
<li>jtable - table editor with handsontable/jquery - <a target="_blank" href="http://www.handsontable.com">www.handsontable.com</a> </li>
<pre class="jcode">
   jtable 'e1';'n' [ n=: i.3 4
   jtable 'e2';'s' [ s=: 2 2$'aa';'b';'c';'dd'
   NB. try cut/paste, cell right-click, undo/redo
</pre>

Study ~addons/ide/jhs/jtable.ijs to see how it works and how easy it will be to extend.
<li>jquery javascript library easily integrates with the JHS framework - see jd3 and jtable</li>
<li>jfiles - list recent files</li>
<li>ctrl+shift+up/down arrow recall</li>
 <ul>
 <li>jijx - recall input lines</li>
 <li>jfile - recall folders</li>
 <li>jfif - recall what/where text lines</li>
 </ul>
<li>browser local storage carries recalls over J task sessions</li>
<li>jijx - menu action|clear window</li>
<li>jijx - menu action|clear refresh (refresh log)</li>
<li>jijx - menu action|clear LS (local storage recalls)</li>
<li>browser javascript calls to J are synchronous</li>
previously if J was busy an event for J got a busy alert and was discarded<br/>
alert was a nuisance if the delay would have been brief<br/>
now events are queued and will fire when J call completes<br/>
avoids alerts, but remember the queue - don't go click crazy when you don't see expected responses<br/>
<li>jlog utility removed</li>
<li>minor bug fixes</li>
</ul>

<a name="jhs"><h1>JHS (J HTTP Server)</h1></a>
JHS is a browser interface to J and
is an alternative to a more traditional desktop application
front end. JHS gives you interactive access to J, an IDE, and
a framework for developing and delivering web applications.
<br><br>

JHS is the J engine running as a console task configured with
scripts to run as an HTTP server. A browser gets pages
from JHS.<br><br>

In default configuration, JHS is similar to a
desktop application. It runs on your desktop and
services local requests from your browser.
It can be configured to provide services to browsers
across a local area network or across the web.<br><br>

JHS is a departure from previous desktop application
front ends and is somewhat experimental.
One issue that arises is whether the JHS IDE should
be as much like a desktop frontend as possible or
whether it should be as much like a browser app as
possible.<br><br>

JHS leans strongly towards being a browser app. You may miss
some desktop features and it may take a while to be comfortable
with the browser approach. With a bit a patience and the
adoption of powerful browser paradigms the result might
please. One advantage of this is that developing new browser
apps from a browser app helps focus the mind.<br><br>

JHS reinvents the wheel in the sense that it doesn't make
use of any of the excellent javascript toolkits for browser
applications. This was partly for the fun of it and partly
because it seemed possible to do so in a lean and effective way.
The JHS javascript library is tiny compared to kits like
jquery.js and ext.js. Hopefully this makes it easier to learn
and use for those who aren't proficient javascript programmers.
There was also a feeling that these toolkits have
largely been developed to deal with complex issues of browser
incompatibilies in older browsers. By insisting on
modern browsers JHS avoids many of these issues. And looking
a bit forward to html5 much of the nice packaging of 
services in these toolkits will come for free. And if
appropriate, it is easy to include any additional toolkit
with the JHS framework and have the best of all worlds
in developing your browser app.

<a name="config"><h1>config</h1>
A jconsole task becomes a server when the JHS scripts are loaded,
configured, and initialized.<br><br>

JHS is configured to serve a port (e.g., 65001), allow any or
only localhost clients, to require a user/pass, etc.<br><br>

Study ~addons/ide/jhs/config/jhs.cfg to see how to start a configured JHS.<br><br>

Usually http://127.0.0.1:65001/jijx is the same as http://localhost:65001/jijx.<br><br>

The JHS port must be accessible through the firewall for clients on other machines.
Allowing access to a port varies between platforms.
If unfamiliar with the steps, do an internet search for: your_platform firewall port access.<br><br>

To access JHS from another machine you need to know the ip address of the server.
Finding this varies between platforms.
If unfamiliar with the steps, do an interent search for: your_platform find ip address.<br><br>

If the ip address is of the form 192.168.0.? then it is a LAN ip address. This address can be used
to access the server from other machines on the same LAN.<br><br>

If the address is not a LAN ip address then you can probably access the server with it from another machine.
An example of this would be an AWS machine that has its own unique ip address.<br><br>

To access a server on a LAN from a machine that is not on the LAN you need to configure the router
to forward the port. The first step is to find the ip address of the router (this is the ip address
seen by the rest of the internet). You may be able to get the routers ip address with:
<pre class="jcode">   getexternalip_jhs_''</pre>

Configure the router to forward internet traffic for the server port to the server LAN ip address.
Search the internet for info on how to configure the router. Typically you browse to 192.168.0.1
and login with a user and password. You need to find the configuration page for port forwarding and
change it so that traffic for your server port is forwarded to your server LAN ip address.

<a name="ix"><h1>iPhone/iPad/... clients</h1></a>

iX 7.0 Safari has adequate HTML 5 support to run JHS client.<br><br>

Relatively few changes were required for iX support.<br><br>

Resize calculations were required to handle the virtual keyboard coming and going.
There are still rough edges but it works well enough to be usefull.
Sometimes a resize event is not triggered and the screen won't be quite right.
An enter will usually fix this.
The iPhone sometimes 'squeezes' its top decorations (time, battery, address, etc) and that confuses
resize calculations. Touch time at top to unsqueeze and do enter to get propoer sizing.<br><br>

A touch stylus makes it much easier to work with these devices (especially the iPhone).<br><br>

A wireless keyboard works well and is highly recommended for other than casual use.<br><br>

JHS esc shortcuts are handy on iX. Virtual keyboard has no esc key and the esc key on the
wireless keyboard does not work. Virtual keyboard pressing s and sliding to ß 
is the esc for shortcuts. Wireless keyboard hold down option and press s for esc.<br><br>

Wireless arrow keys work on jijx but do not work on jijs (codemirror). This is a known codemirror
problem and hopefully will be fixed in a future codemirror release.

<a name="ide"><h1>IDE (Interactive Development Environment)</h1></a>
<span class="h">target</span><br>
New pages target _blank (new tab but may depend on browser).
Change TARGET_jhs_ to be _self (and refresh) to have pages open in place.

<br><br><span class="h">keyboard shortcuts</span><br>
esc key escapes next key to be a shortcut.<br/>
For example, esc j links to jijx page.<br/>
Menu items document shortcuts on the right.<br/>
esc 1 sets focus on menu.<br/>
esc 2 sets focus to page default.<br/>
<br/>
Control shortcuts are supported for ,./<>? as they
less likely conflict with the browser.<br/><br/>

jijx ctrl+. is lab advance.<br/><br/>

Example of a custom jijx handler:
<pre>   ev_comma_ctrl_jijx_=: 3 : 'i.5'</pre>

Other pages require J and Javascript handlers. For example,
<pre>
ev_comma_ctrl=: 3 : 'i.5'
function ev_comma_ctrl(){jdoajax([]);}
</pre>

<span class="h">jijx</span>
Run J sentences (ctrl+shift+&uarr;&darr; recall)

<br><br><span class="h">jijxm</span>
Simple browser support that requires only basic html.
jijx and related pages require HTML 5 features
(javascript, style sheets, contenteditable divs, ajax, ...).

<br><br><span class="h">jfile</span>
Browse files for editing, etc.
Adequate for simple IDE use and for a remote server.
For more complicated requirements use host facilities such
as Windows Explorer or Mac Finder.

<br><br><span class="h">jijs</span>
CodeMirror (www.codemirror.net) editor. See menu action|search/ctrl for ctrl/search/replace info.
<a href="#codemirror">CodeMirror Copyright</a>

<br><br><span class="h">jijsta</span>
edit file in textarea

<br><br><span class="h">jfif</span>
find in files

<br><br><span class="h">jal</span>
Addons package manager (pacman) downloads and installs
software packages

<br><br><span class="h">utils</span>
<pre class="jcode">
   jbd 1      NB. boxdraw +|-
   jfe_jhs_ y NB. toggle console/browser
   jhtml'&lt;font style="color:red;"&gt;A&lt;/font&gt;'

   t=. '~addons/docs/help/index.htm'jhref_jhs_'help'
   jhtml'&lt;div contenteditable="false"&gt;',t,'&lt;/div&gt;'

   open'~temp/f.ijs' NB. create jijs link to file
</pre>

<span class="h">jijx action menu</span>
A script defines action menu items and the verbs to run
when clicked. Shortcuts wqe are hardwired to the first
3 added menu items. The following is a sample file you can
define and then modify:
<pre class="jcode">
*** script ~user/projects/ja/ja.ijs ***
coclass'z'
ja_menu=: 0 : 0
aaa
bbb
<PAREN>

ja_aaa=: 3 : 0
'aaa clicked'
<PAREN>

ja_bbb=: 3 : 0
'bbb clicked'
<PAREN>
***
</pre>

<span class="h">jijx debug menu</span>
With debug on (jijx menu debug), execution suspends at an error or a stop.
<pre class="jcode">
   dbsm'name'      - display numbered definition lines
   dbsm'name ...'  - add stops
   dbsm'name :...' - add dyadic stops
   dbsm'~...'      - remove stops starting with ...
   dbsm''          - display stops
</pre>

Try the following:
<pre class="jcode">
   dbsm'calendar'   NB. numbered explicit defn
   dbsm'calendar 0' NB. stop monadic line 0
menu debug|on
   calendar 1
study stack display - note 6 blank prompt
   y
menu debug|step in
stepped into dyadic call of calendar
   x,y
menu debug|step - step to line 1
   a
menu debug|run - run to error or stop
(runs to end as no error or stops)
</pre>

<a name="plot"><h1>plot</h1>

Plots (graphics) can be done in several ways: viewmat, native, jd3, webgl, gcplot, and gnuplot
(as documented in the following sections).

<h3>viewmat</h3>
<pre class="jcode">
   load'viewmat'
   viewmat ?20 20$2
   viewmat */~ i:9
</pre>

<h3>native plot</h3>

Native plot creates an html file that has the data and javascript for drawing on an html canvas element.<br><br>

Use JAL to be sure required addons are installed:
<pre class="jcode">
 graphics/plot
 graphics/afm
 graphics/color
 demos/plot
 general/misc
 math/misc
 gui/gtk (required only for cairo/png output)
</pre>

Learn about plot with the following examples. Detailed info at the wiki.
<pre class="jcode">
   require 'plot numeric trig'
   plot 10?10
   load '~Demos/plot/plotdemos.ijs' NB. more than 50 demos
   plotdemos 0
   plotdef 'show';'plot';600 400    NB. type;window;size
   plotdemos 1
   plotdef 'show';'plot';600 300
   plotdemos 3                      NB. show in window named plot
   plotdemos 4                      NB. window reused
   plotdef 'link';'plot';600 450    NB. link to plot
   plotdemos 10
   plotdef 'jijx';'';400 100        NB. inline in jijx
   plotdemos 25
   plotdef 'none';'plot';600 300    NB. create ~temp/plot.html - not shown
   plotdemos 30
   'plot' jhsshow '~temp/plot.html' NB. show plot in window plot
   plotdef 'jijx';'';400 200
   plotdemos 54
</pre>

Plot has default output of canvas/html. It can also create cairo/png output.
<pre class="jcode">
   plotdef 'none';'plot';400 200;'cairo'
   plot 10?10                       NB. create ~temp/plot.png
   plotdef 'jijx';'plot';400 200;'cairo'
   plot 10?10                       NB. inline in jijx
   plotdef 'jijx';'plot';400 200;'canvas'
   plot 10?10                       NB. create ~temp/plot.html 
</pre>

<a name="jd3"><h3>jd3 - plots with D3/jquery <a target="_blank" href="http://www.d3js.org">www.d3js.org</a></h3>
<pre class="jcode">
   jd3'help'
</pre>
Currently only simple line, bar, and pie charts are supported.
Study ~addons/ide/jhs/jd3.ijs to see how it works and how easy it will be to extend.

<h3>WebGl</h3>

See studio|demos|demo|12 for an example of WebGl 3d graphics.


<h3>gcplot <a href="http://code.google.com/apis/chart/">Google Charts</a></h3>

<pre class="jcode">
   load'~addons/ide/jhs/jgcp.ijs'
   jgc'help'  NB. plot info
   jgcx''     NB. examples
</pre>

<h3>gnuplot <a href="http://www.gnuplot.info">www.gnuplot.info</a></h3>

Plots can be created with gnuplot and displayed in the browser.<br><br>

The following is out of date and needs changes in order to make use of the new gnuplot addon.<br><br>

After gnuplot is installed, try the following:
<pre class="jcode">
   load'~addons/ide/jhs/gnuplot.ijs'
   gpdemo''
</pre>

Windows gnuplot install:<br>
Follow <a href="http://www.gnuplot.info">www.gnuplot.info</a> download links, download gp444win32.zip, and unzip in your home folder.<br><br>

Linux gnuplot install:<br>
Modern linux distributions can install gnuplot with (or equivalent):
<pre class="jcode">
sudo apt-get install gnuplot
</pre>

Mac gnuplot install:<br>
You need to build libgd and gnuplot binaries and associated files from source.
Google <b>installing gnuplot on mac brainlog</b> 
for one of the better descriptions. 
The following is a simplified set of steps.<br><br>

Xcode developer tools are required to build from source. Install Xcode from the OSX distribution dvd or from the net.<br><br>

Build libgd first so that gnuplot build will include png output.<br><br>

libgd build:<br>
Download gd-2.0.35.tar.gz. From code.google.com search for the file and download the one dated Dec 27,2007.

<pre class="jcode">
cd ~/Downloads
tar -xf gd-2.0.35.tar 
cd ~/Downloads/gd/2.0.35
sudo ln -s /usr/X11R6/include/fontconfig /usr/local/include
ln -s `which glibtool` ./libtool
./configure --with-png=/usr/X11 --x-includes=/usr/X11/include --x-libraries=/usr/X11/lib
sudo make clean
sudo make
sudo make install
</pre>

gnuplot build:<br>
Download gnuplot-4.4.4.tar.gz. From www.gnuplot.info follow links to download from SourceForge.

<pre class="jcode">
cd ~/Downloads
tar -xf gnuplot-4.4.4.tar 
cd ~/Downloads/gnuplot-4.4.4
./configure
make clean
make
sudo make install
</pre>

<a name="gui"><h1>GUI</h1>

GUI applications are built with J, JHS framework, html,
javascript, and css.

See jijx demos for examples of GUI applications.<br><br>

The IDE is built with the same facilities. See
~addons/ide/jhs/jfile.ijs to see how the jfile page is
implemented.<br><br>

You can create simple applications with just info
from the demos. For more complicated applications you
need to learn about html, DOM (document object model),
javascript, and css (styles).<br><br>

There are great web resources on these topics.
You may prefer more structured
presentation and there are many books to choose from. None
stand out, but 'The Definitive Guide' series from O'Reilly
on HTML, Javascript, and CSS are adequate.<br><br>

There is a lot to learn to cover everything. Fortunately
the learning curve, though long, is not terribly steep
and there are significant rewards along the way.
Everything you learn is applicable not just to J,
but to every aspect of the incredible world of web programming.

<a name="jum"><h1>JUM (J User Manager)</h1>
JUM is a JHS application that runs on a web server
that provides JHS servers to J users. JUM users share
a single user account on the server and your files are
accessible to other users.<br><br>

To provide a JUM service you install J on a web server
and run the JHS JUM application. Users can access the JUM
web page and can manage their own JHS server.
The JUM sevice should be provided in a secure environment.
In a Linux server this is done with a jail account.
<br><br>
Study script ~addons/ide/jhs/jum.ijs to learn how to run your own JUM service.
A rough sketch of the steps are:
<pre class="jcode">
   start jconsole task
   load'~addons/ide/jhs/core.ijs'
   load'~addons/ide/jhs/jum.ijs'
   createjum_jum_ 65002;'1234' NB. only if new PORT or PASS
   init_jhs_'jum'
   browse to jum task /jijx and login 
   startjum_jum_ 65003 65004 65005;'localhost';'buzz'
   browse to jum task /jum and create new account
</pre>

<a name="console"><h1>console</h1>
The JHS jconsole window diplays useful information.<br><br>
It can be used (ctrl+c) to signal break to the J task and
it can kill the J task in the event of problems.<br><br> 

In windows you can edit the icon properties to run minimized.
You can hide the window if you wish:
<pre class="jcode">   jshowconsole_j_ 0 NB. hide/show 0/1</pre>

<a name="pop-up"><h1>pop-up</h1></a>
<p>Pop-up windows can be a plague when browsing ill-behaved sites.
However, they can be very useful in an app like JHS that
tries to combine the best of the browser interface with some
features of classic desktop applications.<p>

<p>JHS facilities like jd3 (plots) and jtable (spreadsheet) need
to create pop-ups.</p>

<p>If your browser is set to block pop-ups, then you will get
an alert message when JHS tries to create a pop-up. You can
adjust the browser settings to allow the JHS pop-ups.</p>

<p>You may not want to enable all pop-ups, and instead only
want to allow JHS pop-ups.
More recent versions of most browsers (Firefox/Chrome/Edge/IE)
allow you to configure pop-up blocking to allow pop-ups based
on the site address. In this case you want to enable pop-ups
from localhost:65001/jijx. Safari does not allow this kind of
configuration and you may want to simply allow all pop-ups.</p>






<a name="about"><h1>about</h1></a>
<pre class="jcode">
   JVERSION

<JVERSION>
</pre>
</div>
)

CSS=: 0 : 0
span.h{color:red;}
)

JS=: 0 : 0
function ev_body_load(){jresize();}
)
