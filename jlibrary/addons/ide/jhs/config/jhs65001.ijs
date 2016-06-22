NB. load JHS scripts, configure, and init server
NB. command prompt> ....jconsole ~addons/ide/jhs/config/jhs65001.ijs

NB. create other configs by copying this file to ~config, renaming, and editing
NB. e.g., copy to ~config/jhs65002john.ijs and edit port/user/pass to be 65002/'john'/'abra'
NB. command prompt> ....jconsole ~config/jhs65002john.ijs

NB. see jhelp|config for info on port access, firewall, ip address, router, etc.

load'~addons/ide/jhs/core.ijs'
PORT=: 65001       NB. private port range 49152 to 65535
LHOK=: 1           NB. 0 if localhost requires user/pass login
BIND=: 'localhost' NB. 'any'  - access from any machine
USER=: ''          NB. 'john' - login
PASS=: ''          NB. 'abra' - login
TIPX=: ''          NB. browser tab title prefix - distinguish sessions
init_jhs_''
