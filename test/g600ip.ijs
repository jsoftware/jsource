prolog './g600ip.ijs'

NB. atomic"n in-place and not -------------------------------
randuni''

NB. Use-count test
by =: (<51);(20 20+2.5-2.5);''
(22 0)  4 : 0  by
tlc =. x
'dos dotl wires' =. 3 {. y
dotl =. dotl +"1 tlc
1
)
by -: (<51);(20 20+2.5-2.5);''
NB. Use-count test
(22 0)  4 : 0  by
tlc =. x
'dos dotl wires' =. 3 {. y
dotl =. tlc +"1 dotl
1
)
by -: (<51);(20 20+2.5-2.5);''

NB. Test plusBI and plusIB with overflow and inplace

bx =: 0 1 0 0 0 1 0 1 0 0 0 0 1
IMAX =. _1 - IMIN =. <. - 2 ^ IF64 { 31 63
iy =: (($bx) ,3 5) $ IMAX,IMIN,3 5 6 7,IMIN,2 3,IMAX
(bx + iy) -: bx +"0"0 iy
(bx + (0 + iy)) -: bx +"0"0 iy
((0 +. bx) + iy) -: bx +"0"0 iy
((0 +. bx) + (0 + iy)) -: bx +"0"0 iy

bx =: 4 3$0 1 0 0 0 1 0 1 0 0 0 0 1 0
iy =: (($bx) ,3 5) $ IMAX,IMIN,3 5 6 7,IMIN,2 3,IMAX
(bx + iy) -: bx +"0"0 iy
(bx + (0 + iy)) -: bx +"0"0 iy
((0 +. bx) + iy) -: bx +"0"0 iy
((0 +. bx) + (0 + iy)) -: bx +"0"0 iy
iy =: (($bx) ,3 5) $ IMAX,IMIN,3 5 6 7,IMIN,2 3,IMAX
(bx +"1 2 iy) -: bx +"0"1 2 iy
(bx +"1 2 (0 + iy)) -: bx +"0"1 2 iy
((0 +. bx) +"1 2 iy) -: bx +"0"1 2 iy
((0 +. bx) +"1 2 (0 + iy)) -: bx +"0"1 2 iy

NB. Test minusBI and minusIB with overflow and inplace
bx =: 0 1 0 0 0 1 0 1 0 0 0 0 1
iy =: (($bx) ,3 5) $ IMAX,IMIN,3 5 6 7,IMIN,2 3,IMAX
(bx - iy) -: bx -"0"0 iy
(bx - (iy - 0)) -: bx -"0"0 iy
((0 +. bx) - iy) -: bx -"0"0 iy
((0 +. bx) - (iy - 0)) -: bx -"0"0 iy

bx =: 4 3$0 1 0 0 0 1 0 1 0 0 0 0 1 0
iy =: (($bx) ,3 5) $ IMAX,IMIN,3 5 6 7,IMIN,2 3,IMAX
(bx - iy) -: bx -"0"0 iy
(bx - (iy - 0)) -: bx -"0"0 iy
((0 +. bx) - iy) -: bx -"0"0 iy
((0 +. bx) - (iy - 0)) -: bx -"0"0 iy
iy =: (($bx) ,3 5) $ IMAX,IMIN,3 5 6 7,IMIN,2 3,IMAX
(bx -"1 2 iy) -: bx -"0"1 2 iy
(bx -"1 2 (iy - 0)) -: bx -"0"1 2 iy
((0 +. bx) -"1 2 iy) -: bx -"0"1 2 iy
((0 +. bx) -"1 2 (iy - 0)) -: bx -"0"1 2 iy


NB. x is selection;shapes;prediction
NB.  selection selects a prediction
NB.  prediction is string, 27 predictors separated by '/'; 1st  9 are for noninplace args, 2d for w in place, 3d for a in place
NB.   predictor contains the following chars:
NB.    k,l,L  allocate B01/INT/FL block for conversion of left arg
NB.    q,r,R  allocate B01/INT/FL block for conversion of right arg
NB.    B,b,I,D,d   allocate B01/INT/FL for result; d means (FL only if FL is bigger than INT); b is removed by caller if inplacing allowed
NB.     empty means 'no allocation', i.e. inplace result
NB.  shapes is (xshape,yshape,resultshape)
NB. y is # of bytes used in execution of the sentence
NB. Result is 1 if in range
checkallosize =: 4 : 0
'sel shapes pred' =: x
bytesused =: y
if. 0 = #pred do. 1 return. end.  NB. If no prediction, don't fail
assert. 27 = # predflds =: <;._2 pred
spred =: ' ' -.~ sel {:: predflds  NB. Select fielda
atomct =: 3 3 5 # */@> shapes   NB. Get #atoms in each arg/result
atomsz =: IF64 { 1 4 8 1 4 8 1 1 4 8 8 ,: 1 8 8 1 8 8 1 1 8 8 0   NB. Length of atoms
allobytes =: +/ >.&.(2&^.) (atomct*atomsz) {~ 'klLqrRBbIDd' i. spred  NB. Total # bytes allocated
IGNOREIFFVI (bytesused>allobytes-3000) *. (bytesused<allobytes+3000)
)


NB. u is verb; y is (left shape;right shape). n is 27x3x3 table of # of (b,i,d) buffers expected to be allocated in shape (x,y,result)
NB. Create random x and y; run u not-in-place and save result
NB. Run on combinations of inplaceable executions; verify result
testinplace =: 2 : 0
vb =: u f.
'xs ys' =: y
'resvalidity allopred' =: n
NB. Space calculations were made on the assumption that u was stacked by value.  Now it's not.  But y still is... so we will use
NB. y herein to mean 'u by value'
y =. u f.

if. 'B' e. resvalidity do.
NB. Test binary data, in all precisions

NB. Create random data
bx =: xs ?@$ 2
by =: ys ?@$ 2
NB. Make the first value 0.  This creates 0|0 which goes through a different path in the code; we need to do so predictably
if. #,bx do. bx =: 0 0:} bx end.
if. #,by do. by =: 0 0:} by end.
NB. Create copies in other precisions
ix =: bx + (2-2)
iy =: by + (2-2)
dx =: ix + (2.5-2.5)
dy =: iy + (2.5-2.5)
NB. Save all precisions in a separate area
'svbx svby svix sviy svdx svdy' =: svxy =: 3!:1&.> bx;by;ix;iy;dx;dy
NB. Get result with all looping performed in rank conjunction
r =: bx y"y"y by
xyzs =: xs;ys;$r  NB. Used to predict allocation
NB. For each precision combination, verify identical result
assert. (0;xyzs;allopred) checkallosize 7!:2 'tr =: bx y by'
assert. r -: tr [ 00 [  'tr =: bx y by'
assert. (1;xyzs;allopred) checkallosize 7!:2 'tr =: bx y iy'
assert. r -: tr [ 01 [  'tr =: bx y iy'
assert. (2;xyzs;allopred) checkallosize 7!:2 'tr =: bx y dy'
assert. r -: tr [ 02 [  'tr =: bx y dy'
assert. (3;xyzs;allopred) checkallosize 7!:2 'tr =: ix y by'
assert. r -: tr [ 03 [ 'tr =: ix y by'
assert. (4;xyzs;allopred) checkallosize 7!:2 'tr =: ix y iy'
assert. r -: tr [ 04 [ 'tr =: ix y iy'
assert. (5;xyzs;allopred) checkallosize 7!:2 'tr =: ix y dy'
assert. r -: tr [ 05 [ 'tr =: ix y dy'
assert. (6;xyzs;allopred) checkallosize 7!:2 'tr =: dx y by'
assert. r -: tr [ 06 [ 'tr =: dx y by'
assert. (7;xyzs;allopred) checkallosize 7!:2 'tr =: dx y iy'
assert. r -: tr [ 07 [ 'tr =: dx y iy'
assert. (8;xyzs;allopred) checkallosize 7!:2 'tr =: dx y dy'
assert. r -: tr [ 08 [ 'tr =: dx y dy'
NB. Verify nothing modified in place
assert. svxy -: 3!:1&.> bx;by;ix;iy;dx;dy
NB. Make x inplaceable.  Verify correct result
tx =: 3!:2 (3!:1) bx
assert. (9;xyzs;allopred) checkallosize 7!:2 'tx =: tx y by'
assert. r -: tx [ 10 [ 'tx =: tx y by'
tx =: 3!:2 (3!:1) bx
assert. (10;xyzs;allopred) checkallosize 7!:2 'tx =: tx y iy'
assert. r -: tx [ 11 [ 'tx =: tx y iy'
tx =: 3!:2 (3!:1) bx
assert. (11;xyzs;allopred) checkallosize 7!:2 'tx =: tx y dy'
assert. r -: tx [ 12 [ 'tx =: tx y dy'
tx =: 3!:2 (3!:1) ix
assert. (12;xyzs;allopred) checkallosize 7!:2 'tx =: tx y by'
assert. r -: tx [ 13 [ 'tx =: tx y by'
tx =: 3!:2 (3!:1) ix
assert. (13;xyzs;allopred) checkallosize 7!:2 'tx =: tx y iy'
assert. r -: tx [ 14 [ 'tx =: tx y iy'
tx =: 3!:2 (3!:1) ix
assert. (14;xyzs;allopred) checkallosize 7!:2 'tx =: tx y dy'
assert. r -: tx [ 15 [ 'tx =: tx y dy'
tx =: 3!:2 (3!:1) dx
assert. (15;xyzs;allopred) checkallosize 7!:2 'tx =: tx y by'
assert. r -: tx [ 16 [ 'tx =: tx y by'
tx =: 3!:2 (3!:1) dx
assert. (16;xyzs;allopred) checkallosize 7!:2 'tx =: tx y iy'
assert. r -: tx [ 17 [ 'tx =: tx y iy'
tx =: 3!:2 (3!:1) dx
assert. (17;xyzs;allopred) checkallosize 7!:2 'tx =: tx y dy'
assert. r -: tx [ 18 [ 'tx =: tx y dy'
NB. Verify originals not modified in place
assert. svxy -: 3!:1&.> bx;by;ix;iy;dx;dy
NB. Make y inplaceable.  Verify correct result
ty =: 3!:2 (3!:1) by
assert. (18;xyzs;allopred) checkallosize 7!:2 'ty =: bx y ty'
assert. r -: ty [ 20 [ 'ty =: bx y ty'
ty =: 3!:2 (3!:1) iy
assert. (19;xyzs;allopred) checkallosize 7!:2 'ty =: bx y ty'
assert. r -: ty [ 21 [ 'ty =: bx y ty'
ty =: 3!:2 (3!:1) dy
assert. (20;xyzs;allopred) checkallosize 7!:2 'ty =: bx y ty'
assert. r -: ty [ 22 [ 'ty =: bx y ty'
ty =: 3!:2 (3!:1) by
assert. (21;xyzs;allopred) checkallosize 7!:2 'ty =: ix y ty'
assert. r -: ty [ 23 [ 'ty =: ix y ty'
ty =: 3!:2 (3!:1) iy
assert. (22;xyzs;allopred) checkallosize 7!:2 'ty =: ix y ty'
assert. r -: ty [ 24 [ 'ty =: ix y ty'
ty =: 3!:2 (3!:1) dy
assert. (23;xyzs;allopred) checkallosize 7!:2 'ty =: ix y ty'
assert. r -: ty [ 25 [ 'ty =: ix y ty'
ty =: 3!:2 (3!:1) by
assert. (24;xyzs;allopred) checkallosize 7!:2 'ty =: dx y ty'
assert. r -: ty [ 26 [ 'ty =: dx y ty'
ty =: 3!:2 (3!:1) iy
assert. (25;xyzs;allopred) checkallosize 7!:2 'ty =: dx y ty'
assert. r -: ty [ 27 [ 'ty =: dx y ty'
ty =: 3!:2 (3!:1) dy
assert. (26;xyzs;allopred) checkallosize 7!:2 'ty =: dx y ty'
assert. r -: ty [ 28 [ 'ty =: dx y ty'
NB. Verify originals not modified in place
assert. svxy -: 3!:1&.> bx;by;ix;iy;dx;dy
end.

if. 'I' e. resvalidity do.
NB. Test integer data, in integer and float precisions

NB. Create random data.  This will not generate overflow so we don't have to worry about unreproducible space requirements
ix =: xs ?@$ IF64 { 30000 1000000
iy =: ys ?@$ IF64 { 30000 1000000
NB. Make the first value 0.  This creates 0|0 which goes through a different path in the code; we need to do so predictably
if. #,ix do. ix =: 0 0:} ix end.
if. #,iy do. iy =: 0 0:} iy end.
NB. Create copies in other precisions
dx =: ix + (2.5-2.5)
dy =: iy + (2.5-2.5)
NB. Save all precisions in a separate area
'svix sviy svdx svdy' =: svxy =: 3!:1&.> ix;iy;dx;dy
NB. Get result with all looping performed in rank conjunction
r =: ix y"y"y iy
NB. For each precision combination, verify identical result
assert. (4;xyzs;allopred) checkallosize 7!:2 'tr =: ix y iy'
assert. r -: tr [ 30 [ 'tr =: ix y iy'
assert. (5;xyzs;allopred) checkallosize 7!:2 'tr =: ix y dy'
assert. r -: tr [ 31 [ 'tr =: ix y dy'
assert. (7;xyzs;allopred) checkallosize 7!:2 'tr =: dx y iy'
assert. r -: tr [ 32 [ 'tr =: dx y iy'
assert. (8;xyzs;allopred) checkallosize 7!:2 'tr =: dx y dy'
assert. r -: tr [ 33 [ 'tr =: dx y dy'
NB. Verify nothing modified in place
assert. svxy -: 3!:1&.> ix;iy;dx;dy
NB. Make x inplaceable.  Verify correct result
tx =: 3!:2 (3!:1) ix
assert. (13;xyzs;allopred) checkallosize 7!:2 'tx =: tx y iy'
assert. r -: tx [ 40 [ 'tx =: tx y iy'
tx =: 3!:2 (3!:1) ix
assert. (14;xyzs;allopred) checkallosize 7!:2 'tx =: tx y dy'
assert. r -: tx [ 41 [ 'tx =: ix y dy'
tx =: 3!:2 (3!:1) dx
assert. (16;xyzs;allopred) checkallosize 7!:2 'tx =: tx y iy'
assert. r -: tx [ 42 [ 'tx =: tx y iy'
tx =: 3!:2 (3!:1) dx
assert. (17;xyzs;allopred) checkallosize 7!:2 'tx =: tx y dy'
assert. r -: tx [ 43 [ 'tx =: tx y dy'
NB. Verify originals not modified in place
assert. svxy -: 3!:1&.> ix;iy;dx;dy
NB. Make y inplaceable.  Verify correct result
ty =: 3!:2 (3!:1) iy
assert. (22;xyzs;allopred) checkallosize 7!:2 'ty =: ix y ty'
assert. r -: ty [ 50 [ 'ty =: ix y ty'
ty =: 3!:2 (3!:1) dy
assert. (23;xyzs;allopred) checkallosize 7!:2 'ty =: ix y ty'
assert. r -: ty [ 51 [ 'ty =: ix y ty'
ty =: 3!:2 (3!:1) iy
assert. (25;xyzs;allopred) checkallosize 7!:2 'ty =: dx y ty'
assert. r -: ty [ 52 [ 'ty =: dx y ty'
ty =: 3!:2 (3!:1) dy
assert. (26;xyzs;allopred) checkallosize 7!:2 'ty =: dx y ty'
assert. r -: ty [ 53 [ 'ty =: dx y ty'
NB. Verify originals not modified in place
assert. svxy -: 3!:1&.> ix;iy;dx;dy
end.

if. 'O' e. resvalidity do.
NB. Test integer data, in integer and float precisions, with overflow

NB. Create random data.
ix =: xs (?@$ - <.@:-:@:]) IF64 { 30000 1000000
iy =: ys (?@$ - <.@:-:@:]) IF64 { 30000 1000000
NB. Install random overflow values, with enough coverage to exercise the paths; say 10% of each argument, both IMAX and IMIN
IMAX =. _1 - IMIN =. <. - 2 ^ IF64 { 31 63
if. #ixx =. ($ix) <@#: I. 0.10 > ?@$&0 */ $ ix do.
 ix =: (IMAX - (#ixx) ?@$ 10) ixx} ix
elseif. '' -: $ix do. ix =: IMIN
end.
if. #iyx =: ($iy) <@#: I. 0.10 > ?@$&0 */ $ iy do.
 iy =: (IMIN + (#iyx) ?@$ 10) iyx} iy
elseif. '' -: $iy do. iy =: IMAX
end.
NB. Make the first value 0.  This creates 0|0 which goes through a different path in the code; we need to do so predictably
if. #,ix do. ix =: 0 0:} ix end.
if. #,iy do. iy =: 0 0:} iy end.
NB. Create copies in other precisions
dx =: ix + (2.5-2.5)
dy =: iy + (2.5-2.5)
NB. Save all precisions in a separate area
'svix sviy svdx svdy' =: svxy =: 3!:1&.> ix;iy;dx;dy
NB. Get result with all looping performed in rank conjunction
r =: ix y"y"y iy
NB. For each precision combination, verify reasonable result.  Because we convert to float,
NB. IMAX+IMIN may lose precision if INTs are 64-bit.  We are mainly making sure we don't
NB. turn IMIN to IMAX or vice versa
fuzz =. IF64 { 8 16384 
assert. (4;xyzs;allopred) checkallosize 7!:2 'tr =: ix y iy'
assert. *./ , fuzz > | r - tr [ 60 [ 'tr =: ix y iy'
assert. (5;xyzs;allopred) checkallosize 7!:2 'tr =: ix y dy'
assert. *./ , fuzz > | r - tr [ 61 [ 'tr =: ix y dy'
assert. (7;xyzs;allopred) checkallosize 7!:2 'tr =: dx y iy'
assert. *./ , fuzz > | r - tr [ 62 [ 'tr =: dx y iy'
assert. (8;xyzs;allopred) checkallosize 7!:2 'tr =: dx y dy'
assert. *./ , fuzz > | r - tr [ 63 [ 'tr =: dx y dy'
NB. Verify nothing modified in place
assert. svxy -: 3!:1&.> ix;iy;dx;dy
NB. Make x inplaceable.  Verify correct result
tx =: 3!:2 (3!:1) ix
assert. (13;xyzs;allopred) checkallosize 7!:2 'tx =: tx y iy'
assert. *./ , fuzz > | r - tx [ 70 [ 'tx =: tx y iy'
tx =: 3!:2 (3!:1) ix
assert. (14;xyzs;allopred) checkallosize 7!:2 'tx =: tx y dy'
assert. *./ , fuzz > | r - tx [ 71 [ 'tx =: tx y dy'
tx =: 3!:2 (3!:1) dx
assert. (16;xyzs;allopred) checkallosize 7!:2 'tx =: tx y iy'
assert. *./ , fuzz > | r - tx [ 72 [ 'tx =: tx y iy'
tx =: 3!:2 (3!:1) dx
assert. (17;xyzs;allopred) checkallosize 7!:2 'tx =: tx y dy'
assert. *./ , fuzz > | r - tx [ 73 [ 'tx =: tx y dy'
NB. Verify originals not modified in place
assert. svxy -: 3!:1&.> ix;iy;dx;dy
NB. Make y inplaceable.  Verify correct result
ty =: 3!:2 (3!:1) iy
assert. (22;xyzs;allopred) checkallosize 7!:2 'ty =: ix y ty'
assert. *./ , fuzz > | r - ty [ 80 [ 'ty =: ix y ty'
ty =: 3!:2 (3!:1) dy
assert. (23;xyzs;allopred) checkallosize 7!:2 'ty =: ix y ty'
assert. *./ , fuzz > | r - ty [ 81 [ 'ty =: ix y ty'
ty =: 3!:2 (3!:1) iy
assert. (25;xyzs;allopred) checkallosize 7!:2 'ty =: dx y ty'
assert. *./ , fuzz > | r - ty [ 82 [ 'ty =: dx y ty'
ty =: 3!:2 (3!:1) dy
assert. (26;xyzs;allopred) checkallosize 7!:2 'ty =: dx y ty'
assert. *./ , fuzz > | r - ty [ 83 [ 'ty =: dx y ty'
NB. Verify originals not modified in place
assert. svxy -: 3!:1&.> ix;iy;dx;dy
end.

if. 'D' e. resvalidity do.
NB. Test float data, in float precision

NB. Create random data.  This will not generate overflow so we don't have to worry about unreproducible space requirements
dx =: xs ?@$ 0
dy =: ys ?@$ 0
NB. Save all precisions in a separate area
'svdx svdy' =: svxy =: 3!:1&.> dx;dy
NB. Get result with all looping performed in rank conjunction
r =: dx y"y"y dy
NB. For each precision combination, verify identical result
assert. (8;xyzs;allopred) checkallosize 7!:2 'tr =: dx y dy'
assert. r -: tr [ 90 [ 'tr =: dx y dy'
NB. Verify nothing modified in place
assert. svxy -: 3!:1&.> dx;dy
NB. Make x inplaceable.  Verify correct result
tx =: 3!:2 (3!:1) dx
assert. (17;xyzs;allopred) checkallosize 7!:2 'tx =: tx y dy'
assert. r -: tx [ 91 [ 'tx =: tx y dy'
NB. Verify originals not modified in place
assert. svxy -: 3!:1&.> dx;dy
NB. Make y inplaceable.  Verify correct result
ty =: 3!:2 (3!:1) dy
assert. (26;xyzs;allopred) checkallosize 7!:2 'ty =: dx y ty'
assert. r -: ty [ 92 [ 'ty =: dx y ty'
NB. Verify originals not modified in place
assert. svxy -: 3!:1&.> dx;dy
end.

1
)

NB. u is verb, y is (result precisions to test, string incl BDI),(buffer-allocation predictions)
NB. We try the verb on different ranks and shapes
testinplacer =: 1 : 0
'resultprec predr' =: y
NB. Test for validity.  Checks for inplacing too much
NB. Choose a random shape, then try all combinations of prefixes.
if. 'V' e. resultprec do.
  ((u f.) testinplace (resultprec;''))"1 ,"0/~ a: , <\ >: ? 4 # 10
  NB. Repeat for other ranks of u.  Also create a random cell-shape and use a prefix of it for each argument
  asrank =: 1 : ']"m'  NB. as a verb-rank, m can always be applied
  ((u f.)"(0 0 asrank) testinplace (resultprec;''))"1 (0 0) (] ,&.> [ {.&.> [: < 10 ?@$~ >./@[)"1   ,"0/~ a: , <\ >: ? 2 # 10
  ((u f.)"(0 1 asrank) testinplace (resultprec;''))"1 (0 1) (] ,&.> [ {.&.> [: < 10 ?@$~ >./@[)"1   ,"0/~ a: , <\ >: ? 2 # 10
  ((u f.)"(1 0 asrank) testinplace (resultprec;''))"1 (1 0) (] ,&.> [ {.&.> [: < 10 ?@$~ >./@[)"1   ,"0/~ a: , <\ >: ? 2 # 10
  ((u f.)"(1 1 asrank) testinplace (resultprec;''))"1 (1 1) (] ,&.> [ {.&.> [: < 10 ?@$~ >./@[)"1   ,"0/~ a: , <\ >: ? 3 # 10
  ((u f.)"(0 2 asrank) testinplace (resultprec;''))"1 (0 2) (] ,&.> [ {.&.> [: < 10 ?@$~ >./@[)"1   ,"0/~ a: , <\ >: ? 3 # 10
  ((u f.)"(1 2 asrank) testinplace (resultprec;''))"1 (1 2) (] ,&.> [ {.&.> [: < 10 ?@$~ >./@[)"1   ,"0/~ a: , <\ >: ? 3 # 10
  ((u f.)"(2 2 asrank) testinplace (resultprec;''))"1 (2 2) (] ,&.> [ {.&.> [: < 10 ?@$~ >./@[)"1   ,"0/~ a: , <\ >: ? 2 # 10
  ((u f.)"(2 1 asrank) testinplace (resultprec;''))"1 (2 1) (] ,&.> [ {.&.> [: < 10 ?@$~ >./@[)"1   ,"0/~ a: , <\ >: ? 2 # 10
  ((u f.)"(2 0 asrank) testinplace (resultprec;''))"1 (2 0) (] ,&.> [ {.&.> [: < 10 ?@$~ >./@[)"1   ,"0/~ a: , <\ >: ? 2 # 10
  ((u f.)"(0 3 asrank) testinplace (resultprec;''))"1 (0 3) (] ,&.> [ {.&.> [: < 10 ?@$~ >./@[)"1   ,"0/~ a: , <\ >: ? 2 # 10
  ((u f.)"(1 3 asrank) testinplace (resultprec;''))"1 (1 3) (] ,&.> [ {.&.> [: < 10 ?@$~ >./@[)"1   ,"0/~ a: , <\ >: ? 2 # 10
  ((u f.)"(2 3 asrank) testinplace (resultprec;''))"1 (2 3) (] ,&.> [ {.&.> [: < 10 ?@$~ >./@[)"1   ,"0/~ a: , <\ >: ? 2 # 10
  ((u f.)"(3 3 asrank) testinplace (resultprec;''))"1 (3 3) (] ,&.> [ {.&.> [: < 10 ?@$~ >./@[)"1   ,"0/~ a: , <\ >: ? 2 # 10
  ((u f.)"(3 2 asrank) testinplace (resultprec;''))"1 (3 2) (] ,&.> [ {.&.> [: < 10 ?@$~ >./@[)"1   ,"0/~ a: , <\ >: ? 2 # 10
  ((u f.)"(3 1 asrank) testinplace (resultprec;''))"1 (3 1) (] ,&.> [ {.&.> [: < 10 ?@$~ >./@[)"1   ,"0/~ a: , <\ >: ? 2 # 10
  ((u f.)"(3 0 asrank) testinplace (resultprec;''))"1 (3 0) (] ,&.> [ {.&.> [: < 10 ?@$~ >./@[)"1   ,"0/~ a: , <\ >: ? 2 # 10
end.

NB. Verify size of large operands.  Checks for inplacing too little
(u f.) testinplace (resultprec;(predr -. 'b'))"1 (16000;16000)  NB. 'b' means inplace only if rank not specified
(u f.)"1 testinplace (resultprec;predr)"1 (10 1600;10 1600)
)
NB. Full test with everything allowed
9!:53 (2)

+. testinplacer 'VBID';'B/lI/L/rI/I/L/R/R/D/    /lI/L/rI/I/L/R/R//   /lI/L/rI/I/L/R/R//'
*. testinplacer 'VBID';'B/lI/L/rI/I/L/R/R/D/    /lI/L/rI/I/L/R/R//   /lI/L/rI/I/L/R/R//'

27 b. testinplacer 'VBI';'lr/l/lr/r/I/r/lr/l/lr/    lr/l/lr/r//r/lr/l/lr/   lr/l/lr/r//r/lr/l/lr/'

! testinplacer 'VBID';'B/LRI/L/LRI/LRI/L/R/R/D/    /LRI/L/LRI/LRI/L/R/R//   /LRI/L/LRI/LRI/L/R/R//'

% testinplacer 'VBID';'D/D/D/D/D/D/D/D/D/    D/D/D/d/d/d////   D/d//D/d//D/d//'

* testinplacer 'VBID';'B/I/D/I/I/D/D/D/D/    /I/D///d////   ///I///D/d//'  NB. non-overflow
* testinplacer 'VO';'B/I/D/I/Id/D/D/D/D/    /I/D//d/d////   ///I/d//D/d//'  NB. overflow
NB. mulinasm * testinplacer 'VBID';'B/I/D/I/I/D/D/D/D/    b/I/D//I/d////   b///I/I//D/d//'  NB. non-overflow
NB. mulinasm * testinplacer 'VO';'B/I/D/I/ID/D/D/D/D/    b/I/D//ID/d////   b///I/ID//D/d//'  NB. overflow

+ testinplacer 'VBID';'I/I/D/I/I/D/D/D/D/    I/I/D///d////   I///I///D/d//'  NB. non-overflow
+ testinplacer 'VO';'I/Id/D/Id/Id/D/D/D/D/    I/Id/D/d/d/d////   I/d//Id/d//D/d//'   NB. overflow
- testinplacer 'VBID';'I/I/D/I/I/D/D/D/D/    I/I/D///d////   I///I///D/d//'
- testinplacer 'VO';'I/Id/D/Id/Id/D/D/D/D/    I/Id/D/d/d/d////   I/d//Id/d//D/d//'   NB. overflow

< testinplacer 'VBID';'B/B/B/B/B/B/B/B/B/    ///B/B/B/B/B/B/   /B/B//B/B//B/B/'
= testinplacer 'VBID';'B/B/B/B/B/B/B/B/B/    ///B/B/B/B/B/B/   /B/B//B/B//B/B/'
> testinplacer 'VBID';'B/B/B/B/B/B/B/B/B/    ///B/B/B/B/B/B/   /B/B//B/B//B/B/'
<: testinplacer 'VBID';'B/B/B/B/B/B/B/B/B/    ///B/B/B/B/B/B/   /B/B//B/B//B/B/'
>: testinplacer 'VBID';'B/B/B/B/B/B/B/B/B/    ///B/B/B/B/B/B/   /B/B//B/B//B/B/'
~: testinplacer 'VBID';'B/B/B/B/B/B/B/B/B/    ///B/B/B/B/B/B/   /B/B//B/B//B/B/'

+: testinplacer 'VB';'B/q/q/k/kq/kq/k/kq/kq/    /q/q/k/kq/kq/k/kq/kq/   /q/q/k/kq/kq/k/kq/kq/'
*: testinplacer 'VB';'B/q/q/k/kq/kq/k/kq/kq/    /q/q/k/kq/kq/k/kq/kq/   /q/q/k/kq/kq/k/kq/kq/'

NB. INT ^ INT produces slightly different values than INT ^ FL on 902753 ^ 39  which is close to the IEEE limit.  So don't check those values
^ testinplacer 'VBD';'B/D/D/I/D/D/D/D/D/    /D/D/I/D/D/D/D/D/   /D/D/I/D/D/D/D/D/'

NB. 0|0 allocates an extra FL output buffer.  We ensure that we go through this code
| testinplacer 'VBID';'B/l/L/r/I/ID/R/R/D/    /l/L/r//ID/R/R//   /l/L/r//ID/R/R//'  NB. I | D fails, then runs in-place

<. testinplacer 'VBID';'B/I/D/I/I/D/D/D/D/    /I/D///d////   ///I///D/d//'
>. testinplacer 'VBID';'B/I/D/I/I/D/D/D/D/    /I/D///d////   ///I///D/d//'

1: 0 : 0  NB. partial execution is always allowed now
NB. Go back and recheck with partial execution not allowed.  No need for checking overlap again, or binary, or overflow
NB. Note that the flags here have not been updated for inplace booleans, because we don't test booleans here
9!:53 (1)
+. testinplacer 'ID';'B/lI/L/rI/I/L/R/R/D/    b/lI/L/rI/I/L/R/R/D/   b/lI/L/rI/I/L/R/R/D/'
*. testinplacer 'ID';'B/lI/L/rI/I/L/R/R/D/    b/lI/L/rI/I/L/R/R/D/   b/lI/L/rI/I/L/R/R/D/'

27 b. testinplacer 'I';'lr/l/lr/r/I/r/lr/l/lr/    lr/l/lr/r//r/lr/l/lr/   lr/l/lr/r//r/lr/l/lr/'

! testinplacer 'ID';'B/LRI/L/LRI/LRI/L/R/R/D/    b/LRI/L/LRI/LRI/L/R/R//   b/LRI/L/LRI/LRI/L/R/R//'

% testinplacer 'ID';'D/D/D/D/D/D/D/D/D/    D/D/D/d/d/d///D/   D/d//D/d//D/d/D/'

* testinplacer 'ID';'B/I/D/I/I/D/D/D/D/    b/I/D///d////   b///I///D/d//'
NB. mulinasm * testinplacer 'ID';'B/I/D/I/I/D/D/D/D/    b/I/D//I/d////   b///I/I//D/d//'

+ testinplacer 'ID';'I/I/D/I/I/D/D/D/D/    I/I/D///d///D/   I///I///D/d/D/'
- testinplacer 'ID';'I/I/D/I/I/D/D/D/D/    I/I/D///d///D/   I///I///D/d/D/'

< testinplacer 'ID';'B/B/B/B/B/B/B/B/B/    b/b/b/B/B/B/B/B/B/   b/B/B/b/B/B/b/B/B/'
= testinplacer 'ID';'B/B/B/B/B/B/B/B/B/    b/b/b/B/B/B/B/B/B/   b/B/B/b/B/B/b/B/B/'
> testinplacer 'ID';'B/B/B/B/B/B/B/B/B/    b/b/b/B/B/B/B/B/B/   b/B/B/b/B/B/b/B/B/'
<: testinplacer 'ID';'B/B/B/B/B/B/B/B/B/    b/b/b/B/B/B/B/B/B/   b/B/B/b/B/B/b/B/B/'
>: testinplacer 'ID';'B/B/B/B/B/B/B/B/B/    b/b/b/B/B/B/B/B/B/   b/B/B/b/B/B/b/B/B/'
~: testinplacer 'ID';'B/B/B/B/B/B/B/B/B/    b/b/b/B/B/B/B/B/B/   b/B/B/b/B/B/b/B/B/'

NB. INT ^ INT produces slightly different values than INT ^ FL on 902753 ^ 39  which is close to the IEEE limit.  So don't check those values
^ testinplacer 'D';'B/D/D/I/D/D/D/D/D/    b/D/D/I/D/D/D/D/D/   b/D/D/I/D/D/D/D/D/'

NB. 0|0 allocates an extra FL output buffer.  We ensure that we go through this code
| testinplacer 'ID';'B/l/L/r/I/ID/R/R/D/    b/l/L/r//ID/R/R/D/   b/l/L/r//ID/R/R/D/'  NB. I | D fails, then runs in-place

<. testinplacer 'ID';'B/I/D/I/I/D/D/D/D/    b/I/D///d////   b///I///D/d//'
>. testinplacer 'ID';'B/I/D/I/I/D/D/D/D/    b/I/D///d////   b///I///D/d//'
)

9!:53 (1)   NB. Reset the default

NB. Verify that cells are reset after being modified; here, type is changed by divide
(% i. 4 5) -: (2 % ])"1 +: i. 4 5
(% i. 4 5) -: 2 ([ % ])"1 +: i. 4 5
(% i. 4 5) -: (+: i. 4 5) (] % [)"1 (2)
(%/"1 |: +: i. 4 5) -: ([ % ])/ +: i. 4 5
(%~/"1 |: +: i. 4 5) -: ([ % ])~/ +: i. 4 5

NB. Verify result of explicit def is inplaceable
(2200000 * IF64 { 4 8) > 7!:2 '2 + 3 : ''+: y'' i. 1000000'

NB. Verify that result of explicit def is not virtual.  This crashes if it is, because the backer gets freed
p1=: 3 : 0
rt=. 0$0
rt=. rt, 1:^:(0=]) p2 ''
''
)
p2=: 3 : 0
''&$@:, 0{:: rc=. ,< p3''  NB. Result of this expression is virtual
)
p3 =: 3 : '103 + 0'  NB. returns inplaceable value
p1''
p1''
p1''
p1''
p1''   NB. Fails on the second call



4!:55 ;:'adot1 adot2 asrank sdot0 allobytes allopred atomct atomsz bx by bytesused checkallosize dx dy ix iy iyx p1 p2 p3 pred'
4!:55 ;:'predflds predr r resultprec resvalidity sel shapes sn spred svbx svby svdx svdy'
4!:55 ;:'svix sviy svxy testinplace testinplacer tr tx ty vb xs xyzs ys IMIN IMAX'
randfini''


epilog''

