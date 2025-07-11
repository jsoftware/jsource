prolog './gjfile.ijs'
NB. jfiles --------------------------------------------------------------

require 'jfiles'

NB. test failed on small ct
ct   =: 9!:18''
9!:19[1e_12

NB. 64-bit 3!:3 (2r3^48)
(2r3^48) -: 3!:2>cutLF{{)n
e300000000000000
8000000000000000
0100000000000000
0000000000000000
3000000000000000
6000000000000000
e300000000000000
0200000000000000
0800000000000000
0100000000000000
0100000000000000
0000000000000100
e300000000000000
0200000000000000
1000000000000000
0100000000000000
0200000000000000
c135bcfaaf6dc525
e410000000000000
}}

NB. 32-bit 3!:3 (2r3^48)
(2r3^48) -: 3!:2>cutLF{{)n
e1000000
80000000
01000000
00000000
18000000
34000000
e1000000
02000000
08000000
01000000
02000000
00000000
00000100
e1000000
02000000
0c000000
01000000
03000000
c135bcfa
af6dc525
e4100000
}}

j32=: 3!:11 LF-.~ (0 : 0) 
4QAAACAAAAAHAAAAAQAAAAcAAAAwAAAARAAAAFgAAABsAAAAgAAAAJwAAAC4AAAA
4QAAAAQAAAABAAAAAAAAADMwambhAAAAAQAAAAEAAAAAAAAAAAAAAOEAAAAEAAAA
AQAAAAAAAAAOAAAA4QAAAAQAAAABAAAAAAAAAEAMAADhAAAABAAAAAIAAAABAAAA
AgAAAAAEAAAAAgAA4QAAAAQAAAACAAAAAQAAAAIAAAAABgAAAAEAAOEAAAAEAAAA
AQAAAAAAAAABAAAAAAAAAOEAAAABAAAAAQAAAAAAAAAAAAAAICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgIAAHAAAgAAAAIAcAACAAAABABwAAQAAAAIAHAAAgAAAA
oAcAACAAAADABwAAgAAAAEAIAACAAAAAwAgAAIAAAABACQAAgAAAAMAJAACAAAAA
QAoAAIAAAADACgAAgAAAAEALAACAAAAAwAsAAIAAAAAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
4QAAAAQAAAAAAAAAAgAAAAAAAAACAAAAICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgIOEAAAACAAAABAAAAAEAAAAEAAAAemVybwAAAAAgICAg
4QAAAAAAAgADAAAAAQAAAAMAAABvAG4AZQAAACAgICDhAAAAAAAEAAMAAAABAAAA
AwAAAHQAAAB3AAAAbwAAAAAAAAAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
4QAAAAEAAAABAAAAAAAAAAAAAAAgICAgICAgICAgICDhAAAABAAAAAEAAAAAAAAA
AQAAACAgICAgICAgICAgIOEAAAAEAAAADAAAAAIAAAADAAAABAAAAAAAAAABAAAA
AgAAAAMAAAAEAAAABQAAAAYAAAAHAAAACAAAAAkAAAAKAAAACwAAACAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
4QAAAAgAAAAJAAAAAgAAAAMAAAADAAAAzmA/CKq2/78fqK/FWO4bQM2HES8DHhLA
OofY0IKvEUB4eOzVvnw6wNhhBQq2ujJAzbMpeLZaA8BNYgUKtroyQNd37NW+fCrA
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICDhAAAAQAAAAAIAAAABAAAA
AgAAABwAAAA0AAAA4QAAAAIAAAAEAAAAAQAAAAEAAAABAAAA4QAAAAIAAAAEAAAA
AQAAAAEAAAACAAAAICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgIOEAAACAAAAAAQAAAAAAAAAYAAAAMAAAAOEAAAACAAAA
BAAAAAEAAAABAAAAAwAAAOEAAAACAAAABAAAAAEAAAABAAAABQAAACAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
4QAAAIAAAAACAAAAAQAAAAIAAAAkAAAAPAAAAFAAAABoAAAA4QAAAAIAAAAEAAAA
AQAAAAEAAAABAAAA4QAAAAIAAAAAAAAAAQAAAAAAAADhAAAAAgAAAAQAAAABAAAA
/////wEAAADhAAAAAgAAAAAAAAABAAAAAAAAACAgICDhAAAAgAAAAAEAAAAAAAAA
GAAAADAAAADhAAAAAgAAAAQAAAABAAAAAQAAAAEAAADhAAAAAgAAAAAAAAABAAAA
AAAAACAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgIOEAAACAAAAAAQAAAAAAAAAYAAAAMAAAAOEAAAACAAAA
BAAAAAEAAAABAAAAAQAAAOEAAAACAAAAAAAAAAEAAAAAAAAAICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
4QAAAIAAAAABAAAAAAAAABgAAAAsAAAA4QAAAAIAAAAAAAAAAQAAAAAAAADhAAAA
AgAAAAQAAAABAAAAAQAAAAEAAAAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICDhAAAAIAAAAAIAAAABAAAA
AgAAABwAAABMAAAA4QAAAAQAAAAGAAAAAgAAAAIAAAADAAAAAAAAAAEAAAACAAAA
AwAAAAQAAAAFAAAA4QAAAAIAAAAEAAAAAQAAAAQAAABhYmNkAAAAACAgICAgICAg
ICAgICAgICAgICAgICAgIA==
)

