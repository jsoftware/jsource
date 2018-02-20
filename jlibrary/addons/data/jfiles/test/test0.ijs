NB. test jfiles

F=: jpath '~temp/t1.ijf'
jcreate F
('zero';'one';'two';i.3 4) jappend F
smoutput jsize F
jdup F
smoutput jsize F
