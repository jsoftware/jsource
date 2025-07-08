prolog './g221.ijs'
NB. ~.y -----------------------------------------------------------------

(,b)-:~.b=.0
(,b)-:~.b=.'a'
(,b)-:~.b=.243
(,b)-:~.b=.2.71828
(,b)-:~.b=.3j4.54e2
(,b)-:~.b=.<i.12

b-:~.b=.1 0
b-:~.b=.1 3 4$a.
b-:~.b=.i.1 2 3 4
b-:~.b=.o.i.1 2 3
b-:~.b=.^0j1*i.1 0 3
b-:~.b=.1 3 2$;:'Cogito, ergo sum.'
b-:~.b=.1 3 2$(u:&.>) ;:'Cogito, ergo sum.'
b-:~.b=.1 3 2$(10&u:&.>) ;:'Cogito, ergo sum.'
b-:~.b=.1 3 2$s:@<"0&.> ;:'Cogito, ergo sum.'
b-:~.b=.1 3 2$<"0@s: ;:'Cogito, ergo sum.'

b-:~.b=.0$0
b-:~.b=.0 3 4$a.
b-:~.b=.i.0 2 3 4
b-:~.b=.o.i.0 2 3
b-:~.b=.^0j1*i.0 2 0 3
b-:~.b=.0 3 2$;:'Cogito, ergo sum.'
b-:~.b=.0 3 2$(u:&.>) ;:'Cogito, ergo sum.'
b-:~.b=.0 3 2$(10&u:&.>) ;:'Cogito, ergo sum.'
b-:~.b=.0 3 2$s:@<"0&.> ;:'Cogito, ergo sum.'
b-:~.b=.0 3 2$<"0@s: ;:'Cogito, ergo sum.'

(1{.b)-:~.b=.5#1
(1{.b)-:~.b=.5#1 3 4$a.
(1{.b)-:~.b=.5#i.1 2 3 4
(1{.b)-:~.b=.5#o.i.1 2 3
(1{.b)-:~.b=.5#^0j1*i.1 0 3
(1{.b)-:~.b=.5#1 3 2$;:'Cogito, ergo sum.'
(1{.b)-:~.b=.5#1 3 2$(u:&.>) ;:'Cogito, ergo sum.'
(1{.b)-:~.b=.5#1 3 2$(10&u:&.>) ;:'Cogito, ergo sum.'
(1{.b)-:~.b=.5#1 3 2$s:@<"0&.> ;:'Cogito, ergo sum.'
(1{.b)-:~.b=.5#1 3 2$<"0@s: ;:'Cogito, ergo sum.'

test=: 4 : 0
 yy=: y
 xx=: ~. yy
 assert. (~.y) -: (~. memu y)
 assert. x = (15!:19@~. = 15!:19) memu y
 assert. xx -:&(#@$ ) yy
 assert. xx -:&(}.@$) yy
 assert. xx <:&# yy
 assert. (#xx) > >./xx i. yy
 assert. (i.~xx) -: i.#xx
 1
)

IF64&test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: 0 1
IF64&test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: 'abcde'
IF64&test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: u:'abcde'
IF64&test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: 10&u:'abcde'
IF64&test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: s:@<"0 'abcde'
IF64&test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: ?5$20
IF64&test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: ?5$2e9
IF64&test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.5   [ t=: o.?5$20
IF64&test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.5   [ t=: j./?2 5$20
0&test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.5   [ t=: ;:' miasma chthonic chronic kakistocracy dado'
0&test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.5   [ t=: (u:&.>) ;:' miasma chthonic chronic kakistocracy dado'
0&test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.5   [ t=: (10&u:&.>) ;:' miasma chthonic chronic kakistocracy dado'
0&test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.5   [ t=: s:@<"0&.> ;:' miasma chthonic chronic kakistocracy dado'
0&test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.5   [ t=: <"0@s: ;:' miasma chthonic chronic kakistocracy dado'
0&test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.5   [ t=: x: ?5$20
0&test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.5   [ t=: %/0 1+x:?2 5$20
IF64&test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: u: ?5$65536
IF64&test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: 10&u: RAND32 ?5$C4MAX
IF64&test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: s:' miasma chthonic chronic kakistocracy dado'

NB. compatibility of ~. ~: /.
test =: 4 : 0
if. -. IF64 do. 1 return. end.  NB. The 32-bit code has the old bug, plus a crash on u/. with lots of tolerant equals
assert. (~.y) -: (~. memu y)
assert. x = (15!:19@~. = 15!:19) memu y
yy =: y
assert. (+/ #/.~ y) = #y
assert. (#/.~ y) =&# ~.y
assert. (+/ ~: y) = # ~. y
assert. (#/.~ /:~ y) -: 2 -~/\ I. 1 ,~ ~: /:~ y
assert. (~. y) -:!.0 (~: y) # y
1
)
1&test 1+2e_14*i:3
0&test <"0 ] 1+2e_14*i:3
1&test 1+2e_14*i:20
0&test <"0 ] 1+2e_14*i:20
1&test 1+2e_14*(_12 + i. 5 5)
0&test <"0 ] 1+2e_14*(_12 + i. 5 5)
0&test <"_1 ] a. {~ 20 ?@$ #a.
0&test <"_1 ] a. {~ 20 20 ?@$ #a.
1&test 2 j.~ 1+2e_14*i:20
0&test <"0 ] 2 j.~ 1+2e_14*i:20
1&test 2 j. 1+2e_14*i:20
0&test <"0 ] 2 j. 1+2e_14*i:20

test =: {{
for. i. x do.
 val =: y {~ ((0. 0.5 I. ?0) ?@$ 50) ?@$ #y
 assert. (~.val) -: (~. memu val)
 assert. IF64 = (15!:19@~. = 15!:19) memu val
end.
1
}}"0 1
1000 test 0 1
1000 test i. 10
1000 test i. 400
1000 test o. i. 400
1000 test 100 * i. 400
1000 test j.~ i. 400

test =: {{
for. i. x do.
 val =: y {~ ((0. 0.5 I. ?0) ?@$ 50) ?@$ #y
 assert. (~.val) -: (~. memu val)
 assert. (15!:19@~. ~: 15!:19) memu val
end.
1
}}"0 1
100 test <"0 i. 100  NB. NOT inplace
100 test i. 100x
100 test 4 %~ i. 100x





epilog''

