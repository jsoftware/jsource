prolog './g520b.ijs'
NB. boolean { y ---------------------------------------------------------

f0=: 3 : 0
 x=. a.{~ (2,y) ?@$ #a.
 assert. (b{x) -: i{x
 1
)

f0"0 i.15 [ i=: (2-2)+b=: ?1000$2
f0"0 i.15 [ i=: (2-2)+b=: ?1001$2
f0"0 i.15 [ i=: (2-2)+b=: ?1002$2
f0"0 i.15 [ i=: (2-2)+b=: ?1003$2

f0"0 i.15 [ i=: (2-2)+b=: ?2$~2 3 
f0"0 i.15 [ i=: (2-2)+b=: ?2$~2 3 5 7
f0"0 i.15 [ i=: (2-2)+b=: ?2$~2 3 5 7 11

f0"0 i.15 [ i=: (2-2)+b=: 1000$0
f0"0 i.15 [ i=: (2-2)+b=: 1001$0
f0"0 i.15 [ i=: (2-2)+b=: 1002$0
f0"0 i.15 [ i=: (2-2)+b=: 1003$0

x=: 'a',:'012345',240 255{a.
y=: |."1 x
(b{x) -: (b+2-2){x [ b=: 1000 ?@$ 2
(b{y) -: (b+2-2){y
(b{x) -: (b+2-2){x [ b=: 1001 ?@$ 2
(b{y) -: (b+2-2){y
(b{x) -: (b+2-2){x [ b=: 1002 ?@$ 2
(b{y) -: (b+2-2){y
(b{x) -: (b+2-2){x [ b=: 1003 ?@$ 2
(b{y) -: (b+2-2){y

x=: 'a',:'012345',240 127{a.
y=: |."1 x
(b{x) -: (b+2-2){x [ b=: 1000 ?@$ 2
(b{y) -: (b+2-2){y
(b{x) -: (b+2-2){x [ b=: 1001 ?@$ 2
(b{y) -: (b+2-2){y
(b{x) -: (b+2-2){x [ b=: 1002 ?@$ 2
(b{y) -: (b+2-2){y
(b{x) -: (b+2-2){x [ b=: 1003 ?@$ 2
(b{y) -: (b+2-2){y

f1=: 3 : 0
 x=. a.{~ (3 2,y) ?@$ #a.
 assert. (b{"_ _1 x) -: i{"_ _1 x 
 1
)

f1"0 i.15 [ i=: (2-2)+b=: ?1000$2
f1"0 i.15 [ i=: (2-2)+b=: ?1001$2
f1"0 i.15 [ i=: (2-2)+b=: ?1002$2
f1"0 i.15 [ i=: (2-2)+b=: ?1003$2

f1"0 i.15 [ i=: (2-2)+b=: ?2$~2 3 
f1"0 i.15 [ i=: (2-2)+b=: ?2$~2 3 5 7
f1"0 i.15 [ i=: (2-2)+b=: ?2$~2 3 5 7 11

f1"0 i.15 [ i=: (2-2)+b=: 1000$0
f1"0 i.15 [ i=: (2-2)+b=: 1001$0
f1"0 i.15 [ i=: (2-2)+b=: 1002$0
f1"0 i.15 [ i=: (2-2)+b=: 1003$0

f2=: 3 : 0
 i=: (2-2)+b=: y ?@$ 2
 assert. (b{x) -: i{x=:     ?  2$2
 assert. (b{x) -: i{x=: a.{~?  2$#a.
 assert. (b{x) -: i{x=:     ?  2$2e9
 assert. (b{x) -: i{x=: o.  ?  2$2e9
 assert. (b{x) -: i{x=: r.  ?  2$2e5
 assert. (b{x) -: i{x=: x:  ?  2$2e9
 assert. (b{x) -: i{x=: %/x:?2 2$2e9
 assert. (b{x) -: i{x=: x{~ ?  2$#x=: ;:'Cogito, ergo sum. kakistocracy'
 assert. (b{x) -: i{x=: x{~ ?  2$#x=: (u:&.>) ;:'Cogito, ergo sum. kakistocracy'
 assert. (b{x) -: i{x=: x{~ ?  2$#x=: (10&u:&.>) ;:'Cogito, ergo sum. kakistocracy'
 assert. (b{x) -: i{x=: x{~ ?  2$#x=: s:@<"0&.> ;:'Cogito, ergo sum. kakistocracy'
 assert. (b{x) -: i{x=: x{~ ?  2$#x=: <"0@s: ;:'Cogito, ergo sum. kakistocracy'
 1
)

f2"0 ] 1000+i.10

(b{"1 x) -: i{"1 x=:     ?  5 2$2   [ i=: (2-2)+b=: ?5 120$2
(b{"1 x) -: i{"1 x=: a.{~?  5 2$#a.
(b{"1 x) -: i{"1 x=:     ?  5 2$2e9
(b{"1 x) -: i{"1 x=: o.  ?  5 2$2e9
(b{"1 x) -: i{"1 x=: r.  ?  5 2$2e5
(b{"1 x) -: i{"1 x=: x:  ?  5 2$2e9
(b{"1 x) -: i{"1 x=: %/x:?2 5 2$2e9
(b{"1 x) -: i{"1 x=: x{~?5 2$#x=: ;:'Cogito, ergo sum. kakistocracy'
(b{"1 x) -: i{"1 x=: x{~?5 2$#x=: (u:&.>) ;:'Cogito, ergo sum. kakistocracy'
(b{"1 x) -: i{"1 x=: x{~?5 2$#x=: (10&u:&.>) ;:'Cogito, ergo sum. kakistocracy'
(b{"1 x) -: i{"1 x=: x{~?5 2$#x=: ;:'Cogito, ergo sum. kakistocracy'
(b{"1 x) -: i{"1 x=: x{~?5 2$#x=: ;:'Cogito, ergo sum. kakistocracy'


'index error' -: (10$1 0) { etx i.1 5
'index error' -: (10$0  ) { etx i.0 5





epilog''

