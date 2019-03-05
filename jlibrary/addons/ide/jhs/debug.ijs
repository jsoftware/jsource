NB. JHS - debug
coclass'jhs'

tool_debug=: 0 : 0
debug facilities:
 suspend execution at stop or error
 examine/modify locals - step/stepin/stepout/etc
 
 ctrl+'       - dbstep   - run and stop at next
 crtl+"       - dbstepin - run and stop at next (may be in called name)
 
 dbstepout''  - run and stop at next in caller
 dbjump n     - run n and stop at next
 dbrun''      - run and continue
 dbnxt''      - run next line and continue
 dbcutback''  - cut back 1 stack level and stop
 dbes''       - formatted stack
 dbstopedit'' - create temp script with stops
 dbstopset''  - set stops from temp script
 
practice before you need it for real!
   dbsd'calendar'    NB. show definition with line numbers 
 click monadic line 0 to set stop   
   dbr 1             NB. debug enabled
   calendar 1        NB. suspend at line 0 - note 6 space prompt
   ctrl+"            NB. step into call of dyadic calendar
   ctrl+'            NB. step to next line
   a                 NB. local value a
   dbrun''           NB. run to end as no stops/error
   dbr 0             NB. debug disabled
)

dbstoptxt=: '~temp/dbstop.txt'

dbcutback_z_=: 13!:19
dbstep_z_=:    13!:20
dbstepin_z_=:  13!:21
dbstepout_z_=: 13!:22
dbjump__=: '' dbstep~ ]

dbsd_z_=: 3 : 0
'jdebug;0 0'cojhs y
i.0 0
)

dbes_z_=: 3 : 0
'locale: _',(>coname''),'_',LF,;LF,~each dtb each<"1[2}.13!:18''
)

dbstopedit_z_=: 3 : 0
t=. 13!:2''
t=. ;deb each <;.2 t,';'#~';'~:{:t
(t rplc ';';LF) fwrite dbstoptxt_jhs_
edit dbstoptxt_jhs_
)

dbstopset_z_=: 3 : 0
t=. LF,~fread dbstoptxt_jhs_
t=. dltb each<;._2 t
t=. t-.<''
t=. t-.<,';'
t=. ;LF,~each t
13!:3 deb t rplc LF;' ; ';':';' : '
i.0 0
)
