prolog './g5x5.ijs'
NB. 5!:5 ----------------------------------------------------------------

randuni''

ar=: 5!:1

test1=: 1 : 0
 0!:10 'asdf=.',5!:5 <'x'
 assert. 0 <: 4!:0<'asdf'
 assert. (ar <'x') -: ar <'asdf'
 1
)

test2=: 3 : 0
 0!:10 'asdf=.',5!:5 y
 assert. (ar y) -: ar <'asdf'
)


NB. 5!:5 nouns ----------------------------------------------------------

test=: [ -: ".@(3 : '5!:5<''y''')

NB. literal
test ''
test 0 1$''
test 1 0$''
test 0 3 4$'abc'
test 4 3 2 4 9 0 3 4$'Cogito, ergo sum.'
test 'j'
test ,'j'
test 5$'j'
test 13$'j'
test ''''
test ,''''
test 5$''''
test 6$''''
test 23$''''
test 24$''''
test 'sui generis'
test 'Don''t tread on me!'
test 1 1$'j'
test 1 3$'row'
test 6 1$'column'
test 2 3$'j'
test 1 13$'j'
test 13 1$'j'
test 6 12$'j'
test 2 3$''''
test 1 13$''''
test 13 1$''''
test 6 13$''''
test 3 7$'Opposable thumbs'
test 6 13$'Don''t tread on me!'
test 2 3 4$'j'
test 2 3 4$''''
test 2 3 4$'Don''t tread on me!'
test (?2 3 4 5$#a.){a.
test 'abc', a.
test 'abc',~a.
test (0    {a.), a.
test (0    {a.),~a.
test (0   1{a.), a.
test (0   1{a.),~a.
test (0   4{a.), a.
test (0   4{a.),~a.
test (_1   {a.), a.
test (_1   {a.),~a.
test (_2 _1{a.), a.
test (_2 _1{a.),~a.
test (_4 _1{a.), a.
test (_4 _1{a.),~a.
test '_' (a.i.' ')}a.
test (a.{~(i.26)+a.i.'a') ((i.26)+a.i.'A')}a.
test |.a.
test a.{~?#a.

NB. literal2
test u:''
test 0 1$u:''
test 1 0$u:''
test 0 3 4$u:'abc'
test 4 3 2 4 9 0 3 4$u:'Cogito, ergo sum.'
test u:'j'
test ,u:'j'
test 5$u:'j'
test 13$u:'j'
test u:''''
test ,u:''''
test 5$u:''''
test 6$u:''''
test 23$u:''''
test 24$u:''''
test u:'sui generis'
test u:'Don''t tread on me!'
test 1 1$u:'j'
test 1 3$u:'row'
test 6 1$u:'column'
test 2 3$u:'j'
test 1 13$u:'j'
test 13 1$u:'j'
test 6 12$u:'j'
test 2 3$u:''''
test 1 13$u:''''
test 13 1$u:''''
test 6 13$u:''''
test 3 7$u:'Opposable thumbs'
test 6 13$u:'Don''t tread on me!'
test 2 3 4$u:'j'
test 2 3 4$u:''''
test 2 3 4$u:'Don''t tread on me!'
test (?2 3 4 5$#adot1){adot1
test (u:'abc'), adot1
test (u:'abc'),~adot1
test (0    {adot1), adot1
test (0    {adot1),~adot1
test (0   1{adot1), adot1
test (0   1{adot1),~adot1
test (0   4{adot1), adot1
test (0   4{adot1),~adot1
test (_1   {adot1), adot1
test (_1   {adot1),~adot1
test (_2 _1{adot1), adot1
test (_2 _1{adot1),~adot1
test (_4 _1{adot1), adot1
test (_4 _1{adot1),~adot1
test (u:'_') (adot1 i.{.adot1)}adot1
test (adot1{~(i.26)+adot1 i.{.adot1) ((-i.26)+adot1 i.{:adot1)}adot1
test |.adot1
test adot1{~?#adot1

NB. literal4
test 10&u:''
test 0 1$10&u:''
test 1 0$10&u:''
test 0 3 4$10&u:'abc'
test 4 3 2 4 9 0 3 4$10&u:'Cogito, ergo sum.'
test 10&u:'j'
test ,10&u:'j'
test 5$10&u:'j'
test 13$10&u:'j'
test 10&u:''''
test ,10&u:''''
test 5$10&u:''''
test 6$10&u:''''
test 23$10&u:''''
test 24$10&u:''''
test 10&u:'sui generis'
test 10&u:'Don''t tread on me!'
test 1 1$10&u:'j'
test 1 3$10&u:'row'
test 6 1$10&u:'column'
test 2 3$10&u:'j'
test 1 13$10&u:'j'
test 13 1$10&u:'j'
test 6 12$10&u:'j'
test 2 3$10&u:''''
test 1 13$10&u:''''
test 13 1$10&u:''''
test 6 13$10&u:''''
test 3 7$10&u:'Opposable thumbs'
test 6 13$10&u:'Don''t tread on me!'
test 2 3 4$10&u:'j'
test 2 3 4$10&u:''''
test 2 3 4$10&u:'Don''t tread on me!'
test (?2 3 4 5$#adot2){adot2
test (10&u:'abc'), adot2
test (10&u:'abc'),~adot2
test (0    {adot2), adot2
test (0    {adot2),~adot2
test (0   1{adot2), adot2
test (0   1{adot2),~adot2
test (0   4{adot2), adot2
test (0   4{adot2),~adot2
test (_1   {adot2), adot2
test (_1   {adot2),~adot2
test (_2 _1{adot2), adot2
test (_2 _1{adot2),~adot2
test (_4 _1{adot2), adot2
test (_4 _1{adot2),~adot2
test (10&u:'_') (adot2 i.{.adot2)}adot2
test (adot2{~(i.26)+adot2 i.{.adot2) ((-i.26)+adot2 i.{:adot2)}adot2
test |.adot2
test adot2{~?#adot2

test 0
test i.0
test i.1 0
test i.0 1
test i.0 3 4 0
test 3j4
test ,_123
test _123 2 3 4
test 0 _1.2e_3j_4.5e_6 7 8 9 0 _8
test 5$183164
test 13$183164
test x=:?13$183164
test x=:?2 3$183164
test x=:?1 1$183164
test x=:?1 7$183164
test x=:?7 1$183164
test x=:o.i.3 4
test x=:o.>:i.-4 5
test x=:r.?2 3 4$10
test j.i.4 5
test r.i.8
test i.3 4
test 5+i.3 4
test 5+2*i.3 4
test 5+_1*i.3 4
test i.-3 4
test 10$_
test 4 5$__
test 4 5$_ 0 __

NB. box literal
test 0$<''
test 0 1$<'abc'
test 1 0$<'abc'
test 0 3 4 5 0$<'abc'
test <''
test a:
test <'j'
test <,'j'
test ,<'foobar'
test ;:'Cogito, ergo sum.'
test '' ;;:'sui generis'
test 'f';;:'sui generis'
test ,&.>'f';;:'sui generis'
test 'Opposable';'thumbs'
test 'Opposable';'thumbs '
test 2 3$'foobar';?2 3$183164
test 'abcd';5$233{a.
test <0;'abcd'
test <(,0);'abcd'
test 5!:1<'test'
test 5!:2<'test'
test 5!:4<'test'
test 5!:5<'test'

'a:' -: 5!:5 <'t' [ t=:a:

NB. box literal2
test 0$<u:''
test 0 1$<u:'abc'
test 1 0$<u:'abc'
test 0 3 4 5 0$<u:'abc'
test <u:''
test <u:'j'
test <,u:'j'
test ,<u:'foobar'
test (u:&.>) ;:'Cogito, ergo sum.'
test (u:'') ;(u:&.>) ;:'sui generis'
test (u:'f');(u:&.>) ;:'sui generis'
test ,&.>(u:'f');(u:&.>) ;:'sui generis'
test u:&.>'Opposable';'thumbs'
test u:&.>'Opposable';'thumbs '
test 2 3$(u:'foobar');?2 3$183164
test (u:'abcd');5$233{adot1
test <0;u:'abcd'
test <(,0);u:'abcd'
test 5!:1<u:'test'
test 5!:2<u:'test'
test 5!:4<u:'test'
test 5!:5<u:'test'

NB. box literal4
test 0$<10&u:''
test 0 1$<10&u:'abc'
test 1 0$<10&u:'abc'
test 0 3 4 5 0$<10&u:'abc'
test <10&u:''
test <10&u:'j'
test <,10&u:'j'
test ,<10&u:'foobar'
test (10&u:&.>) ;:'Cogito, ergo sum.'
test (10&u:'') ;(10&u:&.>) ;:'sui generis'
test (10&u:'f');(10&u:&.>) ;:'sui generis'
test ,&.>(10&u:'f');(10&u:&.>) ;:'sui generis'
test 10&u:&.>'Opposable';'thumbs'
test 10&u:&.>'Opposable';'thumbs '
test 2 3$(10&u:'foobar');?2 3$183164
test (10&u:'abcd');5$233{adot1
test <0;10&u:'abcd'
test <(,0);10&u:'abcd'
test 5!:1<10&u:'test'
test 5!:2<10&u:'test'
test 5!:4<10&u:'test'
test 5!:5<10&u:'test'

'00' -: 5!:5 <'s' [ s =: 00
'01' -: 5!:5 <'s' [ s =: 01
(,'2') -: 5!:5 <'s' [ s =: 2
'0 00' -: 5!:5 <'s' [ s =: 00 0
'1 00' -: 5!:5 <'s' [ s =: 01 0
'0 2' -: 5!:5 <'s' [ s =: 0 2
'0.' -: 5!:5 <'s' [ s =: 0.5-0.5
'0 0.' -: 5!:5 <'s' [ s =: 2 # 0.5-0.5
'0.5 0' -: 5!:5 <'s' [ s =: 1 0.5-0.5
'1r1' -: 5!:5 <'s' [ s=: 1r3+2r3
'0 1r1' -: 5!:5 <'s' [ s=: 0 1r1
'0 1r1' -: 5!:5 <'s' [ s=: 0r1 1
'1j0' -: 5!:5 <'s' [ s=: 1j0
'1 2j0' -: 5!:5 <'s' [ s=: 1 2j0
'1 2j0' -: 5!:5 <'s' [ s=: 1j0 2
(,'_') -: 5!:5 <'s' [ s =: _
'_ 2' -: 5!:5 <'s' [ s =: _ 2
'2 _' -: 5!:5 <'s' [ s =: 2 _
'_2.' -: 5!:5 <'s' [ s =: _2.5 + 0.5
'1.e20' -: 5!:5 <'s' [ s =: 1.e20
'6 1.e20' -: 5!:5 <'s' [ s =: 6 1.e20
'1e20 6.' -: 5!:5 <'s' [ s =: 1e20 6
s =: +;.0
'+;.0' -: 5!:5 <'s'

NB. 5!:5 empty arrays ---------------------------------------------------

test=: 3 : '((3!:0 x) -: 3!:0 y) *. x -: y [ x=. ".5!:5 <''y'''

test 0$0
test 0$'abc'
test 0$u:'abc'
test 0$10&u:'abc'
test 0$s: ' a b c'
test 0$3 4
test 0$3.4
test 0$3j4
NB. test 0$3r4
test 0$34x
test 0$3r4
test 0$3;4

test s$0             [ s=: 0 (?#s)}s=: ?10$5
test s$'abc'         [ s=: 0 (?#s)}s=: ?10$5
test s$u:'abc'       [ s=: 0 (?#s)}s=: ?10$5
test s$10&u:'abc'    [ s=: 0 (?#s)}s=: ?10$5
test s$s: ' a b c'   [ s=: 0 (?#s)}s=: ?10$5
test s$3 4           [ s=: 0 (?#s)}s=: ?10$5
test s$3.4           [ s=: 0 (?#s)}s=: ?10$5
test s$3j4           [ s=: 0 (?#s)}s=: ?10$5
NB. test s$3r4       [ s=: 0 (?#s)}s=: ?10$5
test s$34x           [ s=: 0 (?#s)}s=: ?10$5
test s$3;4           [ s=: 0 (?#s)}s=: ?10$5


NB. 5!:5 ----------------------------------------------------------------

+                                test1
+.                               test1
+:                               test1
j.                               test1
0:                               test1
+ t. ''                          test1
f                                test1
+/                               test1
+./                              test1
+/ .*                            test1
3&$                              test1
,&(3 4$'asdf')                   test1
3&$ @ (,&(3 4$'asdf'))           test1
3&$ @  ,&(3 4$'asdf')            test1
,&(3 4$u:'asdf')                 test1
3&$ @ (,&(3 4$u:'asdf'))         test1
3&$ @  ,&(3 4$u:'asdf')          test1
,&(3 4$10&u:'asdf')              test1
3&$ @ (,&(3 4$10&u:'asdf'))      test1
3&$ @  ,&(3 4$10&u:'asdf')       test1
3 : 'foo y'                      test1
3 : (':'; 'x bar y')             test1
3 : ('foo y'; ':'; 'x bar y')    test1
3 : (97 9 98{a.)                 test1
3 : (97 9 98{a.)                 test1
+/ : (- * %)                     test1
1 H. 1                           test1
*:      (2 : 'u"n y') (1;2;3)    test1
(4;5;6) (2 : 'u"n y') *:         test1
(4;5;6) (2 : 'u"n y') (1;2;3)    test1

(3x     (2&+) ])                 test1
(1 2 3x (2&+) ])                 test1
(-&3x   (2&+) ])                 test1
2 b.                             test1

f =:+ 
g =:- 
f3=:$
(+ %)       test1
(+&(3) 4&*) test1
(f g)       test1
(f3 g)      test1
(+(*#))     test1
(+(*#-))    test1
((+-) (*%)) test1
((+-) (*%#))test1
(+-)/       test1
((+-*)%)    test1
((+-*)#)    test1

f=:+ 
g=:- 
h=:$
(+-*              ) test1
((+&3) 4&- *      ) test1
(+ (-&4) 5&*      ) test1
(f g %            ) test1
(% f g            ) test1
(f g h            ) test1
(f g h f          ) test1
(f g h f g        ) test1
(f g h f g h      ) test1
((f g h) f g      ) test1
((f g  ) f g      ) test1
((f g h) f g h    ) test1
((f g  ) f g h    ) test1
((+-*) % #        ) test1
((+ *) % #        ) test1
(+ (-#*) %        ) test1
(+ (- *) %        ) test1
(+ - (*%#)        ) test1
(+ - (* #)        ) test1
((+=-) (*,%) ($;#)) test1
((+ -) (* %) ($ #)) test1

(+`-`*@.i.                  ) test1
(+/`(-&.+)`(%/\.)@.(i.@])   ) test1
((+%)`(-&.+)`(%/\.)@.(i.@]) ) test1
((+-*)`(-&.+)`(%/\.)@.(i.@])) test1

a =: 0 : 0
for_xxx. do.
 if. y > 4 do.
 sentence
 sentence sentence
 else.
 sentence sentence
 end.
end.
)
c =: 4 : a
('4 : 0',LF,a,')') -: 5!:5 <'c'
c test1


a=: /
test2 <'a' 
a=: @
test2 <'a' 
a=: @:
test2 <'a' 
a=: &
test2 <'a' 
a=: &:
test2 <'a' 
a=: &.
test2 <'a' 
a=: &.:
test2 <'a' 
a=: /.
test2 <'a'

c=: .
test2 <'c'
c=: :
test2 <'c'
NB. c=: ..
NB. test2 <'c'
NB. c=: .:
NB. test2 <'c'
c=: &.
test2 <'c'

a=: `]  
test2 <'a'
a=: `(+/ .* ) 
test2 <'a'
a=: `(+/%#)   
test2 <'a'
a=: `(+/`%`#) 
test2 <'a'
a=: *`         
test2 <'a'
a=: +/ .*`     
test2 <'a'
a=: (+/%#)`    
test2 <'a'
a=: +/`%`#`  
test2 <'a'
a=: (+% )`$`#` 
test2 <'a'
a=: (+-*)`$`#`  
test2 <'a'
a=: +/`%`#@.   
test2 <'a'
a=: (+% )`*`#@. 
test2 <'a'
a=: (+-*)`$`#@.
test2 <'a'
a=: +/`%`#`:0  
test2 <'a'
a=: (+% )`%`#`:0
test2 <'a'
a=: (+%-)`%`#`:0 
test2 <'a'
a=:   : -      
test2 <'a'
a=: - :      
test2 <'a'
a=: (+-)@     
test2 <'a'
a=: (+-*)@   
test2 <'a'

f=:&
g=:*
a=: f g       
test2 <'a'
a=: g f       
test2 <'a'
a=: @.(+/%#)
test2 <'a'
a=: `:0 
test2 <'a'

f=:g=:h=:/
a=: f g   
test2 <'a'     
a=: f g h      
test2 <'a'     
a=: f g h f    
test2 <'a'     
a=: f g h f g   
test2 <'a'     
a=: f g h (f g)  
test2 <'a'     
a=: f g   (f g)  
test2 <'a'     
a=: f g h (f g h)
test2 <'a'     
a=: f g   (f g h)
test2 <'a'     
a=: /\.(&+)("1)
test2 <'a'     
a=: (/\.)(+&)("1 _)

c=: 2 : '%&v@(+&v -&u ])'
test2 <'c'

test2 <'test1'
test2 <'test2'

NB. trains

testeq=: 2 : 0
 (ar <'u') -: ar(<'v')
 u2 =. (ar <'u') 5!:0
 (ar <'u') -: ar(<'u2')
 v2 =. (ar <'v') 5!:0
 (ar <'v') -: ar(<'v2')
)

a=: [.].
test2 <'a'     
(+-) testeq (+ a -)

a =: +-&
test2 <'a'     
(+-3&>) testeq (3 a >)

a =: /*-
test2 <'a'     
(+/*-) testeq (+ a)

a =: &*-
test2 <'a'     
(>&3/*-) testeq (> a 3)

a =: &*@
test2 <'a'     
(>&- * >@-) testeq (> a -)
8 = 3 ([.+].) 5

a =: /\*
test2 <'a'     
(+/-\*) testeq (+ a -)

a =: /\/.
test2 <'a'     
(+/\/.) testeq (+ a)

a =: &/\
test2 <'a'     
(+&-/\) testeq (+ a -)

a =: 3&/
test2 <'a'     
(3&(+/)) testeq (+ a)

a =: 3&@
test2 <'a'     
(3&(+@-)) testeq (+ a -)

a =: +&/
test2 <'a'     
(+@(-/)) testeq (- a)

a =: +&@
test2 <'a'     
(+&(*@-)) testeq (* a -)

a =: /&3
test2 <'a'     
(+/&3) testeq (+ a)

a =: /&*
test2 <'a'     
(+/&*) testeq (+ a)

a =: /@\
test2 <'a'     
(+/@(-\)) testeq (+ a -)

a =: /@&
test2 <'a'     
(+/&(+&3)) testeq (+ a 3)

a =: @&2
test2 <'a'     
(+@-&2) testeq (+ a -)

a =: @&*
test2 <'a'     
(+@-&*) testeq (+ a -)

a =: @&/
test2 <'a'     
(+@-&(-/)) testeq (+ a -)

a =: &:&@
test2 <'a'     
(+&:-&(+@-)) testeq (+ a -)
3 = 3 ([. [. ].) -

a =: 4&
test2 <'a'     
(4&+) testeq (+ a)

a =: -&
test2 <'a'     
(-&+) testeq (+ a)

a =: /-
test2 <'a'     
(+/-) testeq (+ a)

a =: /\
test2 <'a'     
(+/\) testeq (+ a)

a =: /&
test2 <'a'     
(+/&+) testeq (+ a)

a =: &3
test2 <'a'     
(+&3) testeq (+ a)

a =: &-
test2 <'a'     
(+&-) testeq (+ a)

a =: &/
test2 <'a'     
(+&5/) testeq (+ a 5)

NB. Verify that all trains produce an equivalent result after going through 5!:5

NB. y is a character string representing a possibly-invalid train
testlr=: {{
a =: y
try.
 0!:0 'asdf=.',;y
catch.
 1
end.
0!:0 'newy=.',5!:5 <'asdf'
assert. (ar <'newy') -: ar <'asdf'
1
}}"1
(1 2 3 4 5 6) (testlr @:({."1))"0 _ ({~   [: (#: i.@:(*/)) 6 # #) ;: '+ [. 3 ]: ( )'


NB. 5!:5 on "real" examples ---------------------------------------------

ar     =: 5!:1
boxed  =: 32&= @ type
oarg   =: >@(1&{)
mtv    =: i.@0:
paren  =: ('('&,)@(,&')')
symb   =: $&' '@(e.&'.:')@{. , ]
quote  =: ''''
alp    =: (,65 97+/i.26){a.
dig    =: '0123456789'

slist  =: $&','@(1&=)
shape  =: mtv`slist`(,&'$'@":)@.(2&<.@#)`('i.'&,@":) @. (0&e.) @ $
vchar  =: >:@(quote&=)@, quote&,@(,&quote)@# ,
vbox   =: }. @ ; @: (','&,@paren@('<'&,)@lnoun&.>)
value  =: vchar`vbox`(":!.18@,) @. (2 32&i.@(type * *@(*/)@$))
lnoun  =: shape , value

dotco  =: 2&=@# *. e.&'.:'@{:
name   =: e.&alp@{. *. *./@(e.&(alp,dig,'_'))@}: *. e.&(alp,dig,'_.:')@{:
num    =: e.&(dig,'_')@{. *. *./@(e.&(dig,'_ .ejdr'))
qstr   =: mtv -: -.@(~:/\)@(e.&quote) -.&quote@# ]
pstr   =: -.@(0&e.)@}:@(+/\)@({&1 _1 0)@('()'&i.)
nopar  =: 1&=@# +. dotco +. name +. num +. qstr +. pstr
cp     =: paren`] @. nopar

bp     =: ]`cp@.(' '&e.)
hfork  =: }.@;@:(' '&,@bp&.>)@]
left   =: bp@>@{.
right  =: mtv`(cp@>@{:)@.(1&<@#)
ins    =: left@] , symb@>@[ , right@]
act    =: ;@:(cp&.>)@]
insert =: hfork`hfork`act`act`act`ins @. ('23456'&i.@{.@>@[)

lx     =: {. insert lr&.>@oarg
ltie   =: lr`(}.@;@:('`'&,@cp@lr&.>)@oarg) @. ((<,'0')&=@{.)
lgl    =: {. insert (ltie&.>@{. , lr  &.>@}.)@oarg
lgr    =: {. insert (lr  &.>@{. , ltie&.>@}.)@oarg
lg     =: lgr`lgl`lx @. (i.&(<,'`')@oarg)
ltil   =: lx`(oarg@>@{.@oarg) @. ((<,'0')&=@{.@>@{.@oarg)
lcase  =: (cp@lnoun@oarg)`lgl`lgl`lg`ltil`lx @. ((;:'0@.`:4~')&i.@{.)
lr     =: symb`lcase@.boxed

lrep   =: lr @ > @ ar

test2"0 ;:'ar boxed oarg mtv paren symb quote alp dig'
test2"0 ;:'slist shape vchar vbox value lnoun'
test2"0 ;:'dotco name num qstr pstr nopar cp'
test2"0 ;:'bp hfork left right ins act insert'
test2"0 ;:'lx ltie lgl lgr lg ltil lcase lr lrep'

ar      =: 5!:1
boxed   =: 32&= @ type
mt      =: 0&e.@$
oarg    =: >@(1&{)
shr     =: |.!.''
shl     =: 1&(|.!.'')
mat     =: (1 1&}.)@(_1 _1&}.)@":@<
boxc    =: 9!:6 ''
dash    =: 10{boxc

extent  =: (+./\ *. +./\.) @ (' '&~:) @: ({."1)
limb1   =: 1&|.@$ 1&~: }. (10 6 0{boxc)&,@($&(9{boxc))
limb    =: -@(i.&1)@[ |. #@[ {. limb1@]
pfx     =: (limb +/)@extent ,. ]
pad     =: [ {. ] ,. dash&=@({:"1)@] {  ' '&,:@($&dash)@(-&{: $)
take    =: pad`($&' '@[) @. (mt@])
rc      =: #@>@{."1 ; >./@:({:@$@>)
kernt   =: (0{boxc)&=@shl@[ *. ' '&~:@]
kernb   =: (6{boxc)&=@] *. ' '&~:@shl@[
kern    =: (<0 0)&{&>"2 (kernt +./"1@:+. kernb) (<_1 0)&{&>"2
gap     =: ,&.>"_1 {&((0 1$' ');1 1$' ')@kern
graft   =: (pfx&.>@{. 0} ]) @ (,&.>/) @ gap @ ({@rc take&.> ])

lab     =: ,: @ (2&|.) @ ((' ',dash,dash,' ')&,)
label   =: lab`((,.dash)&[) @. (e.&'0123456789'@{.)
center  =: ((i.&1) -@+ <.@-:@(+/))@] |. #@] {. [
root    =: label@[ center extent@>@{.@]

leaf    =: ,@<@(((,:dash,' ')&[ center $&1@#) ,. ])@mat@":

trx     =: >@{. (root ; ]) graft@:(tr@>)@oarg
trgl    =: >@{. (root ; ]) graft@:(trx@>@{. , tr @>@}.)@oarg
trgr    =: >@{. (root ; ]) graft@:(tr @>@{. , trx@>@}.)@oarg
trg     =: trgr`trgl`trx @. (i.&(<,'`')@oarg)
trtil   =: trx`(leaf@oarg@>@{.@oarg) @. ((<,'0')&=@{.@>@{.@oarg)
trcase  =: (leaf@oarg)`trgl`trgl`trg`trtil`trx @. ((;:'0@.`:4~')&i.@{.)
tr      =: leaf`trcase @. boxed

rep     =: 2 : 'u & (((# i.@#)@,@v@])})'
right   =: (5{boxc) rep (e.&(9{boxc) *. shr"1@(e.&dash))
cross   =: (4{boxc) rep (e.&(5{boxc) *. shl"1@(e.&dash))
left    =: (3{boxc) rep (e.&(9{boxc) *. shl"1@(e.&dash))
bot     =: (7{boxc) rep (e.&(6{boxc) *. shr"1@(e.&dash))
connect =: bot @ left @ cross @ right

tree    =: connect @ > @ (,.&.>/) @ (> (root ; ]) tr@>@ar)

test2"0 ;:'ar boxed mt oarg shr shl mat boxc dash'
test2"0 ;:'extent limb1 limb pfx pad take rc kernt kernb kern gap graft'
test2"0 ;:'lab label center root leaf'
test2"0 ;:'trx trgl trgr trg trtil trcase tr'
test2"0 ;:'rep right cross left bot connect tree'

en     =: #@]
em     =: (en >.@% -@[)`(en 0&>.@>:@- [) @. (0&<:@[)
kay    =: en`em @. (0&<@[)
omask  =: (em,en) $ ($&0@|@[ , $&1@kay)

base   =: 1&>.@-@[ * i.@em
iind   =: base ,. |@[ <. en - base
seg    =: ((+i.)/@[ { ])"1 _

infix  =: 1 : '(iind  u@seg ])"0 _'
outfix =: 1 : '(omask u@#   ])"0 _'
prefix =: 1 : '>:@,.@i.@# u@{. ]'
suffix =: 1 : ',.@i.@#    u@}. ]'

key    =: 1 : '=@[ u@# ]'

osub   =: >@]`(>@[ >@:{ ]) @. (*@#@])
oind   =: (+/&i./ </.&, i.)@(2&{.)@(,&1 1)@$
ob     =: 1 : 'oind u@osub"0 1 ,@(<"_2)'

bs     =: 1 : '(u prefix) : (u infix )'
bsd    =: 1 : '(u suffix) : (u outfix)'
sd     =: 1 : '(u ob    ) : (u key   )'

test2"0 ;:'en em kay omask'
test2"0 ;:'base iind seg'
test2"0 ;:'infix outfix prefix suffix'
test2"0 ;:'key osub oind ob'
test2"0 ;:'bs bsd sd'

x =: (<,'&')`
'<,''&''`' -: 5!:5 <'x'
x =: `(<,'&')
'`(<,''&'')' -: 5!:5 <'x'
x =: (<,'/')`
'<,''/''`' -: 5!:5 <'x'
x =: `(<,'/')
'`(<,''/'')' -: 5!:5 <'x'
x =: (<,'+')`
'+`' -: 5!:5 <'x'
x =: `(<,'+')
'`+' -: 5!:5 <'x'

x =: (<,'&')`:
'<,''&''`:' -: 5!:5 <'x'
x =: `:(<,'&')
'`:(<,''&'')' -: 5!:5 <'x'
x =: (<,'/')`:
'<,''/''`:' -: 5!:5 <'x'
x =: `:(<,'/')
'`:(<,''/'')' -: 5!:5 <'x'
x =: (<,'+')`:
'+`:' -: 5!:5 <'x'
x =: `:(<,'+')
'`:(<,''+'')' -: 5!:5 <'x'

x =: (+ <!.0 (1 { [))
'+ (<!.0) 1 {  [' -: 5!:5 <'x'



NB. 5!:5, handling gerunds ----------------------------------------------

fx =: 5!:0
lr =: 5!:5

g =:[`((e.&' ' # i.@#)@])`]
f =: g}
(lr<'f') -: '[`((e.&'' '' # i.@#)@])`]} '
f =: (1{g) fx}
(lr<'f') -: '(e.&'' '' # i.@#)@]} '

g =: */\.`(i.@#)`(+/~)
f =: i.^:g
(lr<'f') -: 'i.^:(*/\.`(i.@#)`(+/~))'
f =: ^:g
(lr<'f') -: '^:(*/\.`(i.@#)`(+/~))'
f =: +/\^:
(lr<'f') -: '+/\^:'
f =: i.^:*
(lr<'f') -: 'i.^:*'

g =: */\.`(+/ % #)`]
f =: `g
(lr<'f') -: '`(*/\.`(+/ % #)`])'
f =: g`
(lr<'f') -: '*/\.`(+/ % #)`]`'

g =: ]`(+%)
f =: g `:0
(lr<'f') -: ']`(+ %)`:0'
f =: g`:
(lr<'f') -: ']`(+ %)`:'
f =: `:0
(lr<'f') -: '`:0'

g =: ((%&4@# , 4:) $ ]) ` %: ` $
f =: g@.*
(lr<'f') -: '((%&4@# , 4:) $ ])`%:`$@.*'
f =: g@.
(lr<'f') -: '((%&4@# , 4:) $ ])`%:`$@.'
f =: @.('abc'&i.)
(lr<'f') -: '@.(''abc''&i.)'

f =: 0`
(lr<'f') -: '0`'
f =: (<'0';0)`
(lr<'f') -: '<''0'';0`'
f =: `0
(lr<'f') -: '`0'
f =: `(<'0';0)
(lr<'f') -: '`(<''0'';0)'

f =: (1 0$'') 1 : 'u y'
(lr<'f') -: '(1 0$'''') (1 : ''u y'')'
f=: (<(<,':'),<(<(,'0');1),<(,'0');1 0$'') (1 : 'u u`:6`:6 y')
(lr<'f') -: '(<(<,'':''),<(<(,''0'');1),<(,''0'');1 0$'''') (1 : ''u u`:6`:6 y'')'

4!:55 ;:'a act adot1 adot2 sdot0 alp ar base bot boxc boxed bp bs '
4!:55 ;:'bsd c center connect cp cross dash dig dotco em '
4!:55 ;:'en extent f f3 fx g gap graft h hfork '
4!:55 ;:'id iind infix ins insert kay kern kernb kernt key '
4!:55 ;:'lab label lcase leaf left lg lgl lgr limb limb1 '
4!:55 ;:'lnoun lr lrep ltie ltil lx mat mt mtv name '
4!:55 ;:'nopar num oarg ob oind omask osub outfix pad paren '
4!:55 ;:'pfx prefix pstr qstr quote rc rep right root '
4!:55 ;:'s sd seg shape shl shr slist suffix symb '
4!:55 ;:'t take test test1 test2 testeq testlr tr trcase tree trg trgl trgr trtil trx '
4!:55 ;:'value vbox vchar x '
randfini''


epilog''

