NB. JHS initialization
NB.  loads
NB.   ~addons/ide/jhs/config/jhs_default.ijs
NB.  then loads first file (if any) that exists from
NB.   init x arg (error if not '' and does not exist)
NB.   ~config/jhs.ijs
NB.   ~addons/ide/jhs/config/jhs.ijs

NB. config assumed complete if PORT defined when init called

NB.  if init x arg starts with PORT=: then ". instead of load
NB.  start server on port 65011 without editing files with:
NB.  ...jconsole ~addons/ide/jhs/core.ijs -js " 'PORT=:65011'init_jhs_'' "

NB. jhs_default.ijs is a default/template (do not edit)
NB. copy 2nd and 3rd from default and edit as required

NB. custom configs (2nd/3rd files) need not be a complete
NB. and can just change specific things

NB. private port range 49152 to 65535
PORT=: 65001

NB. 0 localhost jlogin if PASS set
NB. 1 localhost ok (no jlogin even if PASS set)
LHOK=: 1

NB. 'localhost' access from same machine
NB. 'any' access from any machine (should have PASS set)
BIND=: 'localhost'

NB. ''    no jlogin
NB. '...' jlogin password
PASS=: ''

NB. user for PASS
NB. JUM ignores and sets USER to be JUM username (jhs folder)
USER=: ''

NB. browser tab title prefix - distinguish sessions
TIPX=: ''