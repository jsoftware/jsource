NB. scalar function integrals -------------------------------------------

I      =: 1 : 'x d. _1'
D      =: 1 : 'x d.  1'

t      =: 5e_11
z      =: (% |) j./ 16384 %~ _5e5 + 2 4 5 ?@$ 1e6  NB. data for testing
test   =: 1 : 'x I D -: x"0'                       NB. use as   f test z

testlr =: 1 : '(u testl y) , (u testr y)'

testl=: 1 : 0
 assert. t > | (      u     -     u       I D) y
 assert. t > | (<:   @u     - <: @u       I D) y
 assert. t > | (>:   @u     - >: @u       I D) y
 assert. t > | (+:   @u     - +: @u       I D) y
 assert. t > | (-    @u     - -  @u       I D) y
 assert. t > | (-.   @u     - -. @u       I D) y
 assert. t > | (-:   @u     - -: @u       I D) y
 assert. t > | (j.   @u     - j. @u       I D) y
 assert. t > | (0.3&+@u     - 0.3&+@u     I D) y
 assert. t > | (+&0.3@u     - +&0.3@u     I D) y
 assert. t > | (0.3&-@u     - 0.3&-@u     I D) y
 assert. t > | (-&0.3@u     - -&0.3@u     I D) y
 assert. t > | (0.3&*@u     - 0.3&*@u     I D) y
 assert. t > | (*&0.3@u     - *&0.3@u     I D) y
 assert. t > | (%&0.3@u     - %&0.3@u     I D) y
 assert. t > | (0.4 _0.3&p.        @u - (0.4 _0.3&p.)      @u I D) y
 assert. t > | ((0.5"0 + 0.7"0 * ])@u - (0.5"0 + 0.7"0 * ])@u I D) y
 1
)

testr=: 1 : 0
 assert. t > | (u           - u           I D) y
 assert. t > | (u@<:        - u@<:        I D) y
 assert. t > | (u@>:        - u@>:        I D) y
 assert. t > | (u@+:        - u@+:        I D) y
 assert. t > | (u@-         - u@-         I D) y
 assert. t > | (u@-.        - u@-.        I D) y
 assert. t > | (u@-:        - u@-:        I D) y
 assert. t > | (u@j.        - u@j.        I D) y
 assert. t > | (u@(0.3&+)   - u@(0.3&+)   I D) y
 assert. t > | (u@(+&0.3)   - u@(+&0.3)   I D) y
 assert. t > | (u@(0.3&-)   - u@(0.3&-)   I D) y
 assert. t > | (u@(-&0.3)   - u@(-&0.3)   I D) y
 assert. t > | (u@(0.4&*)   - u@(0.4&*)   I D) y
 assert. t > | (u@(*&0.5)   - u@(*&0.5)   I D) y
 assert. t > | (u@(%&3)     - u@(%&3)     I D) y
 assert. t > | (u@(0.4 _0.3&p.)       - u@(0.4 _0.3&p.)       I D) y
 assert. t > | (u@(0.5"0 + 0.7"0 * ]) - u@(0.5"0 + 0.7"0 * ]) I D) y
 1
)

<:   testlr z
>:   testlr z
+:   testlr z
*:   testlr z
-    testlr z
-.   testlr z
-:   testlr z
%:   testlr z
^    testlr z
^.   testlr z
]    testlr z
[    testlr z
j.   testlr z
o.   testlr z
r.   testlr z
_9:  testlr z
_8:  testlr z
_7:  testlr z
_6:  testlr z
_5:  testlr z
_4:  testlr z
_3:  testlr z
_2:  testlr z
_1:  testlr z
0:   testlr z
1:   testlr z
2:   testlr z
3:   testlr z
4:   testlr z
5:   testlr z
6:   testlr z
7:   testlr z
8:   testlr z
9:   testlr z

+ ~  testlr z
* ~  testlr z
- ~  testlr z
% ~  testlr z
^.~  testlr z

3&!  testlr z
1&o. testlr z
2&o. testlr z
3&o. testlr z
5&o. testlr z
6&o. testlr z
7&o. testlr z

*: @(1&o.) testlr z
^&2@(1&o.) testlr z
^&3@(1&o.) testlr z
^&4@(1&o.) testlr z
^&5@(1&o.) testlr z

*: @(2&o.) testlr z
^&2@(2&o.) testlr z
^&3@(2&o.) testlr z
^&4@(2&o.) testlr z
^&5@(2&o.) testlr z

*: @(3&o.) testlr z
^&2@(3&o.) testlr z
^&3@(3&o.) testlr z
^&4@(3&o.) testlr z
^&5@(3&o.) testlr z

*: @(7&o.) testlr z
^&2@(7&o.) testlr z
^&3@(7&o.) testlr z
^&4@(7&o.) testlr z
^&5@(7&o.) testlr z

*: @^.     testlr z
^&2@^.     testlr z
^&3@^.     testlr z
^&4@^.     testlr z
^&5@^.     testlr z


4!:55 ;:'D I t test testl testlr testr z'
