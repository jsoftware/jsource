1:@:(dbr bind Debug)@:(9!:19)2^_44[(echo^:ECHOFILENAME) './g520.ijs'
NB. {y ------------------------------------------------------------------

randuni''

NB. Boolean
a =: 1=?(1+?10)$2
b =: 1=?(1+?10)$2
({a;b) -: a,&.>/b

NB. literal
a =: a.{~97+?(1+?10)$26
b =: a.{~97+?(1+?10)$26
({a;b) -: a,&.>/b

NB. literal2
a =: adot1{~97+?(1+?10)$26
b =: adot1{~97+?(1+?10)$26
({a;b) -: a,&.>/b

NB. literal4
a =: adot2{~97+?(1+?10)$26
b =: adot2{~97+?(1+?10)$26
({a;b) -: a,&.>/b

NB. symbol
a =: sdot0{~97+?(1+?10)$26
b =: sdot0{~97+?(1+?10)$26
({a;b) -: a,&.>/b

NB. integer
a =: _40+?(1+?10)$100
b =: _40+?(1+?10)$100
({a;b) -: a,&.>/b
(i.v) -: v#.>{i.&.>v =: >: ?7 7 7 7
(i.v) -: v#.>{i.&.>v =: >: ?7 7 7 7
(i.v) -: v#.>{i.&.>v =: >: ?7 7 7 7

NB. floating-point
a =: 0.1*_40+?(1+?10)$100
b =: 0.1*_40+?(1+?10)$100
({a;b) -: a,&.>/b

NB. complex
a =: ^0j0.1*_40+?(1+?10)$100
b =: ^0j0.1*_40+?(1+?10)$100
({a;b) -: a,&.>/b
b =: _40+?(1+?10)$100
({a;b) -: a,&.>/b

NB. boxed
a =: (1,}.0=?(#a)$4)<;.(1) a=:_40+?(1+?20)$100
b =: (1,}.0=?(#b)$4)<;.(1) b=:0.1*_40+?(1+?20)$100
({(<a),<b) -: a<@,"0/b

count   =: */@$@>
prod    =: */\.@}.@(,&1)
copy    =: */@[ $&> prod@[ (#,)&.> ]
catalog =: ;@:($&.>) $ count <"1@|:@copy ]

f =: { -: catalog

f 0 1;1=?2 3$6
f (3 4$'foobar');'lieben'
f (3 4$u:'foobar');u:'lieben'
f (3 4$10&u:'foobar');10&u:'lieben'
f (3 4$s:@<"0 'foobar');s:@<"0 'lieben'
f (?5$105);?2 3$10
f o.&.>(?5$105);2 3$10
f 3j4;5j6 7 8 9
f 0 1;2;3.4;5j6 7

'domain error' -: { etx 1 2 3; 'ab'
'domain error' -: { etx 1 2 3;~'ab'
'domain error' -: { etx 1 2 3; u:'ab'
'domain error' -: { etx 1 2 3;~u:'ab'
'domain error' -: { etx 1 2 3; 10&u:'ab'
'domain error' -: { etx 1 2 3;~10&u:'ab'
'domain error' -: { etx 1 2 3; s:@<"0 'ab'
'domain error' -: { etx 1 2 3;~s:@<"0 'ab'
'domain error' -: { etx 1 2 3; <<4 5
'domain error' -: { etx 1 2 3;~<<4 5
'domain error' -: { etx 'abc'; <<4 5
'domain error' -: { etx 'abc';~<<4 5


NB. x{y -----------------------------------------------------------------

a -: 0{a=:?2
a -: 0{a=:(?#a.){a.
a -: 0{a=:(?#adot1){adot1
a -: 0{a=:(?#adot2){adot2
a -: 0{a=:(?#sdot0){sdot0
a -: 0{a=:?1e9
a -: 0{a=:o.?1e9
a -: 0{a=:r.?1e7
a -: 0{a=:<?1e9

NB. Boolean
a=:1=?2 3 4$2
(|.a)      -: 1 0{a
(2{."1 a)  -: 0 1{"1 a
(_2{."2 a) -: _2 _1{"2 a
(($0)$,_1 _1 _1{.a) -: (<_1 _1 _1){a

p=:?(?(?4)$5)$2
q=:?(?(?4)$5)$3
r=:?(?(?4)$5)$4
i=:p;q;r
(;$&.>i)      -: $(<i){a
(($p),3 4)    -: $(<<p){a
(($p),($q),4) -: $(<p;q){a

NB. literal
a=:a.{~?2 3 4$256
(|.a)      -: 1 0{a
(2{."1 a)  -: 0 1{"1 a
(_2{."2 a) -: _2 _1{"2 a
(($0)$,_1 _1 _1{.a) -: (<_1 _1 _1){a

NB. literal2
a=:adot1{~?2 3 4$(#adot1)
(|.a)      -: 1 0{a
(2{."1 a)  -: 0 1{"1 a
(_2{."2 a) -: _2 _1{"2 a
(($0)$,_1 _1 _1{.a) -: (<_1 _1 _1){a

NB. literal4
a=:adot2{~?2 3 4$(#adot2)
(|.a)      -: 1 0{a
(2{."1 a)  -: 0 1{"1 a
(_2{."2 a) -: _2 _1{"2 a
(($0)$,_1 _1 _1{.a) -: (<_1 _1 _1){a

NB. symbol
a=:sdot0{~?2 3 4$(#sdot0)
(|.a)      -: 1 0{a
(2{."1 a)  -: 0 1{"1 a
(_2{."2 a) -: _2 _1{"2 a
(($0)$,_1 _1 _1{.a) -: (<_1 _1 _1){a

p=:?(?(?4)$5)$2
q=:?(?(?4)$5)$3
r=:?(?(?4)$5)$4
i=:p;q;r
(;$&.>i)      -: $(<i){a
(($p),3 4)    -: $(<<p){a
(($p),($q),4) -: $(<p;q){a

NB. integer
a=:?2 3 4$10000
(|.a)      -: 1 0{a
(2{."1 a)  -: 0 1{"1 a
(_2{."2 a) -: _2 _1{"2 a
(($0)$,_1 _1 _1{.a) -: (<_1 _1 _1){a

p=:?(?(?4)$5)$2
q=:?(?(?4)$5)$3
r=:?(?(?4)$5)$4
i=:p;q;r
(;$&.>i)      -: $(<i){a
(($p),3 4)    -: $(<<p){a
(($p),($q),4) -: $(<p;q){a

NB. floating point
a=:o.?2 3 4$10000
(|.a)      -: 1 0{a
(2{."1 a)  -: 0 1{"1 a
(_2{."2 a) -: _2 _1{"2 a
(($0)$,_1 _1 _1{.a) -: (<_1 _1 _1){a

p=:?(?(?4)$5)$2
q=:?(?(?4)$5)$3
r=:?(?(?4)$5)$4
i=:p;q;r
(;$&.>i)      -: $(<i){a
(($p),3 4)    -: $(<<p){a
(($p),($q),4) -: $(<p;q){a

NB. complex
a=:^0j1*?2 3 4$200
(|.a)      -: 1 0{a
(2{."1 a)  -: 0 1{"1 a
(_2{."2 a) -: _2 _1{"2 a
(($0)$,_1 _1 _1{.a) -: (<_1 _1 _1){a

p=:?(?(?4)$5)$2
q=:?(?(?4)$5)$3
r=:?(?(?4)$5)$4
i=:p;q;r
(;$&.>i)      -: $(<i){a
(($p),3 4)    -: $(<<p){a
(($p),($q),4) -: $(<p;q){a

NB. boxed
a=:2 3 4$;:'(($p),($q),4)-:$(<p;q){a'
(|.a)      -: 1 0{a
(2{."1 a)  -: 0 1{"1 a
(_2{."2 a) -: _2 _1{"2 a
(($0)$,_1 _1 _1{.a) -: (<_1 _1 _1){a

p=:?(?(?4)$5)$2
q=:?(?(?4)$5)$3
r=:?(?(?4)$5)$4
i=:p;q;r
(;$&.>i)      -: $(<i){a
(($p),3 4)    -: $(<<p){a
(($p),($q),4) -: $(<p;q){a

(i.0 3 0 59) -: (i.0 3){i.(_1+2^31),0 59
(i.0   )     -: (<_1+1e9 2e9){i.1e9 2e9 0
(i.0 59)     -: (<_1+1e9 2e9){i.1e9 2e9 0 59
(i. 1e9 1e9 1e9 4 0 4) -: (0 1 2 1) {"3 i. 1e9 1e9 1e9 3 0 4

(i.2 0 3 4 5 6) -: (i.2 0 3){0 4 5 6$'abc'
(i.2 0 3 4 5 6) -: (i.2 0 3){0 4 5 6$u:'abc'
(i.2 0 3 4 5 6) -: (i.2 0 3){0 4 5 6$10&u:'abc'
(i.2 0 3 4 5 6) -: (i.2 0 3){0 4 5 6$s:@<"0 'abc'
(i.2 0 3 4 5 6) -: (i.2 0 3){0 4 5 6$4
(i.2 0 3 4 5 6) -: (i.2 0 3){0 4 5 6$0.5

(i.4 5 2 0 3 6) -: (i.2 0 3){"_ 2 [ 4 5 0 6$'abc'
(i.4 5 2 0 3 6) -: (i.2 0 3){"_ 2 [ 4 5 0 6$u:'abc'
(i.4 5 2 0 3 6) -: (i.2 0 3){"_ 2 [ 4 5 0 6$10&u:'abc'
(i.4 5 2 0 3 6) -: (i.2 0 3){"_ 2 [ 4 5 0 6$s:@<"0 'abc'
(i.4 5 2 0 3 6) -: (i.2 0 3){"_ 2 [ 4 5 0 6$4
(i.4 5 2 0 3 6) -: (i.2 0 3){"_ 2 [ 4 5 0 6$0.5

j -: x i. j{x [ j=:?~#x=: ~.a.{~?100 1$256
j -: x i. j{x [ j=:?~#x=: ~.a.{~?100 2$256
j -: x i. j{x [ j=:?~#x=: ~.a.{~?100 3$256
j -: x i. j{x [ j=:?~#x=: ~.a.{~?100 4$256
j -: x i. j{x [ j=:?~#x=: ~.a.{~?100 5$256
j -: x i. j{x [ j=:?~#x=: ~.a.{~?100 6$256
j -: x i. j{x [ j=:?~#x=: ~.a.{~?100 7$256
j -: x i. j{x [ j=:?~#x=: ~.a.{~?100 8$256
j -: x i. j{x [ j=:?~#x=: ~.a.{~?100 9$256

NB. scalar right arguments

x -: i{ x=: 0    [ i=: <i.0
x -: i{ x=: 'a'
x -: i{ x=: 2
x -: i{ x=: 2.5
x -: i{ x=: 2j5
x -: i{ x=: <2 3 4
x -: i{ x=: 2x

x -: i{ x=: 0    [ i=: <''
x -: i{ x=: 0    [ i=: <u:''
x -: i{ x=: 0    [ i=: <10&u:''
x -: i{ x=: 0    [ i=: <s:@<"0 ''
x -: i{ x=: 'a'
x -: i{ x=: u:'a'
x -: i{ x=: 10&u:'a'
x -: i{ x=: s:@<"0 'a'
x -: i{ x=: 2
x -: i{ x=: 2.5
x -: i{ x=: 2j5
x -: i{ x=: <2 3 4
x -: i{ x=: 2x

x -: i{ x=: 0    [ i=: <0$<i.12
x -: i{ x=: 'a'
x -: i{ x=: u:'a'
x -: i{ x=: 10&u:'a'
x -: i{ x=: s:@<"0 'a'
x -: i{ x=: 2
x -: i{ x=: 2.5
x -: i{ x=: 2j5
x -: i{ x=: <2 3 4
x -: i{ x=: 2x

'domain error' -: 3.5       { etx i.12                       
'domain error' -: 'abc'     { etx i.12
'domain error' -: (u:'abc')     { etx i.12
'domain error' -: (10&u:'abc')     { etx i.12
'domain error' -: (s:@<"0 'abc')     { etx i.12
'domain error' -: (<3.5)    { etx i.12
'domain error' -: (<0;'abc'){ etx i.3 4
'domain error' -: (<0;u:'abc'){ etx i.3 4
'domain error' -: (<0;10&u:'abc'){ etx i.3 4
'domain error' -: (<0;s:@<"0 'abc'){ etx i.3 4
'domain error' -: (<0;3.5)  { etx i.3 4

'length error' -: (<0 2)    { etx i.12
'length error' -: (<0;2)    { etx i.12

'index error'  -: 0         { etx 0 5$'abc'
'index error'  -: 0         { etx 0 5$u:'abc'
'index error'  -: 0         { etx 0 5$10&u:'abc'
'index error'  -: 0         { etx 0 5$s:@<"0 'abc'
'index error'  -: 0         { etx i.0 5
'index error'  -: 0         { etx o.i.0 5
'index error'  -: 2         { etx a=:1=?2 3 4$2                
'index error'  -: _3        { etx a                           
'index error'  -: (<0 _4 0) { etx a                    
'index error'  -: (<2 0)    { etx a                       

'index error'  -: 2         { etx a=:(?2 3 4$#a.){a.           
'index error'  -: 2         { etx a=:(?2 3 4$#adot1){adot1          
'index error'  -: 2         { etx a=:(?2 3 4$#adot2){adot2          
'index error'  -: 2         { etx a=:(?2 3 4$#sdot0){sdot0          
'index error'  -: _3        { etx a                           
'index error'  -: (<0 _4 0) { etx a                    
'index error'  -: (<2 0)    { etx a                       

'index error'  -: 2         { etx a=:?2 3 4$333                
'index error'  -: _3        { etx a                           
'index error'  -: (<0 _4 0) { etx a                    
'index error'  -: (<2 0)    { etx a

'index error'  -: 2         { etx a=:o.?2 3 4$333              
'index error'  -: _3        { etx a                           
'index error'  -: (<0 _4 0) { etx a                    
'index error'  -: (<2 0)    { etx a                       

'index error'  -: 2         { etx a=:^0j1*?2 3 4$33            
'index error'  -: _3        { etx a                           
'index error'  -: (<0 _4 0) { etx a                    
'index error'  -: (<2 0)    { etx a                       

'index error'  -: 2         { etx a=:2 3 4$3;'abc'             
'index error'  -: 2         { etx a=:2 3 4$3;u:'abc'             
'index error'  -: 2         { etx a=:2 3 4$3;10&u:'abc'             
'index error'  -: 2         { etx a=:2 3 4$3;s:@<"0 'abc'             
'index error'  -: _3        { etx a                           
'index error'  -: (<0 _4 0) { etx a                    
'index error'  -: (<2 0)    { etx a                       
'index error'  -:  (0 1 2 1) {"2 etx i. 1e9 1e9 1e9 3 0 4
'index error'  -:  (0) {"2 etx i. 1e9 1e9 1e9 3 0 4


NB. x{y boxed indices ---------------------------------------------------

fr =: 4 : '>{&.>/(<"0|.>x),<y'

(<i=:      <:s-?+:s) ({ -: fr) ?s$2          [ s=:2 3 4 7
(<i=:      <:s-?+:s) ({ -: fr) a.{~?s$#a.    [ s=:2 3 4 7 11
(<i=:      <:s-?+:s) ({ -: fr) adot1{~?s$#adot1    [ s=:2 3 4 7 11
(<i=:      <:s-?+:s) ({ -: fr) adot2{~?s$#adot2    [ s=:2 3 4 7 11
(<i=:      <:s-?+:s) ({ -: fr) sdot0{~?s$#sdot0    [ s=:2 3 4 7 11
(<i=:      <:s-?+:s) ({ -: fr)   _1e6+?s$2e6 [ s=:2 5 1 7 1 1
(<i=:      <:s-?+:s) ({ -: fr) o._1e6+?s$2e6 [ s=:2 1 5 1 1 7 3
(<i=:      <:s-?+:s) ({ -: fr) r._1e6+?s$2e6 [ s=:1 2 5 1 1 1 1 7
(<i=:      <:s-?+:s) ({ -: fr) <"0?s$1e8     [ s=:2 3 4 7 1 1
 
(<i=:    }:<:s-?+:s) ({ -: fr) ?s$2          [ s=:2 3 4 7
(<i=:    }:<:s-?+:s) ({ -: fr) a.{~?s$#a.    [ s=:2 3 4 7 11
(<i=:    }:<:s-?+:s) ({ -: fr) adot1{~?s$#adot1    [ s=:2 3 4 7 11
(<i=:    }:<:s-?+:s) ({ -: fr) adot2{~?s$#adot2    [ s=:2 3 4 7 11
(<i=:    }:<:s-?+:s) ({ -: fr) sdot0{~?s$#sdot0    [ s=:2 3 4 7 11
(<i=:    }:<:s-?+:s) ({ -: fr)   _1e6+?s$2e6 [ s=:2 5 1 7 1 1
(<i=:    }:<:s-?+:s) ({ -: fr) o._1e6+?s$2e6 [ s=:2 1 5 1 1 7 3
(<i=:    }:<:s-?+:s) ({ -: fr) r._1e6+?s$2e6 [ s=:1 2 5 1 1 1 1 7
(<i=:    }:<:s-?+:s) ({ -: fr) <"0?s$1e8     [ s=:2 3 4 7 1 1
 
(<i=:<"0   <:s-?+:s) ({ -: fr) ?s$2          [ s=:2 3 4 7
(<i=:<"0   <:s-?+:s) ({ -: fr) a.{~?s$#a.    [ s=:2 3 4 7 11
(<i=:<"0   <:s-?+:s) ({ -: fr) adot1{~?s$#adot1    [ s=:2 3 4 7 11
(<i=:<"0   <:s-?+:s) ({ -: fr) adot2{~?s$#adot2    [ s=:2 3 4 7 11
(<i=:<"0   <:s-?+:s) ({ -: fr) sdot0{~?s$#sdot0    [ s=:2 3 4 7 11
(<i=:<"0   <:s-?+:s) ({ -: fr)   _1e6+?s$2e6 [ s=:2 5 1 7 1 1
(<i=:<"0   <:s-?+:s) ({ -: fr) o._1e6+?s$2e6 [ s=:2 1 5 1 1 7 3
(<i=:<"0   <:s-?+:s) ({ -: fr) r._1e6+?s$2e6 [ s=:1 2 5 1 1 1 1 7
(<i=:<"0   <:s-?+:s) ({ -: fr) <"0?s$1e8     [ s=:2 3 4 7 1 1
 
(<i=:<"0 }:<:s-?+:s) ({ -: fr) ?s$2          [ s=:2 3 4 7
(<i=:<"0 }:<:s-?+:s) ({ -: fr) a.{~?s$#a.    [ s=:2 3 4 7 11
(<i=:<"0 }:<:s-?+:s) ({ -: fr) adot1{~?s$#adot1    [ s=:2 3 4 7 11
(<i=:<"0 }:<:s-?+:s) ({ -: fr) adot2{~?s$#adot2    [ s=:2 3 4 7 11
(<i=:<"0 }:<:s-?+:s) ({ -: fr) sdot0{~?s$#sdot0    [ s=:2 3 4 7 11
(<i=:<"0 }:<:s-?+:s) ({ -: fr)   _1e6+?s$2e6 [ s=:2 5 1 7 1 1
(<i=:<"0 }:<:s-?+:s) ({ -: fr) o._1e6+?s$2e6 [ s=:2 1 5 1 1 7 3
(<i=:<"0 }:<:s-?+:s) ({ -: fr) r._1e6+?s$2e6 [ s=:1 2 5 1 1 1 1 7
(<i=:<"0 }:<:s-?+:s) ({ -: fr) <"0?s$1e8     [ s=:2 3 4 7 1 1

x -: (<i.&.>4{.$x){x=:?2 3 4 5$1e6 
x -: (<i.&.>3{.$x){x
x -: (<i.&.>2{.$x){x
x -: (<i.&.>1{.$x){x
x -: (<i.&.>0{.$x){x

(|.|."_1 |."_2 |."_3 x) -: (<i.&.>-4{.$x){x=:?2 3 4 5$2
(|.|."_1 |."_2       x) -: (<i.&.>-3{.$x){x
(|.|."_1             x) -: (<i.&.>-2{.$x){x
(|.                  x) -: (<i.&.>-1{.$x){x

NB. literal
jot=:<$0
x=:(?3 4 5 7$#a){a=:'supercalifragilisticexpialidocious !@#$'
((<i; j         ){x) -: i{      j{"_ _1 x [ i=:?2 3$3 [ j=:?7 1 1$4
((<i; j; <a:    ){x) -: i{      j{"_ _1 x [ i=:?2 3$3 [ j=:?7 1 1$4
((<i; j; a:;<a: ){x) -: i{      j{"_ _1 x [ i=:?2 3$3 [ j=:?7 1 1$4
((<i; a:;j      ){x) -: i{      j{"_ _2 x [ i=:?23 $3 [ j=:?1 17 $5
((<i; a:;j; <a: ){x) -: i{      j{"_ _2 x [ i=:?23 $3 [ j=:?1 17 $5
((<i; a:;a:;j   ){x) -: i{      j{"_ _3 x [ i=:?2 3$3 [ j=:?23   $7
((<a:;i; j      ){x) -: i{"_ _1 j{"_ _2 x [ i=:?2 3$4 [ j=:?17   $5
((<a:;i; j; <a: ){x) -: i{"_ _1 j{"_ _2 x [ i=:?2 3$4 [ j=:?17 2 $5
((<a:;i; a:;j   ){x) -: i{"_ _1 j{"_ _3 x [ i=:?2 3$4 [ j=:?1 7 2$7
((<a:;a:;i; j   ){x) -: i{"_ _2 j{"_ _3 x [ i=:?23 $5 [ j=:?2 11 $7

i=:?&.(+&n)2 3 $n=:0{$x
j=:?&.(+&n)57  $n=:1{$x
k=:?&.(+&n)1 11$n=:2{$x
l=:?&.(+&n)13  $n=:3{$x
((<i;j;k;l){x) -: i{j{"_ _1 k{"_ _2 l{"_ _3 x
((<i;j;k  ){x) -: i{j{"_ _1 k{"_ _2         x
((<i;j    ){x) -: i{j{"_ _1                 x
((<<i     ){x) -: i{                        x

((<0 1)|:x) -: (,&.>~i.#x){x=:?15 15$10000

NB. literal2
jot=:<$0
x=:(?3 4 5 7$#a){a=:u:'supercalifragilisticexpialidocious !@#$'
((<i; j         ){x) -: i{      j{"_ _1 x [ i=:?2 3$3 [ j=:?7 1 1$4
((<i; j; <a:    ){x) -: i{      j{"_ _1 x [ i=:?2 3$3 [ j=:?7 1 1$4
((<i; j; a:;<a: ){x) -: i{      j{"_ _1 x [ i=:?2 3$3 [ j=:?7 1 1$4
((<i; a:;j      ){x) -: i{      j{"_ _2 x [ i=:?23 $3 [ j=:?1 17 $5
((<i; a:;j; <a: ){x) -: i{      j{"_ _2 x [ i=:?23 $3 [ j=:?1 17 $5
((<i; a:;a:;j   ){x) -: i{      j{"_ _3 x [ i=:?2 3$3 [ j=:?23   $7
((<a:;i; j      ){x) -: i{"_ _1 j{"_ _2 x [ i=:?2 3$4 [ j=:?17   $5
((<a:;i; j; <a: ){x) -: i{"_ _1 j{"_ _2 x [ i=:?2 3$4 [ j=:?17 2 $5
((<a:;i; a:;j   ){x) -: i{"_ _1 j{"_ _3 x [ i=:?2 3$4 [ j=:?1 7 2$7
((<a:;a:;i; j   ){x) -: i{"_ _2 j{"_ _3 x [ i=:?23 $5 [ j=:?2 11 $7

i=:?&.(+&n)2 3 $n=:0{$x
j=:?&.(+&n)57  $n=:1{$x
k=:?&.(+&n)1 11$n=:2{$x
l=:?&.(+&n)13  $n=:3{$x
((<i;j;k;l){x) -: i{j{"_ _1 k{"_ _2 l{"_ _3 x
((<i;j;k  ){x) -: i{j{"_ _1 k{"_ _2         x
((<i;j    ){x) -: i{j{"_ _1                 x
((<<i     ){x) -: i{                        x

((<0 1)|:x) -: (,&.>~i.#x){x=:?15 15$10000

NB. literal4
jot=:<$0
x=:(?3 4 5 7$#a){a=:10&u:'supercalifragilisticexpialidocious !@#$'
((<i; j         ){x) -: i{      j{"_ _1 x [ i=:?2 3$3 [ j=:?7 1 1$4
((<i; j; <a:    ){x) -: i{      j{"_ _1 x [ i=:?2 3$3 [ j=:?7 1 1$4
((<i; j; a:;<a: ){x) -: i{      j{"_ _1 x [ i=:?2 3$3 [ j=:?7 1 1$4
((<i; a:;j      ){x) -: i{      j{"_ _2 x [ i=:?23 $3 [ j=:?1 17 $5
((<i; a:;j; <a: ){x) -: i{      j{"_ _2 x [ i=:?23 $3 [ j=:?1 17 $5
((<i; a:;a:;j   ){x) -: i{      j{"_ _3 x [ i=:?2 3$3 [ j=:?23   $7
((<a:;i; j      ){x) -: i{"_ _1 j{"_ _2 x [ i=:?2 3$4 [ j=:?17   $5
((<a:;i; j; <a: ){x) -: i{"_ _1 j{"_ _2 x [ i=:?2 3$4 [ j=:?17 2 $5
((<a:;i; a:;j   ){x) -: i{"_ _1 j{"_ _3 x [ i=:?2 3$4 [ j=:?1 7 2$7
((<a:;a:;i; j   ){x) -: i{"_ _2 j{"_ _3 x [ i=:?23 $5 [ j=:?2 11 $7

i=:?&.(+&n)2 3 $n=:0{$x
j=:?&.(+&n)57  $n=:1{$x
k=:?&.(+&n)1 11$n=:2{$x
l=:?&.(+&n)13  $n=:3{$x
((<i;j;k;l){x) -: i{j{"_ _1 k{"_ _2 l{"_ _3 x
((<i;j;k  ){x) -: i{j{"_ _1 k{"_ _2         x
((<i;j    ){x) -: i{j{"_ _1                 x
((<<i     ){x) -: i{                        x

((<0 1)|:x) -: (,&.>~i.#x){x=:?15 15$10000

NB. symbol
jot=:<$0
x=:(?3 4 5 7$#a){a=:s:@<"0 'supercalifragilisticexpialidocious !@#$'
((<i; j         ){x) -: i{      j{"_ _1 x [ i=:?2 3$3 [ j=:?7 1 1$4
((<i; j; <a:    ){x) -: i{      j{"_ _1 x [ i=:?2 3$3 [ j=:?7 1 1$4
((<i; j; a:;<a: ){x) -: i{      j{"_ _1 x [ i=:?2 3$3 [ j=:?7 1 1$4
((<i; a:;j      ){x) -: i{      j{"_ _2 x [ i=:?23 $3 [ j=:?1 17 $5
((<i; a:;j; <a: ){x) -: i{      j{"_ _2 x [ i=:?23 $3 [ j=:?1 17 $5
((<i; a:;a:;j   ){x) -: i{      j{"_ _3 x [ i=:?2 3$3 [ j=:?23   $7
((<a:;i; j      ){x) -: i{"_ _1 j{"_ _2 x [ i=:?2 3$4 [ j=:?17   $5
((<a:;i; j; <a: ){x) -: i{"_ _1 j{"_ _2 x [ i=:?2 3$4 [ j=:?17 2 $5
((<a:;i; a:;j   ){x) -: i{"_ _1 j{"_ _3 x [ i=:?2 3$4 [ j=:?1 7 2$7
((<a:;a:;i; j   ){x) -: i{"_ _2 j{"_ _3 x [ i=:?23 $5 [ j=:?2 11 $7

i=:?&.(+&n)2 3 $n=:0{$x
j=:?&.(+&n)57  $n=:1{$x
k=:?&.(+&n)1 11$n=:2{$x
l=:?&.(+&n)13  $n=:3{$x
((<i;j;k;l){x) -: i{j{"_ _1 k{"_ _2 l{"_ _3 x
((<i;j;k  ){x) -: i{j{"_ _1 k{"_ _2         x
((<i;j    ){x) -: i{j{"_ _1                 x
((<<i     ){x) -: i{                        x

((<0 1)|:x) -: (,&.>~i.#x){x=:?15 15$10000


NB. i{y integer indices -------------------------------------------------

fr =: 4 : '((x**/s)+/i.s=.}.$y){,y'

i -: (i=:?2 3 4$1000) {  y=:i.1000
i -: (i=:?2 3 4$1000) fr y=:i.1000

(?100  $#y) ({ -: fr) y=:?200   $2
(?100  $#y) ({ -: fr) y=:?200  3$2
(?100  $#y) ({ -: fr) y=:?200 10$2
(?100  $#y) ({ -: fr) y=:?200 21$2
(?100 8$#y) ({ -: fr) y=:?200   $2
(?100 8$#y) ({ -: fr) y=:?200  3$2
(?100 8$#y) ({ -: fr) y=:?200 10$2
(?100 8$#y) ({ -: fr) y=:?200 21$2

(?100  $#y) ({ -: fr) y=:(?20   $256){a.
(?100  $#y) ({ -: fr) y=:(?20  3$256){a.
(?100  $#y) ({ -: fr) y=:(?20 10$256){a.
(?100  $#y) ({ -: fr) y=:(?20 21$256){a.
(?100 9$#y) ({ -: fr) y=:(?20   $256){a.
(?100 9$#y) ({ -: fr) y=:(?20  3$256){a.
(?100 9$#y) ({ -: fr) y=:(?20 10$256){a.
(?100 9$#y) ({ -: fr) y=:(?20 21$256){a.

(?100  $#y) ({ -: fr) y=:(?20   $(#adot1)){adot1
(?100  $#y) ({ -: fr) y=:(?20  3$(#adot1)){adot1
(?100  $#y) ({ -: fr) y=:(?20 10$(#adot1)){adot1
(?100  $#y) ({ -: fr) y=:(?20 21$(#adot1)){adot1
(?100 9$#y) ({ -: fr) y=:(?20   $(#adot1)){adot1
(?100 9$#y) ({ -: fr) y=:(?20  3$(#adot1)){adot1
(?100 9$#y) ({ -: fr) y=:(?20 10$(#adot1)){adot1
(?100 9$#y) ({ -: fr) y=:(?20 21$(#adot1)){adot1

(?100  $#y) ({ -: fr) y=:(?20   $(#adot2)){adot2
(?100  $#y) ({ -: fr) y=:(?20  3$(#adot2)){adot2
(?100  $#y) ({ -: fr) y=:(?20 10$(#adot2)){adot2
(?100  $#y) ({ -: fr) y=:(?20 21$(#adot2)){adot2
(?100 9$#y) ({ -: fr) y=:(?20   $(#adot2)){adot2
(?100 9$#y) ({ -: fr) y=:(?20  3$(#adot2)){adot2
(?100 9$#y) ({ -: fr) y=:(?20 10$(#adot2)){adot2
(?100 9$#y) ({ -: fr) y=:(?20 21$(#adot2)){adot2

(?100  $#y) ({ -: fr) y=:(?20   $(#sdot0)){sdot0
(?100  $#y) ({ -: fr) y=:(?20  3$(#sdot0)){sdot0
(?100  $#y) ({ -: fr) y=:(?20 10$(#sdot0)){sdot0
(?100  $#y) ({ -: fr) y=:(?20 21$(#sdot0)){sdot0
(?100 9$#y) ({ -: fr) y=:(?20   $(#sdot0)){sdot0
(?100 9$#y) ({ -: fr) y=:(?20  3$(#sdot0)){sdot0
(?100 9$#y) ({ -: fr) y=:(?20 10$(#sdot0)){sdot0
(?100 9$#y) ({ -: fr) y=:(?20 21$(#sdot0)){sdot0

(?101  $#y) ({ -: fr) y=:?20   $29999
(?101  $#y) ({ -: fr) y=:?20  3$29999
(?101  $#y) ({ -: fr) y=:?20 10$29999
(?101  $#y) ({ -: fr) y=:?20 21$29999
(?101 7$#y) ({ -: fr) y=:?20   $29999
(?101 7$#y) ({ -: fr) y=:?20  3$29999
(?101 7$#y) ({ -: fr) y=:?20 10$29999
(?101 7$#y) ({ -: fr) y=:?20 21$29999
                                  
(?101  $#y) ({ -: fr) y=:o.?20   $29999
(?101  $#y) ({ -: fr) y=:o.?20  2$29999
(?101  $#y) ({ -: fr) y=:o.?20  5$29999
(?101  $#y) ({ -: fr) y=:o.?20 11$29999
(?101 7$#y) ({ -: fr) y=:o.?20   $29999
(?101 7$#y) ({ -: fr) y=:o.?20  2$29999
(?101 7$#y) ({ -: fr) y=:o.?20  5$29999
(?101 7$#y) ({ -: fr) y=:o.?20 11$29999

(?101  $#y) ({ -: fr) y=:r.?25   $29999
(?101  $#y) ({ -: fr) y=:r.?25  2$29999
(?101  $#y) ({ -: fr) y=:r.?25  5$29999
(?101  $#y) ({ -: fr) y=:r.?25  7$29999
(?101 7$#y) ({ -: fr) y=:r.?25   $29999
(?101 7$#y) ({ -: fr) y=:r.?25  2$29999
(?101 7$#y) ({ -: fr) y=:r.?25  5$29999
(?101 7$#y) ({ -: fr) y=:r.?25  7$29999
                                 
(?100  $#y) ({ -: fr) y=:(?23   $25){25$;:'opposable thumbs!'
(?100  $#y) ({ -: fr) y=:(?23   $25){25$(u:&.>) ;:'opposable thumbs!'
(?100  $#y) ({ -: fr) y=:(?23   $25){25$(10&u:&.>) ;:'opposable thumbs!'
(?100  $#y) ({ -: fr) y=:(?23   $25){25$s:@<"0&.> ;:'opposable thumbs!'
(?100  $#y) ({ -: fr) y=:(?23   $25){25$<"0@s: ;:'opposable thumbs!'
(?100  $#y) ({ -: fr) y=:(?23  3$25){25$;:'+/i.1 2$a.'
(?100  $#y) ({ -: fr) y=:(?23  3$25){25$(u:&.>) ;:'+/i.1 2$a.'
(?100  $#y) ({ -: fr) y=:(?23  3$25){25$(10&u:&.>) ;:'+/i.1 2$a.'
(?100  $#y) ({ -: fr) y=:(?23  3$25){25$s:@<"0&.> ;:'+/i.1 2$a.'
(?100  $#y) ({ -: fr) y=:(?23  3$25){25$<"0@s: ;:'+/i.1 2$a.'
(?100  $#y) ({ -: fr) y=:(?23 10$25){<"0?25$11234
(?100  $#y) ({ -: fr) y=:(?23 21$25){;/?25$12355
(?100 9$#y) ({ -: fr) y=:(?23   $25){;/o.?25$12345
(?100 9$#y) ({ -: fr) y=:(?23  3$25){;/r.?25$12355
(?100 9$#y) ({ -: fr) y=:(?23 10$25){;/j.?25$10000
(?100 9$#y) ({ -: fr) y=:(?23 21$25){;/o.?25$23145

'index error' -: (2-2){   etx i.0
'index error' -: (2-2){   etx ''
'index error' -: (2-2){   etx u:''
'index error' -: (2-2){   etx 10&u:''
'index error' -: (2-2){   etx s:''
'index error' -: (2-2){   etx 0 2 3$a:
'index error' -: (2-2){"1 etx     i.4 0
'index error' -: (2-2){"1 etx a.{~i.4 0

NB. for '' { i. 0 3 bug fix (,0) -: $ '' { i. 0 3
NB. for '' { i. 0 3 bug fix (,0) -: $ (0$0) { i. 0 3
NB. for '' { i. 0 3 bug fix (,0) -: $ (0$4) { i. 0 3

NB. x{"r y --------------------------------------------------------------

from =: 4 : 'x{y'

(?100$5) ({"1 -: from"1) ?67 5$2
(?100$5) ({"1 -: from"1) (?67 5$#x){x=:'sesquipedalian milquetoast'
(?100$5) ({"1 -: from"1) (?67 5$#x){x=:u:'sesquipedalian milquetoast'
(?100$5) ({"1 -: from"1) (?67 5$#x){x=:10&u:'sesquipedalian milquetoast'
(?100$5) ({"1 -: from"1) (?67 5$#x){x=:s:@<"0 'sesquipedalian milquetoast'
(?100$5) ({"1 -: from"1) _1e6+?67 5$2e6
(?100$5) ({"1 -: from"1) o._1e6+?67 5$2e6
(?100$5) ({"1 -: from"1) r._1e6+?67 5$2e6
(?100$5) ({"1 -: from"1) (?67 5$#x){x=:1;2;3;4;;:'quidnunc quinquagenarian 2e6'
(?100$5) ({"1 -: from"1) (?67 5$#x){x=:1;2;3;4;(u:&.>) ;:'quidnunc quinquagenarian 2e6'
(?100$5) ({"1 -: from"1) (?67 5$#x){x=:1;2;3;4;(10&u:&.>) ;:'quidnunc quinquagenarian 2e6'
(?100$5) ({"1 -: from"1) (?67 5$#x){x=:1;2;3;4;s:@<"0&.> (10&u:&.>) ;:'quidnunc quinquagenarian 2e6'
(?100$5) ({"1 -: from"1) (?67 5$#x){x=:1;2;3;4;<"0@s: (10&u:&.>) ;:'quidnunc quinquagenarian 2e6'

(_5+?100$10) ({"1 -: from"1) ?67 5$2
(_5+?100$10) ({"1 -: from"1) (?67 5$#x){x=:'boustrophedonic'
(_5+?100$10) ({"1 -: from"1) (?67 5$#x){x=:u:'boustrophedonic'
(_5+?100$10) ({"1 -: from"1) (?67 5$#x){x=:10&u:'boustrophedonic'
(_5+?100$10) ({"1 -: from"1) (?67 5$#x){x=:s:@<"0 'boustrophedonic'
(_5+?100$10) ({"1 -: from"1) _1e6+?67 5$2e6
(_5+?100$10) ({"1 -: from"1) o._1e6+?67 5$2e6
(_5+?100$10) ({"1 -: from"1) r._1e6+?67 5$2e6
(_5+?100$10) ({"1 -: from"1) (?67 5$#x){x=:1;2;3;4;;:'miasma eleemosynary gruntlement'
(_5+?100$10) ({"1 -: from"1) (?67 5$#x){x=:1;2;3;4;(u:&.>) ;:'miasma eleemosynary gruntlement'
(_5+?100$10) ({"1 -: from"1) (?67 5$#x){x=:1;2;3;4;(10&u:&.>) ;:'miasma eleemosynary gruntlement'
(_5+?100$10) ({"1 -: from"1) (?67 5$#x){x=:1;2;3;4;s:@<"0&.> ;:'miasma eleemosynary gruntlement'
(_5+?100$10) ({"1 -: from"1) (?67 5$#x){x=:1;2;3;4;<"0@s: ;:'miasma eleemosynary gruntlement'

(_5+?100$10) ({"1 -: from"1) ?3 67 5$2
(_5+?100$10) ({"1 -: from"1) (?3 67 5$#x){x=:'onomatopoeia'
(_5+?100$10) ({"1 -: from"1) (?3 67 5$#x){x=:u:'onomatopoeia'
(_5+?100$10) ({"1 -: from"1) (?3 67 5$#x){x=:10&u:'onomatopoeia'
(_5+?100$10) ({"1 -: from"1) (?3 67 5$#x){x=:s:@<"0 'onomatopoeia'
(_5+?100$10) ({"1 -: from"1) _1e6+?3 67 5$2e6
(_5+?100$10) ({"1 -: from"1) o._1e6+?3 67 5$2e6
(_5+?100$10) ({"1 -: from"1) r._1e6+?3  7 5$2e6
(_5+?100$10) ({"1 -: from"1) (?3 67 5$#x){x=:[&.>'supercalifragilisticexpialidocious'
(_5+?100$10) ({"1 -: from"1) (?3 67 5$#x){x=:[&.>u:'supercalifragilisticexpialidocious'
(_5+?100$10) ({"1 -: from"1) (?3 67 5$#x){x=:[&.>10&u:'supercalifragilisticexpialidocious'
(_5+?100$10) ({"1 -: from"1) (?3 67 5$#x){x=:[&.>s:@<"0 'supercalifragilisticexpialidocious'

(_67+?100$134) ({"2 -: from"2) ?3 67 5$2
(_67+?100$134) ({"2 -: from"2) (?3 67 5$#x){x=:'quotidian'
(_67+?100$134) ({"2 -: from"2) (?3 67 5$#x){x=:u:'quotidian'
(_67+?100$134) ({"2 -: from"2) (?3 67 5$#x){x=:10&u:'quotidian'
(_67+?100$134) ({"2 -: from"2) (?3 67 5$#x){x=:s:@<"0 'quotidian'
(_67+?100$134) ({"2 -: from"2) _1e6+?3 67 5$2e6
(_67+?100$134) ({"2 -: from"2) o._1e6+?3 67 5$2e6
(_7 +?100$ 14) ({"2 -: from"2) r._1e6+?3  7 5$2e6
(_67+?100$134) ({"2 -: from"2) <"0?3 67 5$3e6

(_5+?7 11$10) ({"_ 1 -: from"_ 1) ?67 5$2
(_5+?7 11$10) ({"_ 1 -: from"_ 1) (?67 5$#x){x=:'rhematic hoplite'
(_5+?7 11$10) ({"_ 1 -: from"_ 1) (?67 5$#x){x=:u:'rhematic hoplite'
(_5+?7 11$10) ({"_ 1 -: from"_ 1) (?67 5$#x){x=:10&u:'rhematic hoplite'
(_5+?7 11$10) ({"_ 1 -: from"_ 1) (?67 5$#x){x=:s:@<"0 'rhematic hoplite'
(_5+?7 11$10) ({"_ 1 -: from"_ 1) _1e6+?67 5$2e6
(_5+?7 11$10) ({"_ 1 -: from"_ 1) o._1e6+?67 5$2e6
(_5+?7 11$10) ({"_ 1 -: from"_ 1) r._1e6+? 7 5$2e6
(_5+?7 11$10) ({"_ 1 -: from"_ 1) (?67 5$#x){x=:;:'Cogito, ergo sum.'
(_5+?7 11$10) ({"_ 1 -: from"_ 1) (?67 5$#x){x=:(u:&.>) ;:'Cogito, ergo sum.'
(_5+?7 11$10) ({"_ 1 -: from"_ 1) (?67 5$#x){x=:(10&u:&.>) ;:'Cogito, ergo sum.'
(_5+?7 11$10) ({"_ 1 -: from"_ 1) (?67 5$#x){x=:s:@<"0&.> ;:'Cogito, ergo sum.'
(_5+?7 11$10) ({"_ 1 -: from"_ 1) (?67 5$#x){x=:<"0@s: ;:'Cogito, ergo sum.'

(_5+?67$10) ({"_1 -: from"_1) ?67 5$2
(_5+?67$10) ({"_1 -: from"_1) (?67 5$#x){x=:'tetragrammaton'
(_5+?67$10) ({"_1 -: from"_1) (?67 5$#x){x=:u:'tetragrammaton'
(_5+?67$10) ({"_1 -: from"_1) (?67 5$#x){x=:10&u:'tetragrammaton'
(_5+?67$10) ({"_1 -: from"_1) (?67 5$#x){x=:s:@<"0 'tetragrammaton'
(_5+?67$10) ({"_1 -: from"_1) _1e6+?67 5$2e6
(_5+?67$10) ({"_1 -: from"_1) o._1e6+?67 5$2e6
(_5+? 7$10) ({"_1 -: from"_1) r._1e6+? 7 5$2e6
(_5+?67$10) ({"_1 -: from"_1) (?67 5$#x){x=:1;2;3;;:'4 chthonic thalassic amanuensis'
(_5+?67$10) ({"_1 -: from"_1) (?67 5$#x){x=:1;2;3;(u:&.>) ;:'4 chthonic thalassic amanuensis'
(_5+?67$10) ({"_1 -: from"_1) (?67 5$#x){x=:1;2;3;(10&u:&.>) ;:'4 chthonic thalassic amanuensis'
(_5+?67$10) ({"_1 -: from"_1) (?67 5$#x){x=:1;2;3;s:@<"0&.> ;:'4 chthonic thalassic amanuensis'
(_5+?67$10) ({"_1 -: from"_1) (?67 5$#x){x=:1;2;3;<"0@s: ;:'4 chthonic thalassic amanuensis'

(<2) ({"1 -: from"1) x=:?4 5$2
(<2) ({"1 -: from"1) x=:a.{~?4 5$#a.
(<2) ({"1 -: from"1) x=:adot1{~?4 5$#adot1
(<2) ({"1 -: from"1) x=:adot1{~?4 5$#adot1
(<2) ({"1 -: from"1) x=:sdot0{~?4 5$#sdot0
(<2) ({"1 -: from"1) x=:?4 5$100
(<2) ({"1 -: from"1) x=:o.?4 5$100
(<2) ({"1 -: from"1) x=:j./?2 4 5$100

(<2) ({"2 -: from"2) x=:?3 4 5$2
(<2) ({"2 -: from"2) x=:a.{~?3 4 5$#a.
(<2) ({"2 -: from"2) x=:adot1{~?3 4 5$#adot1
(<2) ({"2 -: from"2) x=:adot2{~?3 4 5$#adot2
(<2) ({"2 -: from"2) x=:sdot0{~?3 4 5$#sdot0
(<2) ({"2 -: from"2) x=:?3 4 5$100
(<2) ({"2 -: from"2) x=:o.?3 4 5$100
(<2) ({"2 -: from"2) x=:j./?2 3 4 5$100

(<2 1) ({"2 -: from"2) x=:?3 4 5$2
(<2 1) ({"2 -: from"2) x=:a.{~?3 4 5$#a.
(<2 1) ({"2 -: from"2) x=:adot1{~?3 4 5$#adot1
(<2 1) ({"2 -: from"2) x=:adot2{~?3 4 5$#adot2
(<2 1) ({"2 -: from"2) x=:sdot0{~?3 4 5$#sdot0
(<2 1) ({"2 -: from"2) x=:?3 4 5$1000
(<2 1) ({"2 -: from"2) x=:o.?3 4 5$1000
(<2 1) ({"2 -: from"2) x=:j./?2 3 4 5$1000

(<<<2 1) ({"1 -: from"1) x=:?4 5$2
(<<<2 1) ({"1 -: from"1) x=:a.{~?4 5$#a.
(<<<2 1) ({"1 -: from"1) x=:adot1{~?4 5$#adot1
(<<<2 1) ({"1 -: from"1) x=:adot2{~?4 5$#adot2
(<<<2 1) ({"1 -: from"1) x=:sdot0{~?4 5$#sdot0
(<<<2 1) ({"1 -: from"1) x=:?4 5$1000
(<<<2 1) ({"1 -: from"1) x=:o.?4 5$1000
(<<<2 1) ({"1 -: from"1) x=:j./?2 4 5$1000

(6$&><"0 x) -: (6$0){"_ 0 x=:?4 5$2
(6$&><"0 x) -: (6$0){"_ 0 x=:(?4 5$#x){x=:'archipelago'
(6$&><"0 x) -: (6$0){"_ 0 x=:(?4 5$#x){x=:u:'archipelago'
(6$&><"0 x) -: (6$0){"_ 0 x=:(?4 5$#x){x=:10&u:'archipelago'
(6$&><"0 x) -: (6$0){"_ 0 x=:(?4 5$#x){x=:s:@<"0 'archipelago'
(6$&><"0 x) -: (6$0){"_ 0 x=:?4 5$1000
(6$&><"0 x) -: (6$0){"_ 0 x=:o.?4 5$1000
(6$&><"0 x) -: (6$0){"_ 0 x=:r.?4 5$1000
(6$&><"0 x) -: (6$0){"_ 0 x=:<"0?4 5$1000

(1 2;2 1)          ({"2 -: from"2) x=:?3 4 5$1000
(<1 2)             ({"2 -: from"2) x
(<"1 ?3   6 2$4 5) ({"2 -: from"2) x
(<"1 ?3   6 1$4  ) ({"2 -: from"2) x
(<"1 ?3 7 6 2$4 5) ({"2 -: from"2) x
(<"1 ?3 7 6 1$4  ) ({"2 -: from"2) x

NB. different langths of {"1
(100?@$75) (?@$&100@[ ({"1 -: from"1) ])"0 _ o._1e6+?12 100$2e6
(100?@$75) ((_10 + ?@$&100@[) ({"1 -: from"1) ])"0 _ o._1e6+?12 100$2e6

(0 4$'') -: (i.0 4){"1 'abc'

NB. for '' { i. 0 3 bug fix (,3) -: $ '' {"2 i. 3 0 3
NB. for '' { i. 0 3 bug fix (,3) -: $ (0$0) { i. 3 0 3
NB. for '' { i. 0 3 bug fix (,3) -: $ (0$4) { i. 3 0 3
NB. for '' { i. 0 3 bug fix (3 2 0) -: $ (2 0$' ') {"2 i. 3 0 3
NB. for '' { i. 0 3 bug fix (3 2 0) -: $ (2 0$0) {"2 i. 3 0 3
NB. for '' { i. 0 3 bug fix (3 2 0) -: $ (2 0$4) {"2 i. 3 0 3



'domain error' -: 'abc' {"1 etx i.3 4
'domain error' -: (u:'abc') {"1 etx i.3 4
'domain error' -: (10&u:'abc') {"1 etx i.3 4
'domain error' -: (s:@<"0 'abc') {"1 etx i.3 4
'domain error' -: 2.3   {"1 etx i.3 4
'domain error' -: 2j3   {"1 etx i.3 4
'domain error' -: (<'1'){"1 etx i.3 4

'length error' -: (i.7) {"0 1 etx i.8 9
'length error' -: (i.7) {"_1  etx 5 9$'asdf'
'length error' -: (i.7) {"_1  etx 5 9$u:'asdf'
'length error' -: (i.7) {"_1  etx 5 9$10&u:'asdf'
'length error' -: (i.7) {"_1  etx 5 9$s:@<"0 'asdf'
'length error' -: (<0 1){"1   etx i.3 4

'index error'  -: 5     {"1 etx ?4 5  $1234 
'index error'  -: _6    {"2 etx ?4 5 6$1234 
'index error'  -: 0     {"1 etx ?4 0  $1234 
'index error'  -: 0     {"2 etx ?4 0 5$1234 
'index error'  -: (<3)  {"1 etx ?5 2  $1234


NB. x{y complementary indexing ------------------------------------------

jot=:<$0

NB. Boolean
a =: 1=?2 3 4$2
(1{.a)       -: (<<<1){a
(1{.a)       -: (<<<1 _1){a
(1 0 1#"2 a) -: (<jot;<<1){a

NB. literal
a =: a.{~?2 3 4$256
(1{.a)       -: (<<<1){a
(1{.a)       -: (<<<1 _1){a
(1 0 1#"2 a) -: (<jot;<<_2){a

NB. literal2
a =: adot1{~?2 3 4$(#adot1)
(1{.a)       -: (<<<1){a
(1{.a)       -: (<<<1 _1){a
(1 0 1#"2 a) -: (<jot;<<_2){a

NB. literal4
a =: adot2{~?2 3 4$(#adot2)
(1{.a)       -: (<<<1){a
(1{.a)       -: (<<<1 _1){a
(1 0 1#"2 a) -: (<jot;<<_2){a

NB. symbol
a =: sdot0{~?2 3 4$(#sdot0)
(1{.a)       -: (<<<1){a
(1{.a)       -: (<<<1 _1){a
(1 0 1#"2 a) -: (<jot;<<_2){a

NB. integer
a =: ?2 3 4$256
(1{.a)       -: (<<<1){a
(1{.a)       -: (<<<1 _1){a
(1 0 1#"2 a) -: (<jot;<<1){a

NB. floating point
a =: o.?2 3 4$256
(1{.a)       -: (<<<1){a
(1{.a)       -: (<<<1 _1){a
(1 0 1#"2 a) -: (<jot;<<_2){a

NB. complex
a =: ^0j1*?2 3 4$256
(1{.a)       -: (<<<1){a
(1{.a)       -: (<<<1 _1){a
(1 0 1#"2 a) -: (<jot;<<1){a

NB. boxed
a =: 2 3 4$;:'+/..*(1 0 1#"2 a)-:(<jot;<<0){a'
(1{.a)       -: (<<<1){a
(1{.a)       -: (<<<1 _1){a
(1 0 1#"2 a) -: (<jot;<<_2){a

'domain error' -: (<<<'a')   { etx i.12
'domain error' -: (<<<3.5)   { etx i.12

'length error' -: (<0;<<_2)  { etx i.12

'index error'  -: (<<<2)     { etx a=:1=?2 3 4$2
'index error'  -: (<0;<<_4 2){ etx a
'index error'  -: (<<<2)     { etx a=:(?2 3 4$#a.){a.
'index error'  -: (<0;<<_4 2){ etx a
'index error'  -: (<<<2)     { etx a=:?2 3 4$1234
'index error'  -: (<0;<<_4 2){ etx a
'index error'  -: (<<<2)     { etx a=:o.?2 3 4$124
'index error'  -: (<0;<<_4 2){ etx a
'index error'  -: (<<<2)     { etx a=:r.?2 3 4$124
'index error'  -: (<0;<<_4 2){ etx a
'index error'  -: (<<<2)     { etx a=:2 3 4$'Mary';4
'index error'  -: (<0;<<_4 2){ etx a


NB. (<"1 x){y -----------------------------------------------------------

y=: ?(QKTEST{::7 11 13 17;11 13 17 19)$1e6
x=: ?(#$y)$$y
((<"1     x){y) -: x      (<"1@[ { ]) y
((<"1    -x){y) -: (-x)   (<"1@[ { ]) y
((<"1 <"0 x){y) -: (<"0 x)(<"1@[ { ]) y

y=: ?(QKTEST{::7 11 13 17;11 13 17 19)$1e6
x=: ?(0,#$y)$$y
((<"1     x){y) -: x      (<"1@[ { ]) y
((<"1    -x){y) -: (-x)   (<"1@[ { ]) y
((<"1 <"0 x){y) -: (<"0 x)(<"1@[ { ]) y

y=: ?(QKTEST{::7 11 13 17;11 13 17 19)$1e6
x=: ?(2 3 5 7 11,#$y)$$y
((<"1     x){y) -: x      (<"1@[ { ]) y
((<"1    -x){y) -: (-x)   (<"1@[ { ]) y
((<"1 <"0 x){y) -: (<"0 x)(<"1@[ { ]) y
NB. Test special cases: rank 2, rank 3, non-INT x
y=: ?17 19$1e6
x=: ?(2 3 5 7 11,0)$0{.$y
((<"1     x){y) -: x      (<"1@[ { ]) y
((<"1    -x){y) -: (-x)   (<"1@[ { ]) y
((<"1 <"0 x){y) -: (<"0 x)(<"1@[ { ]) y
x=: ?(2 3 5 7 11,1)$1{.$y
((<"1     x){y) -: x      (<"1@[ { ]) y
((<"1    -x){y) -: (-x)   (<"1@[ { ]) y
((<"1 <"0 x){y) -: (<"0 x)(<"1@[ { ]) y
x=: ?(2 3 5 7 11,2)$2{.$y
((<"1     x){y) -: x      (<"1@[ { ]) y
((<"1    -x){y) -: (-x)   (<"1@[ { ]) y
((<"1 <"0 x){y) -: (<"0 x)(<"1@[ { ]) y

y=: 0.5 + ?(QKTEST{::13 17 19;11 13 17)$1e6
x=: ?(2 3 5 7 11,0)$0{.$y
((<"1     x){y) -: x      (<"1@[ { ]) y
((<"1    -x){y) -: (-x)   (<"1@[ { ]) y
((<"1 <"0 x){y) -: (<"0 x)(<"1@[ { ]) y
x=: ?(2 3 5 7 11,1)$1{.$y
((<"1     x){y) -: x      (<"1@[ { ]) y
((<"1    -x){y) -: (-x)   (<"1@[ { ]) y
((<"1 <"0 x){y) -: (<"0 x)(<"1@[ { ]) y
x=: ?(2 3 5 7 11,2)$2{.$y
((<"1     x){y) -: x      (<"1@[ { ]) y
((<"1    -x){y) -: (-x)   (<"1@[ { ]) y
((<"1 <"0 x){y) -: (<"0 x)(<"1@[ { ]) y
x=: ?(2 3 5 7 11,3)$3{.$y
((<"1     x){y) -: x      (<"1@[ { ]) y
((<"1    -x){y) -: (-x)   (<"1@[ { ]) y
((<"1 <"0 x){y) -: (<"0 x)(<"1@[ { ]) y

y=: 1r2 + ?3 7 3 5 5$1e6
x=: ?(2 3 5 7 11,0)$0{.$y
((<"1     x){y) -: x      (<"1@[ { ]) y
((<"1    -x){y) -: (-x)   (<"1@[ { ]) y
((<"1 <"0 x){y) -: (<"0 x)(<"1@[ { ]) y
x=: ?(2 3 5 7 11,1)$1{.$y
((<"1     x){y) -: x      (<"1@[ { ]) y
((<"1    -x){y) -: (-x)   (<"1@[ { ]) y
((<"1 <"0 x){y) -: (<"0 x)(<"1@[ { ]) y
x=: ?(2 3 5 7 11,2)$2{.$y
((<"1     x){y) -: x      (<"1@[ { ]) y
((<"1    -x){y) -: (-x)   (<"1@[ { ]) y
((<"1 <"0 x){y) -: (<"0 x)(<"1@[ { ]) y
x=: ?(2 3 5 7 11,3)$3{.$y
((<"1     x){y) -: x      (<"1@[ { ]) y
((<"1    -x){y) -: (-x)   (<"1@[ { ]) y
((<"1 <"0 x){y) -: (<"0 x)(<"1@[ { ]) y
x=: ?(2 3 5 7 11,4)$4{.$y
((<"1     x){y) -: x      (<"1@[ { ]) y
((<"1    -x){y) -: (-x)   (<"1@[ { ]) y
((<"1 <"0 x){y) -: (<"0 x)(<"1@[ { ]) y
x=: ?(2 3 5 7 11,5)$5{.$y
((<"1     x){y) -: x      (<"1@[ { ]) y
((<"1    -x){y) -: (-x)   (<"1@[ { ]) y
((<"1 <"0 x){y) -: (<"0 x)(<"1@[ { ]) y


y=: ?2 2 2$1e6
x=: ?(2 3 5 7 11,#$y)$$y
((<"1     x){y) -: x      (<"1@[ { ]) y
((<"1    -x){y) -: (-x)   (<"1@[ { ]) y
((<"1 <"0 x){y) -: (<"0 x)(<"1@[ { ]) y



'domain error' -: 'abc' (<"1@[ { ]) etx y
'domain error' -: (u:'abc') (<"1@[ { ]) etx y
'domain error' -: (10&u:'abc') (<"1@[ { ]) etx y
'domain error' -: (s:@<"0 'abc') (<"1@[ { ]) etx y
'domain error' -: 3.5   (<"1@[ { ]) etx y

'index error'  -: 999   (<"1@[ { ]) etx y


NB. x{y leading indices are numeric singletons --------------------------

f=: 4 : 0
 (;$&.>>x)$ ,: > {&.>/ (a:($,)&.>|.>x),<y
)

s=: 11 7 5 3 2 13 7
x=: ?s$1e9

(i=: <?&.>1{.s) ({ -: f) x
(i=: <0:&.>1{.s) ({ -: f) x
(i=: <(0. + ?)&.>2{.s) ({ -: f) x
(i=: <((0 j. 0) + ?)&.>3{.s) ({ -: f) x
(i=: <(0x + ?)&.>4{.s) ({ -: f) x
(i=: <(0r3 + ?)&.>5{.s) ({ -: f) x
(i=: <?&.>6{.s) ({ -: f) x
(i=: <?&.>7{.s) ({ -: f) x

(i=: <((?1$5){.&.><7$1)$&.>?&.>1{.s) ({ -: f) x
(i=: <((?1$5){.&.><7$1)$&.>0:&.>1{.s) ({ -: f) x
(i=: <((?2$5){.&.><7$1)$&.>(0. + ?)&.>2{.s) ({ -: f) x
(i=: <((?3$5){.&.><7$1)$&.>((0 j. 0) + ?)&.>3{.s) ({ -: f) x
(i=: <((?4$5){.&.><7$1)$&.>(0x + ?)&.>4{.s) ({ -: f) x
(i=: <((?5$5){.&.><7$1)$&.>(0r3 + ?)&.>5{.s) ({ -: f) x
(i=: <((?6$5){.&.><7$1)$&.>?&.>6{.s) ({ -: f) x
(i=: <((?7$5){.&.><7$1)$&.>?&.>7{.s) ({ -: f) x

NB. test long names
abcdefghijabcdefghijabcdefghij0 =. 1
abcdefghijabcdefghijabcdefghij1 =. 2
abcdefghijabcdefghijabcdefghij00 =. 3
abcdefghijabcdefghijabcdefghij01 =. 4
abcdefghijabcdefghijabcdefghij000 =. 5
abcdefghijabcdefghijabcdefghij001 =. 6
abcdefghijabcdefghijabcdefghij0000 =. 7
abcdefghijabcdefghijabcdefghij0001 =. 8
abcdefghijabcdefghijabcdefghij0 -: 1
abcdefghijabcdefghijabcdefghij1 -: 2
abcdefghijabcdefghijabcdefghij00 -: 3
abcdefghijabcdefghijabcdefghij01 -: 4
abcdefghijabcdefghijabcdefghij000 -: 5
abcdefghijabcdefghijabcdefghij001 -: 6
abcdefghijabcdefghijabcdefghij0000 -: 7
abcdefghijabcdefghijabcdefghij0001 -: 8


NB. 128!:9  g;i;v;M ---------------------------------------------------------
{{)v
M =. 0. + =/~ i. 6
assert. (0;2 3 0 0 0 0) -: (128!:9) 1;0 1;2 3;M 
assert. (0;2 _3 0 0 0 0) -: (128!:9) 1;0 1;2 _3;M 
assert. (1;'') -: (128!:9) 0;0 1;2 _3;M 
assert. (0;_2 _3 0 0 0 0) -: (128!:9) 1;0 1;_2. _3;M 
assert. (0;0 0 0 0 0 0) -: (128!:9) 1;($0);($0);M 
assert. (0;0$0) -: (128!:9) 1;0 1;_2. _3;i. 0 6 
NB. Test every different length.  Different size of M are not so important
for_l. >:  i. 50 do.
  for_j. 4$0 do.
    'r c' =. $M =. _0.5 + ((?16),l+?20)?@$ 0
    ix =. l ? c  NB. indexes
    v =. l ?@$ 0  NB. vector values
    g =. 0 (?r)}^:(r>0) r$1  NB. allow one test-enabled value
    ref =. (ix{"1 M) +/@:*"1 v
    abort =. 0 e. g +. ref<:0
    if. abort do. ref=.'' end.
abr   =: abort;ref
    assert. (abort;ref) -:!.1e_11 (128!:9) err   =: g;ix;v;M
  end.
end.
1
}}^:(1 e. 'avx2' E. 9!:14'') 1


 
4!:55 ;:'a adot1 adot2 sdot0 b catalog copy count f fr from i j '
4!:55 ;:'jot k l n p prod q r s v x y '
4!:55 <'abcdefghijabcdefghijabcdefghij0'
4!:55 <'abcdefghijabcdefghijabcdefghij1'
4!:55 <'abcdefghijabcdefghijabcdefghij00'
4!:55 <'abcdefghijabcdefghijabcdefghij01'
4!:55 <'abcdefghijabcdefghijabcdefghij000'
4!:55 <'abcdefghijabcdefghijabcdefghij001'
4!:55 <'abcdefghijabcdefghijabcdefghij0000'
4!:55 <'abcdefghijabcdefghijabcdefghij0001'
randfini''

