NB. test data (not self-describing) files

dbg 1
dbstops''
unmapall_jmf_''
G=: jpath '~temp/t2.dat'

(a.{~,4 {.&> i.20) fwrite G
JINT map_jmf_ 'rst';G
smoutput rst
