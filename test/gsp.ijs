NB. $. ------------------------------------------------------------------

s=: $. x=: 7 3 5 ?@$ 5
t=: $. y=: 7 3 5 ?@$ 5

(s=t) -: x=y
    
'nonce error' -: < etx s
(s<t) -: x<y

(<.s) -: <.x
(s<.t) -: x<.y

(<:s) -: <:x
(s<:t) -: x<:y

(>s) -: >x
(s>t) -: x>y
  
(>.s) -: >.x
(s>.t) -: x>.y

(>:s) -: >:x
(s>:t) -: x>:y

(+s) -: +x
(s+t) -: x+y

(+.s) -: +.x
(s+.t) -: x+.y

(+:s) -: +:x
((2|s)+:(2|t)) -: (2|x)+:2|y

(*s) -: *x
(s*t) -: x*y

(*.s) -: *.x
(s*.t) -: x*.y

(*:s) -: *:x
((2|s)*:(2|t)) -: (2|x)*:2|y

(-s) -: -x
(s-t) -: x-y

(-.s) -: -.x

(-:s) -: -:x
s -: x

(%s) -: %x
(s%t) -: x%y

NB.  %. CDOMINO, VERB, minv,    mdiv,   

(%:s) -: %:x
(s%:t) -: x%:y

(^s) -: ^x
(s^t) -: x^y

(^.s) -: ^.x
'NaN error' -: s^. etx t

NB.  ^: CPOWOP,  CONJ, 0L,      powop,  
NB.  $  CDOLLAR, VERB, shape,   reitem, 
NB.  $. CSPARSE, VERB, sparse1, sparse2,
NB.  $: CSELF,   VERB, self1,   self2,  
NB.  ~  CTILDE,  ADV,  swap,    0L,  
   
(s~:t) -: x~:y
    
(|s) -: |x
(s|t) -: x|y

(|.s) -: |.x
(3|.s) -: 3|.x

NB.  |: CCANT,   VERB, cant1,   cant2,  
NB.  .  CDOT,    CONJ, 0L,      dot,    
NB.  .. CEVEN,   CONJ, 0L,      even,   
NB.  .: CODD,    CONJ, 0L,      odd,    
NB.  :  CCOLON,  CONJ, 0L,      colon,  
NB.  :. COBVERSE,CONJ, 0L,      obverse,
NB.  :: CADVERSE,CONJ, 0L,      adverse,

(,s) -: ,x
(s,t) -: x,y

(,.s) -: ,.x
(s,.t) -: x,.y

(,: s) -: ,: x
(s,:t) -: x,:y

(;s) -: ;x
'nonce error' -: s ; etx t

NB.  ;. CCUT,    CONJ, 0L,      cut,    
NB.  ;: CWORDS,  VERB, words,   0L,  

(#s) -: #x
(3#s) -: 3#x

'nonce error' -: #. etx t
'nonce error' -: 3 #. etx t
'nonce error' -: s #. etx 3
'nonce error' -: s #. etx t

'nonce error' -: #: etx t

'nonce error' -: 3 #: etx t
'nonce error' -: s #: etx 3
'nonce error' -: s #: etx t

(!s) -: !x
(s!t) -: x!y

NB.  !. CFIT,    CONJ, 0L,      fit,    
NB.  !: CIBEAM,  CONJ, 0L,      foreign,

(+/s) -: +/x
NB. (s +/ t) -: x +/ y

NB.  /. CSLDOT,  ADV,  sldot,   0L,     
NB.  /: CGRADE,  VERB, grade1,  grade2, 
NB.  \  CBSLASH, ADV,  bslash,  0L,     
NB.  \. CBSDOT,  ADV,  bsdot,   0L,     
NB.  \: CDGRADE, VERB, dgrade1, dgrade2,

([s) -: [x
(s[t) -: x[y

NB.  [: CCAP,    VERB, 0L,      0L,    

(]s) -: ]x
(s]t) -: x]y

'nonce error' -: { etx s
(i{s) -: i{x [ i=: <"1 ?(2 3,#$s)$$s 

({.s) -: {.x
(3 4{.s) -: 3 4{.x

({:s) -: {:x

'nonce error' -: i} etx s [ i=: ?(}.$s)$#s
(33 (<"1 i)}s) -: 33 (<"1 i)}x [ i=: ?(7,#$s)$$s

(}.s) -: }.x
(5 1}.s) -: 5 1}.x

(}:s) -: }:x
   
NB.  "  CQQ,     CONJ, 0L,      qq,     
NB.  ". CEXEC,   VERB, exec1,   exec2,  
NB.  ": CTHORN,  VERB, thorn1,  thorn2, 
NB.  `  CGRAVE,  CONJ, 0L,      tie,    
NB.  `: CGRCO,   CONJ, 0L,      evger,  
NB.  @  CAT,     CONJ, 0L,      atop,   
NB.  @. CATDOT,  CONJ, 0L,      agenda, 
NB.  @: CATCO,   CONJ, 0L,      atco,   
NB.  &  CAMP,    CONJ, 0L,      amp,

'nonce error' -:   $.&.> etx 1 2;3 4 5
'nonce error' -: 1 $.&.> etx 1 2;3 4 5

NB.  &: CAMPCO,  CONJ, 0L,      ampco,  
  
NB. 'domain error' -:   ?  etx 1+s
NB. 'domain error' -: 2 ?  etx 2+s

NB. 'domain error' -:   ?. etx 1+s
NB. 'domain error' -: 2 ?. etx 2+s

NB. {:: CFETCH,  VERB, map,     fetch,  
NB. }:: CEMEND,  ADV,  emend,   0L,     
NB.  A. CATOMIC, VERB, adot1,   adot2,  
NB.  b. CBDOT,   ADV,  bdot,    0L,     
NB.  c. CEIGEN,  VERB, eig1,    eig2,   
NB.  C. CCYCLE,  VERB, cdot1,   cdot2,  
NB.  d. CDDOT,   CONJ, 0L,      ddot,   
NB.  D. CDCAP,   CONJ, 0L,      dcap,   
NB.  D: CDCAPCO, CONJ, 0L,      dcapco, 

'nonce error' -: e. etx t

NB.  E. CEBAR,   VERB, 0L,      ebar,   
NB.  f. CFIX,    ADV,  fix,     0L,     
NB.  H. CHGEOM,  CONJ, 0L,      hgeom,  

(i.2 3) -: i. $. 2 3

(i:2 3) -: i: $. 2 3

NB.  I. CICAP,   ADV,  icap,    0L,     

(j.s) -: j. x
(s j. t) -: x j. y

NB.  L. CLDOT,   VERB, level1,  0,

'nonce error' -:   $. L: 0 etx 1 2 ;3 4 5  
'nonce error' -: 1 $. L: 0 etx 1 2 ;3 4 5  

(o.s) -: o.x
(1 o. s) -: 1 o. x
(2 o. s) -: 2 o. x
  
NB.  p. CPOLY,   VERB, poly1,   poly2,  
NB.  p: CPCO,    VERB, prime,   0,      
NB.  q: CQCO,    VERB, factor,  qcol2,  

(r.s) -: r. x
(s r. t) -: x r. y

NB.  S: CSCO,    CONJ, 0L,      sco,    
NB.  t. CTDOT,   ADV,  tdot,    0L,     
NB.  t: CTCO,    ADV,  tco,     0L,     
NB.  T. CTCAP,   CONJ, 0L,      tcap,  
 
'nonce error' -: x: etx s
'nonce error' -: 1 x: etx s
'nonce error' -: 2 x: etx s


4!:55 ;:'i s t x y'