j64=: 3!:11 LF-.~ (0 : 0) 
4wAAAAAAAAAgAAAAAAAAAAcAAAAAAAAAAQAAAAAAAAAHAAAAAAAAAGAAAAAAAAAA
iAAAAAAAAACwAAAAAAAAANgAAAAAAAAAAAEAAAAAAAA4AQAAAAAAAHABAAAAAAAA
4wAAAAAAAAAEAAAAAAAAAAEAAAAAAAAAAAAAAAAAAAA1MGpmAAAAAOMAAAAAAAAA
AQAAAAAAAAABAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADjAAAAAAAAAAQAAAAAAAAA
AQAAAAAAAAAAAAAAAAAAAA4AAAAAAAAA4wAAAAAAAAAEAAAAAAAAAAEAAAAAAAAA
AAAAAAAAAADAEAAAAAAAAOMAAAAAAAAABAAAAAAAAAACAAAAAAAAAAEAAAAAAAAA
AgAAAAAAAAAABAAAAAAAAAACAAAAAAAA4wAAAAAAAAAEAAAAAAAAAAIAAAAAAAAA
AQAAAAAAAAACAAAAAAAAAAAGAAAAAAAAAAEAAAAAAADjAAAAAAAAAAQAAAAAAAAA
AQAAAAAAAAAAAAAAAAAAAAEAAAAAAAAAAAAAAAAAAADjAAAAAAAAAAEAAAAAAAAA
AQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgIAAAAAAAAAcAAAAAAAAAAEAAAAAAAAAHQAAAAAAAAABA
AAAAAAAAB4AAAAAAAAAAQAAAAAAAAAfAAAAAAAAAAEAAAAAAAAAIAAAAAAAAAABA
AAAAAAAACEAAAAAAAAABAAAAAAAAAAlAAAAAAAAAAIAAAAAAAAAJwAAAAAAAAAEA
AAAAAAAACsAAAAAAAAABAAAAAAAAAAvAAAAAAAAAAQAAAAAAAAAMwAAAAAAAAAEA
AAAAAAAADcAAAAAAAAABAAAAAAAAAA7AAAAAAAAAAQAAAAAAAAAPwAAAAAAAAAEA
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
4wAAAAAAAAAEAAAAAAAAAAAAAAAAAAAAAgAAAAAAAAAAAAAAAAAAAAIAAAAAAAAA
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgIOMAAAAAAAAAAgAAAAAAAAAEAAAAAAAAAAEAAAAAAAAA
BAAAAAAAAAB6ZXJvAAAAACAgICAgICAgICAgICAgICDjAAAAAAAAAAAAAgAAAAAA
AwAAAAAAAAABAAAAAAAAAAMAAAAAAAAAbwBuAGUAAAAgICAgICAgICAgICAgICAg
4wAAAAAAAAAAAAQAAAAAAAMAAAAAAAAAAQAAAAAAAAADAAAAAAAAAHQAAAB3AAAA
bwAAAAAAAAAgICAgICAgIOMAAAAAAAAAAQAAAAAAAAABAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAgICAgICAgICAgICAgICAgICAgICAgICDjAAAAAAAAAAQAAAAAAAAA
AQAAAAAAAAAAAAAAAAAAAAEAAAAAAAAAICAgICAgICAgICAgICAgICAgICAgICAg
4wAAAAAAAAAEAAAAAAAAAAwAAAAAAAAAAgAAAAAAAAADAAAAAAAAAAQAAAAAAAAA
AAAAAAAAAAABAAAAAAAAAAIAAAAAAAAAAwAAAAAAAAAEAAAAAAAAAAUAAAAAAAAA
BgAAAAAAAAAHAAAAAAAAAAgAAAAAAAAACQAAAAAAAAAKAAAAAAAAAAsAAAAAAAAA
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgIOMAAAAAAAAACAAAAAAAAAAJAAAAAAAAAAIAAAAAAAAA
AwAAAAAAAAADAAAAAAAAAD9fPwiqtv+/yaSvxVjuG0BxhREvAx4SwLiF2NCCrxFA
P3Xs1b58OsCRXwUKtroyQK2xKXi2WgPABmAFCra6MkCgdOzVvnwqwCAgICAgICAg
4wAAAAAAAABAAAAAAAAAAAIAAAAAAAAAAQAAAAAAAAACAAAAAAAAADgAAAAAAAAA
aAAAAAAAAADjAAAAAAAAAAIAAAAAAAAACAAAAAAAAAABAAAAAAAAAAEAAAAAAAAA
AQAAAAAAAADjAAAAAAAAAAIAAAAAAAAACAAAAAAAAAABAAAAAAAAAAEAAAAAAAAA
AgAAAAAAAAAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgIOMAAAAAAAAAgAAAAAAAAAABAAAAAAAAAAAAAAAAAAAA
MAAAAAAAAABgAAAAAAAAAOMAAAAAAAAAAgAAAAAAAAAIAAAAAAAAAAEAAAAAAAAA
AQAAAAAAAAADAAAAAAAAAOMAAAAAAAAAAgAAAAAAAAAIAAAAAAAAAAEAAAAAAAAA
AQAAAAAAAAAFAAAAAAAAACAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICDjAAAAAAAAAIAAAAAAAAAA
AgAAAAAAAAABAAAAAAAAAAIAAAAAAAAASAAAAAAAAAB4AAAAAAAAAKAAAAAAAAAA
0AAAAAAAAADjAAAAAAAAAAIAAAAAAAAACAAAAAAAAAABAAAAAAAAAAEAAAAAAAAA
AQAAAAAAAADjAAAAAAAAAAIAAAAAAAAAAAAAAAAAAAABAAAAAAAAAAAAAAAAAAAA
4wAAAAAAAAACAAAAAAAAAAgAAAAAAAAAAQAAAAAAAAD//////////wEAAAAAAAAA
4wAAAAAAAAACAAAAAAAAAAAAAAAAAAAAAQAAAAAAAAAAAAAAAAAAACAgICAgICAg
4wAAAAAAAACAAAAAAAAAAAEAAAAAAAAAAAAAAAAAAAAwAAAAAAAAAGAAAAAAAAAA
4wAAAAAAAAACAAAAAAAAAAgAAAAAAAAAAQAAAAAAAAABAAAAAAAAAAEAAAAAAAAA
4wAAAAAAAAACAAAAAAAAAAAAAAAAAAAAAQAAAAAAAAAAAAAAAAAAACAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgIOMAAAAAAAAAgAAAAAAAAAABAAAAAAAAAAAAAAAAAAAA
MAAAAAAAAABgAAAAAAAAAOMAAAAAAAAAAgAAAAAAAAAIAAAAAAAAAAEAAAAAAAAA
AQAAAAAAAAABAAAAAAAAAOMAAAAAAAAAAgAAAAAAAAAAAAAAAAAAAAEAAAAAAAAA
AAAAAAAAAAAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICDjAAAAAAAAAIAAAAAAAAAA
AQAAAAAAAAAAAAAAAAAAADAAAAAAAAAAWAAAAAAAAADjAAAAAAAAAAIAAAAAAAAA
AAAAAAAAAAABAAAAAAAAAAAAAAAAAAAA4wAAAAAAAAACAAAAAAAAAAgAAAAAAAAA
AQAAAAAAAAABAAAAAAAAAAEAAAAAAAAAICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
4wAAAAAAAAAgAAAAAAAAAAIAAAAAAAAAAQAAAAAAAAACAAAAAAAAADgAAAAAAAAA
mAAAAAAAAADjAAAAAAAAAAQAAAAAAAAABgAAAAAAAAACAAAAAAAAAAIAAAAAAAAA
AwAAAAAAAAAAAAAAAAAAAAEAAAAAAAAAAgAAAAAAAAADAAAAAAAAAAQAAAAAAAAA
BQAAAAAAAADjAAAAAAAAAAIAAAAAAAAABAAAAAAAAAABAAAAAAAAAAQAAAAAAAAA
YWJjZAAAAAAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAg
ICAgICAgICAgICAgICAgIA==
) 

s=: ('zero';(7&u:'one');(10&u:'two');0;01;(i.3 4);(%.%:i.3 3);(1 2x);(3r5);(x:_ __);(1r0);(_r0);(0r0);(<(i.2 3);'abcd'))
NB. echo _64]\ 5!:5 <'s'
s -: ". 5!:5 <'s'
F=: jpath '~temp/t1-','.ijf',~(":2!:6''),'_',":3&T.''

_1 ~: F fwrite~ j32
NB. echo s ,: jread"1 (<F),.<"0[i.#s
s = jread"1 (<F),.<"0[i.#s

_1 ~: F fwrite~ j64
NB. echo s ,: jread"1 (<F),.<"0[i.#s
s = jread"1 (<F),.<"0[i.#s

ferase ::1: F

9!:19 ct


epilog''
