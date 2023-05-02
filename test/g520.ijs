prolog './g520.ijs'


NB. 128!:9  g;i;v;M;thresh ---------------------------------------------------------

NB. y is 128!:9 parms, x is expected result (empty if we have to check column values).  m is parms
NB. Save args, run 128!:9, check result.  For multithreads, expand M/bk/Frow to big enough to engage threading (leaving last row/col harmless)
NB. and expand bkg and ndx to  make enough work to keep the threads busy
run128_9 =: {{
epadd =. (|:~ (_1 |. i.@#@$)) @: ((1.0"0 +/@:*"1!.1 ])@,"1&(0&|:))
epsub =. (epadd -)
savx =: x [ savy =: y
nthr =. 1 T. ''
opts =. m
if. '' -: $ 0 {::y do.
  NB. onecol
  if. #x do.  NB. compare value given, use it
    if. 0=nthr do.  NB. compare values only in single-threaded mode
      assert. x -: 128!:9 y
    end.
  else.  NB. random values, compare results.  column must be in A
    if. 0~:nthr do.   NB. multiple threads: expand M with 0s to make size threadable, scramble rows in the data
      expsiz =. 1200 + ? 50  NB. size we grow M to
      y =. (< (0{:: y) + expsiz - {:$4{::y) 0} y  NB. adjust ndx to match column# in A we are calculating
      y =. (< ({"2~ ?~@{:@$) (2 , 2 # expsiz) {. 4 {:: y) 4} y  NB. expand M and scramble rows
      savy =: y
    end.
    savref   =: ref =. |: (,~ 3{::y) +/@:*"1!.1 ,.~/ (2{::y){"1 (4){::y 
    assert. 1e_25 > >./ | +/ ref epsub savres   =: 128!:9 y
  end.
elseif. 10 13 e.~ #y do.  NB. DIP/gradient
  if. 8 = $ 5 {:: y do.  NB. DIP
    if. 0=nthr do.  NB. single-threaded, compare for exact result
      if. 10 = #y do.
        y =. y , (0$0.) ; ($&0. ; $&'4') # 8 {:: y  NB. if boundbk;beta;rvt  omitted, supply it
        y =. (< (6{::y) {"1 (7){:: y) 7} y   NB. convert bk to be read from the front
      end.
      savy =: y
      assert. x -: savres =:  128!:9 y
    else.  NB. multiple threads.  Expand M with one harmless row/col, bk/Frow to match; pad ndx and bkg, preserving order, to make work
      if. -. opts -: 0 do.  NB. opts of 0 means single thread only
        'ndx M bkg bk Frow cons' =. 0 4 6 7 8 5 { y
        prirow =. 6 { cons
        msiz =. {:$M  NB. size of M part
        M =. (0 1 1 + $M) {. (0 0 1 + $M) {.!.1e_10 M  NB. One column of small c above threshold, ending row of 0
        bk =. (0 1 + $bk) {. bk
        Frow =. (1 + $Frow) {.!.1. Frow
        'nndx nbkg' =. 35 + ? 20 20   NB. product must exceed 1000
        saferc =. <: {: $ bk  NB. last row/col doesn't change anything
        ndx =. ndx + ndx >: msiz  NB. relocate indexes in A
        ndx =. ndx (/:~ (#ndx) ? nndx)} nndx # saferc  NB. Keep prirow at head, if given, to avoid cutoff
        bkg =. bkg (0 (0})^:(prirow={.bkg) /:~ (#bkg) ? nbkg)} nbkg # saferc
        y =. (ndx;M;bkg;bk;Frow) 0 4 6 7 8} y
        if. 10 = #y do.
          y =. y , (0$0.) ; ($&0. ; $&'4') # Frow  NB. if boundbk;beta;rvt  omitted, supply it
          y =. (< (6{::y) {"1 (7){:: y) 7} y   NB. convert bk to be read from the front
        end.
        savy =: y
        if. #opts do.
          assert. opts e.~ 3 {. savres =: 128!:9 savy  NB. alternative results
        else.
          assert. x -:&(3&{.) savres =: 128!:9 savy  NB. exact match
        end.
      end.
    end.
  else.  NB. gradient mode
    if. 0=nthr do.  NB. single-threaded, compare for exact result
      if. 10 = #y do. savy =: y =. y , (0$0.) ; ($&0. ; $&'4') # 8 {:: y end.  NB. if boundbk;beta;rvt  omitted, supply it
      assert. x -: savres =: 128!:9 y
    else.  NB. multiple threads.  Expand M with one harmless row/col, Frow to match;
      if. -. opts -: 0 do.  NB. opts of 0 means single thread only
        'ndx M cons bkg bk Frow' =. 0 4 5 6 7 8 { y
        prirow =. 6 { cons
        msiz =. {:$M  NB. size of M part
        M =. (0 1 1 + $M) {. (0 0 1 + $M) {.!.0. M  NB. One column of small c above threshold, ending row of 0
        Frow =. (1 + $Frow) {.!.1e_20 Frow
NB. obsolete         bk =. (0 1 + $bk) {. bk   NB. zeros are active here, but they do nothing
        'nndx nbkg' =. 35 + ? 20 20   NB. product must exceed 1000
        saferc =. <: {: $ M  NB. last row/col doesn't change anything
        Frow =. (({:$Frow) + nbkg - {:$M) {. Frow
        M =.nbkg {."1 M  NB. unused M, but needed to match size of bk
        ndx =. ndx + ndx >: msiz  NB. relocate indexes in A
        ndx =. ndx (/:~ (#ndx) ? nndx)} nndx # saferc
NB. obsolete         bkg=.i. msiz   NB. we don't use bkg as a parameter, but we must shuffle bk
        ordpts =. /:~ (#bkg) ? nbkg
        bkg =. bkg ordpts} nbkg # saferc
        bk =. bk ordpts}"1 (nbkg _1} $bk) $ 0.0
        M =. nbkg {."2 M
        if. prirow>: 0 do. cons =. (bkg i. prirow) 6} cons end.  NB. priority row can float; find it
NB. obsolete         modx =: x =. bkg (i. 2&{)`2:`]} x  NB. replace row #s with positions in bkg
        if. 10 = #y do. y =. y , (0$0.) ; ($&0. ; $&'4') # Frow end.  NB. if boundbk;beta;rvt  omitted, supply it
        savy =: (ndx;M;cons;bkg;bk;Frow) 0 4 5 6 7 8} y
        if. #opts do.
          opts =. bkg (i. 2&{)`2:`]}"1 opts
          assert. opts e.~ 3 {. savres =: 128!:9 savy  NB. alternative results
        else.
          assert. x -:&(3&{.) savres =: 128!:9 savy  NB. exact match
        end.
      end.
    end.
  end.
else.  NB. nonimp
  'ndx An Ax Av M cons bkg' =. 7 {. y
  if. 0=nthr do.  NB. single-threaded, compare for exact result
    assert. x -: savres =: 128!:9 y
  else.  NB. multiple threads.  Expand M with one harmless row/col. pad ndx and bkg, preserving order, to make work
    if. -. opts -: 0 do.  NB. opts of 0 means single thread only
      'M bkg' =. 4 6 { y
      msiz =. {:$M  NB. size of M part
      M =. (0 1 1 + $M) {. M  NB. last row/col of 0s
      nbkg =. 2000 + ? 200   NB. product must exceed 1000
      saferc =. <: {: $ M  NB. last row/col doesn't change anything
      ndx =. ndx + ndx >: msiz  NB. relocate indexes in A
      bkg =. bkg (/:~ (#bkg) ? nbkg)} nbkg # saferc
      savres =: 128!:9 savy =: (ndx;M;bkg) 0 4 6} y
      if. #opts do.
        assert. opts e.~ 3 {. savres  NB. alternative results
      else.
        assert. x -:&(3&{.) savres  NB. exact match
      end.
    end.
  end.
end.
1
}}

{{)v
epmul =. (|:~ (_1 |. i.@#@$)) @: (((0 0 1 1{[) +/@:*"1!.1 (0 1 0 1{]))"1&(0&|:))
epadd =. (|:~ (_1 |. i.@#@$)) @: ((1.0"0 +/@:*"1!.1 ])@,"1&(0&|:))
epsub =. (epadd -)
epcanon=. (epadd   0 $~ $)
dptoqp=. 2 {. ,:
delth =. {{ while. 1 T. '' do. 55 T. '' end. 1 }}  NB. delete all worker threads

delth''  NB. start with no threads
for_t. i. 4 do.
  M =. (,:   ] * 1e_20 * i.@#) 0. + =/~ i. 6
  Yt=. 2000 $ '4'
  assert. (_6 ]\ 1 0 0 0 0 0  0 0 0 0 0 0) ('' run128_9) 00;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;0.0;Yt
  assert. (_6 ]\ 0 0 1 0 0 0  0 0 2e_20 0 0 0) ('' run128_9) 2;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;0.0;Yt
  assert. (_6 ]\ 1 2 0 0 0 0  0 2e_20 0 0 0 0) ('' run128_9) 6;(,."1 (_2) ]\ 0 2);(00 1);(1. 2.);M;0.0;Yt
  assert. (_6 ]\ 0 0 1 2 0 3  0 0 2e_20 6e_20 0 15e_20) ('' run128_9) 8;(,."1 (_2) ]\ (4$0) , 3 3);((3$0) , 2 3 5);((3$0), 1. 2. 3.);M;0.0;Yt
  NB. change sign of result when Yt = '1'
  assert. (- _6 ]\ 1 0 0 0 0 0  0 0 0 0 0 0) ('' run128_9) 00;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;0.0;('1' (0}) Yt)
  assert. (- _6 ]\ 0 0 1 0 0 0  0 0 2e_20 0 0 0) ('' run128_9) 2;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;0.0;('1' (2}) Yt)
  assert. (- _6 ]\ 1 2 0 0 0 0  0 2e_20 0 0 0 0) ('' run128_9) 6;(,."1 (_2) ]\ 0 2);(00 1);(1. 2.);M;0.0;('1' (6}) Yt)
  NB. clamping to threshold
  M =. 1 1e_20 (<1;3;2 3)} 2 {. ,: |: _4 ]\ _1. 0 1 2  _1e_5 _2e_9 0 1e_9   0 0 0 0 0 0 0 1e_10   NB. input by columns
  assert. (2 {. ,: _1. 0 1 2) ('' run128_9) 00;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;0.5;Yt
  assert. (2 {. ,: 0. 0 0 2) ('' run128_9) 00;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;1.5;Yt
  assert. (2 {. ,: 0. 0 0 0) ('' run128_9) 00;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;2.5;Yt
  assert. (2 {. ,: _1e_5 _2e_9 0 1e_9) ('' run128_9) 01;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;1e_10;Yt
  assert. (2 {. ,: _1e_5 _2e_9 0 1e_9) ('' run128_9) 01;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;9e_11;Yt
  assert. (2 {. ,: _1e_5 0 0 0) ('' run128_9) 01;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;1e_8;Yt
  assert. (2 {. ,: 0 0 0 0) ('' run128_9) 2;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;1e_8;Yt  NB. low part ignored because high part 0
  assert. (2 {. ,: 0 0 0 0) ('' run128_9) 3;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;1e_8;Yt  NB. low part cleared when high part is
  NB. Test every different length.  Different size of M are not so important
  for_l. >: i. (*1 T. ''){51 5 do.
    M =. (*  0.25 < 0 ?@$~ $)  _0.5 + (2 # l+?20)?@$ 0  NB. random values of random sizes, with 25% 0s
    M =. 0. + epcanon (,:   ] * (2^_53) * _0.5 + 0 ?@$~ $) M  NB. append extended part, force to float
    'r2 r c' =. $M
    for_j. 0 $~ (*1 T. ''){100 2 do.
      ix =. 00 + l ? c  NB. indexes
      v =. l ?@$ 0  NB. vector values
      pad =. ? 20  NB. number of dummy columns to install
      ref =. |: (,~ v) +/@:*"1!.1 ,.~/ ix{"1 M  NB. M values, small first, weighted by v
      assert. '' ('' run128_9) (c+pad);(,."1 (-pad+1) {. (_2) ]\ 0 , #ix);ix;v;M;0.0;Yt
 NB. obsolete      if. -. 1e_25 > >./ re=. | +/ ref epsub act do. 13!:8]4 [ 'pad__ r__ c__ re__ ix__ v__ ref__ act__ M__' =: pad;r;c;re;ix;v;ref;act;M end.
    end.
  end.

  prtpms =. ([: 1: smoutput@[ smoutput@'' smoutput@])
  prtpms =. -:
  bk =. dptoqp _2 1 3 1e_8
  M =. dptoqp |: _4 ]\ 0. 1 3 0  0 0.5 3 0   1 0 0 0   0 1e_9 0 0   NB. input by columns
  cons =. 1e_11 1e_25 1e_25 1e_11 1e_6 1e_25 _1 0 NB.  QpThresh,Col0Threshold,ColBk0Threshold,ColDangerPivot,ColOkPivot,Bk0Threshold,PriRow,BkOvershoot
  Frow =. _4 _3 _2 _1. 1.
  bkg =. i.{:$M
  sched=.100 $ 100
  NB. Test DIP mode - on identity cols    ndx;Ax;Am;Av;(M, shape 2,m,n);parms;bkgrd;bk;Frow;sched
  assert. 0 0 1 4 10 _4 (((0 0 1,:0 0 2) run128_9)) 0 1 2 3;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  assert. 0 0 1 4 13 _4 (((0 0 1,:0 0 2) run128_9)) 1 0 2 3;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  assert. 0 0 1 4 13 _4 (((0 0 1,:0 0 2) run128_9)) 3 1 0 2;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  assert. 0 3 1 1 4 0 ('' run128_9) (,3);(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched  NB. dangerous pivot
  M =. (0 1 1 + $M) {. (0 0 1 + $M) {.!.1e_10 M
  bk =. (0 1 + $bk) {. bk
  Frow =. (1 + $Frow) {.!.1. Frow
  assert. 0 0 1 16 22 _4 (((0 0 1,:0 0 2) run128_9)) (1j3 #!.4 ]0 1 2 3);(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  assert. 0 0 1 16 25 _4 (((0 0 1,:0 0 2) run128_9)) (1j3 #!.4 ]1 0 2 3);(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  assert. 0 0 1 16 25 _4 (((0 0 1,:0 0 2) run128_9)) (1j3 #!.4 ]3 1 0 2);(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  bkg =. ,00
  assert. 4 0 0 0 0 __ ((4 0 0,4 1 0,:4 3 0) run128_9) (1j3 #!.4 ]0 1 2 3);(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  assert. 4 1 0 0 0 __ ((4 0 0,4 1 0,:4 3 0) run128_9) (1j3 #!.4 ]1 0 2 3);(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  assert. 4 3 0 0 0 __ ((4 0 0,4 1 0,:4 3 0) run128_9) (1j3 #!.4 ]3 1 0 2);(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  assert. 4 3 0 0 0 __ ('' run128_9) (,3);(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched  NB. dangerous pivot
  bkg =. 00 1
  assert. 0 1 1 16 19 _6 ('' run128_9) (1j3 #!.4 ]0 1 2 3);(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  assert. 0 1 1 16 18 _6 ('' run128_9) (1j3 #!.4 ]1 0 2 3);(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  assert. 0 1 1 16 18 _6 ('' run128_9) (1j3 #!.4 ]3 1 0 2);(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  assert. 0 3 1 1 2 0 ('' run128_9) (,3);(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched  NB. dangerous pivot
  bkg =. 0 1 2
  assert. 0 0 1 16 20 _4 (((0 0 1,:0 0 2) run128_9)) (1j3 #!.4 ]0 1 2 3);(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  assert. 0 0 1 16 22 _4 (((0 0 1,:0 0 2) run128_9)) (1j3 #!.4 ]1 0 2 3);(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  assert. 0 0 1 16 22 _4 (((0 0 1,:0 0 2) run128_9)) (1j3 #!.4 ]3 1 0 2);(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  assert. 0 3 1 1 3 0 ('' run128_9) (,3);(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched  NB. dangerous pivot
  NB. verify each lane
  bkg =. 1j3 #!.4 ]0 1 2 3
  assert. 0 0 1 4 42 _4 (((0 0 1,:0 0 2) run128_9)) 0 1 2 3;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  assert. 0 0 1 4 49 _4 (((0 0 1,:0 0 2) run128_9)) 1 0 2 3;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  assert. 0 0 1 4 49 _4 (((0 0 1,:0 0 2) run128_9)) 3 1 0 2;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  bkg =. _1 |.!.4 ] 1j3 #!.4 ]0 1 2 3
  assert. 0 0 1 4 42 _4 (((0 0 1,:0 0 2) run128_9)) 0 1 2 3;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  assert. 0 0 1 4 49 _4 (((0 0 1,:0 0 2) run128_9)) 1 0 2 3;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  assert. 0 0 1 4 49 _4 (((0 0 1,:0 0 2) run128_9)) 3 1 0 2;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  bkg =. _2 |.!.4 ] 1j3 #!.4 ]0 1 2 3
  assert. 0 0 1 4 42 _4 (((0 0 1,:0 0 2) run128_9)) 0 1 2 3;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  assert. 0 0 1 4 49 _4 (((0 0 1,:0 0 2) run128_9)) 1 0 2 3;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  assert. 0 0 1 4 49 _4 (((0 0 1,:0 0 2) run128_9)) 3 1 0 2;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  bkg =. _3 |.!.4 ] 1j3 #!.4 ]0 1 2 3
  assert. 0 0 1 4 42 _4 (((0 0 1,:0 0 2) run128_9)) 0 1 2 3;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  assert. 0 0 1 4 49 _4 (((0 0 1,:0 0 2) run128_9)) 1 0 2 3;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  assert. 0 0 1 4 49 _4 (((0 0 1,:0 0 2) run128_9)) 3 1 0 2;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  NB. empty
  assert. 6 ((0 run128_9)) (0$00);(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched  NB. no columns
  assert. 6 ((0 run128_9)) (0$00);(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched  NB. no columns
  assert. 6 ((0 run128_9)) (,3);(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;($0);bk;Frow;sched  NB. no rows
  assert. 6 ((0 run128_9)) (,3);(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);(2 0 0$0.);cons;bkg;bk;(1$0.);sched  NB. empty M
  bk =. dptoqp _2 1 3 1e_8
  M =. dptoqp |: _4 ]\ 0. 1 3 0  0 0.5 3 0   1 0 0 0   0 1e_9 0 0   NB. input by columns
  Frow =. _4 _3 _2 _1. 1.
  bkg =. i.-{:$M
  sched =.4 $ 100
  assert. 0 0 2 4 10 _4 (((0 0 1,:0 0 2) run128_9)) 0 1 2 3;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  assert. 0 0 2 4 13 _4 (((0 0 1,:0 0 2) run128_9)) 1 0 2 3;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  assert. 0 0 2 4 10 _4 (((0 0 1,:0 0 2) run128_9)) 2 0 1 3;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  assert. 0 0 2 4 13 _4 (((0 0 1,:0 0 2) run128_9)) 3 1 0 2;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  NB. row cutoff
  M =. ({.~   0 1 0 + $) ({.!.1e_10~  0 0 1 + $) dptoqp |: _4 ]\ % 1. 2 3 4   2 1 2 3   1.8 1.2 3 4   2.3 3 4 5 
  Frow =. _4 _3 _2 _1. 1. 1
  bk =. dptoqp 1. 1 1 1 0
  bkg =. 0 1 2 3  NB. test cutoff in different lanes, and then in one lane
  assert. 0 0 0 4 7 _4 ('' run128_9) 0 1 2 3;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  assert. 0 0 0 4 10 _4 ('' run128_9) 1 0 2 3;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  assert. 0 0 0 4 10 _4 ('' run128_9) 2 0 1 3;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  assert. 0 0 0 4 13 _4 ('' run128_9) 2 1 0 3;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  assert. 0 0 0 4 10 _4 ('' run128_9) 3 0 1 2;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  assert. 0 0 0 4 13 _4 ('' run128_9) 3 2 0 1;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  assert. 0 0 0 4 13 _4 ('' run128_9) 3 1 0 2;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  for_j. i. 4 do.
    bkg =. (-j) |.!.4 ] 1j3 #!.4 ]0 1 2 3
    assert. 0 0 0 4 23 _4 ('' run128_9) 0 1 2 3;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
    assert. 0 0 0 4 34 _4 ('' run128_9) 1 0 2 3;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
    assert. 0 0 0 4 38 _4 ('' run128_9) 2 0 1 3;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
    assert. 0 0 0 4 49 _4 ('' run128_9) 2 1 0 3;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
    assert. 0 0 0 4 38 _4 ('' run128_9) 3 0 1 2;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
    assert. 0 0 0 4 53 _4 ('' run128_9) 3 2 0 1;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
    assert. 0 0 0 4 49 _4 ('' run128_9) 3 1 0 2;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  end.
  bk =. dptoqp 0. 0 0 0 0
  NB. col cutoff using schedule
  M =. dptoqp 4 4 $ 1.  NB. every ele is valid
  bkg =. i.{:$M
  bk =. dptoqp 1. 2 2 0  NB. the pivot is the first row; if row 3 is in, all rows abort
  Frow =. _1 _1.1 _1.5 _2 1  NB. improvements are _1 _1.1 _3 _4
  sched =. 5 4 3
  assert. 3 0 0 7 7 0 ((0 run128_9)) 00 0 0 0 0 0 0;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;1 2 3;bk;Frow;sched
  assert. 0 1 0 5 6 _1.1 ((0 run128_9)) 01 1 1 1 1 1 1;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;0 1 2;bk;Frow;sched
  assert. 0 2 0 4 7 _1.5 ((0 run128_9)) 1 2 1 1 1 1 1;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;0 1 2;bk;Frow;sched
  assert. 0 2 0 4 5 _1.5 ((0 run128_9)) 1 1 1 2 1 1 1;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;0 1 2;bk;Frow;sched
  M =. dptoqp 4 4 $!.0 ] 1e_8 1 1 1  NB. first row is pivot; first col is dangerous
  assert. 0 2 0 5 12 _1.5 ((0 run128_9)) 0 0 0 2 2 2 2 2 2 2 2 2 2;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;0 1 2;bk;Frow;sched  NB. don't count dangerous col as improvement
  NB. bk thresh
  M =. dptoqp 4 4 $ 1.
  bk =. dptoqp 1e_1 1e_2 1 1
  bkg =. i.{:$M
  sched =. 100 $ 100
  Frow =. _1 _1.1 _1.5 _2 1  NB. improvements are _1 _1.1 _3 _4
  assert. 3 0 0 4 4 0 ('' run128_9) 0 1 2 3;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;(0.5 (5}) cons);bkg;bk;Frow;sched
  assert. 3 0 0 4 4 0 ('' run128_9) 0 1 2 3;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;(0.05 (5}) cons);bkg;bk;Frow;sched
  assert. 0 3 1 4 16 _.02 ('' run128_9) 0 1 2 3;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;(0.005 (5}) cons);bkg;bk;Frow;sched
  NB. unbounded
  M =. dptoqp |: _3 ]\ 0. 0 0   1 1 1   0 0 1e_40   NB. input by columns.  Need small nonzero to ensure reading the extension.  0 2 3 unbounded
  bk =. dptoqp 1. 2 2  NB. the first row on each col is the pivot
  bkg =. i.{:$M
  Frow =. _1 _1.1 _1.5 1  NB. improvements
  assert. 4 0 0 0 0 __ (((4 0 0,:4 2 0) run128_9)) 0 1 2;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  assert. 4 2 0 1 3 __ (((4 0 0,:4 2 0) run128_9)) 1 2 0;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  assert. 4 3 0 0 0 __ (((4 4 0,4 0 0,:4 2 0) run128_9)) 3 1 2;(,."1 (_2) ]\ 00 1);(1$2);(1$1.0);M;cons;bkg;bk;Frow;sched
  M =. 1. (<1 2 2)} M   NB. now column 3 is no longer unbounded
  assert. 4 0 0 0 0 __ (((4 0 0,:4 2 0) run128_9)) 0 1 2;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  assert. 4 2 0 1 3 __ (((4 0 0,:4 2 0) run128_9)) 1 2 0;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  assert. 4 2 0 2 6 __ (((4 0 0,:4 2 0) run128_9)) 3 1 2;(,."1 (_2) ]\ 00 1);(1$2);(1$1.0);M;cons;bkg;bk;Frow;sched
  NB. stall
  bk =. dptoqp 0. 0 0  NB. no improving pivots
  M =. dptoqp |: _3 ]\ 1. 1 1  1 1 1  1 1 1   NB. input by columns
  assert. 3 0 0 3 3 0 ('' run128_9) 0 1 2;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  NB. virtual row gets priority
  M =. dptoqp |: _4 ]\ 0. 1 1 0   1 0 0 0  0 0 1 0   0 0 0 1   NB. input by columns
  bk =. dptoqp 1. 1 1 1  NB. every row can pivot
  bkg =. i.{:$M
  Frow =. _4. _3 _2 _1 1  NB. improvements reduced col by col
  assert. 0 0 1 4 7 _4 (((0 0 1,:0 0 2) run128_9)) 0 1 2 3;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  assert. 0 1 0 1 8 __ ('' run128_9) 0 1 2 3;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;(00 (6}) cons);bkg;bk;Frow;sched
  assert. 0 0 1 0 4 __ (((0 0 1,:0 0 2) run128_9)) 0 1 2 3;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;(01 (6}) cons);bkg;bk;Frow;sched
  assert. 0 2 2 2 9 __ (((0 2 2,:0 0 2) run128_9)) 0 1 2 3;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;(2 (6}) cons);bkg;bk;Frow;sched  NB. col 0 misses because row 1 gets there first
  assert. 0 0 2 0 4 __ (((0 2 2,:0 0 2) run128_9)) 0 1 2 3;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;(2 (6}) cons);2 0 1 3;bk;Frow;sched  NB. Tie goes to the lowest lane
  assert. 0 0 2 1 8 __ (((0 2 2,:0 0 2) run128_9)) 1 0 2 3;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;(2 (6}) cons);2 0 1 3;bk;Frow;sched
  M =. dptoqp |: _4 ]\ 0. 1 1 0   1 0 1 0  0 0 0 1  0 0 0 1   NB. input by columns
  assert. 0 1 0 1 8 __ (((0 0 1,0 0 2,0 1 0,:0 1 2) run128_9)) 0 1 2 3;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;(00 (6}) cons);bkg;bk;Frow;sched
  M =. dptoqp |: _4 ]\ 0. 1 1 0   1 0 2 0  0 0 0 1  0 0 0 1   NB. input by columns
  assert. 0 0 1 4 10 _4 (((0 0 1,0 1 2,:0 0 2) run128_9)) 0 1 2 3;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;(00 (6}) cons);bkg;bk;Frow;sched   NB. 0 0 2 may slip by if columns reordered; 0 0 1 if rows
  NB. ColBk0 threshold
  M =. dptoqp |: _4 ]\ 1e_8 1e_8 1e_8 1  1e_9 1e_9 1e_9 1  1e_10 1e_10 1e_10 1   0 0 0 1   NB. input by columns
  bk =. dptoqp 0. 0 0 1  NB. every row can pivot
  bkg =. i.{:$M
  Frow =. _4. _3 _2 _1 1  NB. improvements reduced col by col
  assert. 0 3 3 4 7 _1 ('' run128_9) 0 1 2 3;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  assert. 1 2 3 4 7 _2 ('' run128_9) 0 1 2 3;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;(5e_10 (2}) cons);bkg;bk;Frow;sched
  assert. 1 1 3 4 7 _3 ('' run128_9) 0 1 2 3;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;(5e_9 (2}) cons);bkg;bk;Frow;sched
  assert. 1 0 3 4 7 _4 ('' run128_9) 0 1 2 3;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;(5e_8 (2}) cons);bkg;bk;Frow;sched
  NB. Switchover to qp
  M =. 1 ,:~ 4 4 $ 1e_20   NB. input by columns
  bk =. dptoqp 1. 1 1 1  NB. every row can pivot
  Frow =. _4. _3 _2 _1 _1  NB. improvements reduced col by col
  assert. 3 0 0 4 16 0 (((0 4,"1 0 i. 4) run128_9)) 0 1 4 3;(,."1 (_2) ]\ 00 1);(1$00);(1$1.0);M;(1e_21 (0}) cons);bkg;bk;Frow;sched  NB. multithreaded, col4 is a pivot
  assert. 0 4 0 4 16 _1 (((0 4,"1 0 i. 4) run128_9)) 0 1 4 3;(,."1 (_2) ]\ 00 1);(1$00);(1$1.0);M;(1e_19 (0}) cons);bkg;bk;Frow;sched
  
NB. obsolete   NB. nonimproving pivots
NB. obsolete   bk =. dptoqp 4 $ 0.
NB. obsolete   M =. dptoqp |: _4 ]\ 0. 1 0 0        0 1e_10 3 0   1 1e_7 0 0   1e5 1e_5 0 0   NB. input by columns
NB. obsolete   assert. 0 0 1 0 1 0 ('' run128_9) (,00);(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;0 1 2
NB. obsolete   assert. 0 0 1 0 1 0 ('' run128_9) (,00);(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;2 0 1
NB. obsolete   assert. 0 0 2 0 1 0 ('' run128_9) (,01);(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;2 0 1
NB. obsolete   assert. 3 0 0 0 0 0 ('' run128_9) (,01);(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;3 0 1
NB. obsolete   assert. 0 0 0 0 1 0 ('' run128_9) (,3);(,."1 (_2) ]\ 00 1);(1$3);(1$1.0);M;cons;0 1 2
NB. obsolete   assert. 0 0 0 0 1 0 ('' run128_9) (,4);(,."1 (_2) ]\ 00 1);(1$3);(1$1.0);M;cons;0 1 2
NB. obsolete   assert. 0 0 1 0 1 0 ('' run128_9) (,4);(,."1 (_2) ]\ 00 1);(1$3);(1$1.0);M;cons;1 0 2
NB. obsolete   assert. 3 0 0 0 0 0 ('' run128_9) (,4);(,."1 (_2) ]\ 00 1);(1$3);(1$1.0);M;cons;2 3
NB. obsolete   M =. M 1}~ |: _4 ]\ 0. _1 0 0  0 0 0 0        0 0 0 0     0 _1e_5 1e_5 0 
NB. obsolete   assert. 0 0 1 0 1 0 ('' run128_9) (,00);(,."1 (_2) ]\ 00 1);(1$3);(1$1.0);M;cons;0 1 2
NB. obsolete   assert. 0 0 1 0 1 0 ('' run128_9) (,4);(,."1 (_2) ]\ 00 1);(1$3);(1$1.0);M;cons;3 1 2
NB. obsolete   assert. 0 0 0 0 1 0 ('' run128_9) (,4);(,."1 (_2) ]\ 00 1);(1$3);(1$1.0);M;cons;0 1 2
NB. obsolete   assert. 0 0 1 0 1 0 ('' run128_9) (,4);(,."1 (_2) ]\ 00 1);(1$3);(1$1.0);M;cons;3 1 0
NB. obsolete   assert. 0 0 1 0 1 0 ('' run128_9) (,4);(,."1 (_2) ]\ 00 1);(1$3);(1$1.0);M;cons;3 1
NB. obsolete   M =. 1e_20 (<0 3 3)} M 
NB. obsolete   assert. 0 0 0 0 1 0 ((0 0 0,:0 0 1) run128_9) (,4);(,."1 (_2) ]\ 00 1);(1$3);(1$1.0);M;cons;3 1 0
NB. obsolete   assert. 0 0 2 0 1 0 ((0 1 1,0 0 1,:0 0 2) run128_9) (,4);(,."1 (_2) ]\ 00 1);(1$3);(1$1.0);M;cons;3 1 2  NB. qp triggered; but not necessarily in all rows
NB. obsolete   assert. 3 0 0 0 0 0 ((0 0 1,:3 0 0) run128_9) (,4);(,."1 (_2) ]\ 00 1);(1$3);(1$1.0);M;cons;3 1
NB. obsolete 
  NB. gradient-stall mode
  cons =. 1e_11 1e_25 1e_25 1e_11 1e_6 1e_25 _1 NB.  QpThresh,Col0Threshold,ColBk0Threshold,ColDangerPivot,ColOkPivot,Bk0Threshold,PriRow
  M =. dptoqp |: _4 ]\ 1. 2 5 4    1 2 3 _10   _1 _2 _20 _10   1 2 3 0   NB. input by columns
  Frow =. _4. _5 _2 _5 _1e_20
  bk=. dptoqp 4 $ 1e_29
  bkg=. i. {: $ bk
  assert. 0 0 2 2 5 2.9375 ('' run128_9) 00 1;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  assert. 0 3 2 3 9 0.6 ('' run128_9) 00 1 3;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  assert. 0 0 2 3 6 2.9375 ('' run128_9) 0 1 2;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  assert. 0 0 3 3 7 __ (0 run128_9) 3 2 1 0;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;(3 (6)}cons);bkg;bk;Frow;sched  NB. prirow gets priority; can't multithread since we don't move it to top
  assert. 0 0 2 2 5 2.9375 ('' run128_9) 00 1;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;(|.bkg);(|."1 bk);Frow;sched
  assert. 0 3 2 3 9 0.6 ('' run128_9) 00 1 3;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;(|.bkg);(|."1 bk);Frow;sched
  assert. 0 0 2 3 6 2.9375 ('' run128_9) 0 1 2;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;(|.bkg);(|."1 bk);Frow;sched
  bk=. dptoqp 0. 0 1 0  NB. don't look where b not 0 
  assert. 0 0 3 2 5 2.9375 ('' run128_9) 00 1;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  assert. 0 3 1 3 9 0.6 ('' run128_9) 00 1 3;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  assert. 0 0 3 3 6 2.9375 ('' run128_9) 0 1 2;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  assert. 0 0 1 0 1 __ (0 run128_9) 00 1;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;(1 (6)}cons);bkg;bk;Frow;sched  NB. prirow gets priority
  assert. 0 3 1 0 1 __ (0 run128_9) 3 1 0;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;(1 (6)}cons);bkg;bk;Frow;sched
  assert. 0 0 3 2 5 2.9375 ('' run128_9) 00 1;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;(|.bkg);(|."1 bk);Frow;sched
  assert. 0 3 1 3 9 0.6 ('' run128_9) 00 1 3;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;(|.bkg);(|."1 bk);Frow;sched
  assert. 0 0 3 3 6 2.9375 ('' run128_9) 0 1 2;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;(|.bkg);(|."1 bk);Frow;sched
  M =. dptoqp |: _6 ]\ 1. 2 5 4 5 6    1 2 3 _1 1 10   _1 _2 0 0 1 4   1 _2 _3 4 5 6    1 1 1 1 1 1   1 2 3 _1 1 0   NB. input by columns
  bk=. dptoqp 6 $ 1e_29
  bkg=. i. {: $ bk
  Frow =. _4. _5 _2.5 _1 _1 _5 _1e_20 
  assert. 0 1 5 2 12 4.68 ('' run128_9) 00 1;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  assert. 0 5 2 2 12 0.68 ('' run128_9) 00 5;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  assert. 0 2 5 3 18 3.68 ('' run128_9) 0 1 2;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched
  M =. dptoqp |: _4 ]\ 1.0001e_4 _1e_4 1e_4 _1e_4    1 2 3 _10   1e_4 _1e_4 1e_4 _10   1 2 3 0   NB. input by columns
  bk=. dptoqp 4 $ 1e_29
  bkg=. i. {: $ bk
  Frow =. _1.e10 _5 _2.5 _2e10 _1e_20 
  assert. 0 1 2 3 9 4.6 ('' run128_9) 00 1 2;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;(1e_3 (1)} cons);bkg;bk;Frow;sched  NB. Test discarding small column values
  assert. 0 0 0 2 5 1.000000040002e_20 ('' run128_9) 00 1;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;(cons);bkg;bk;Frow;sched

  NB. bound variables.  These run only in a single thread
  if. 0 = 1 T. '' do.
  dptoqp=. 2 {. ,:
  bk =. dptoqp         0.25 0.5
  M =. dptoqp |: _4 ]\ 0.25 1 0 0  _0.25 0.25 0 0  _0.25 0.25 0 0  _0.25 0.25 0 0    NB. input by columns
  cons =. 1e_11 1e_25 1e_25 1e_11 1e_6 1e_25 _1 0 NB.  QpThresh,Col0Threshold,ColBk0Threshold,ColDangerPivot,ColOkPivot,Bk0Threshold,PriRow,BkOvershoot
  Frow =. _4 _3 _2 _1. 1.
  bkg=. 00 1
  sched =.4 $ 100
  bkbeta=.,1.
  beta=.1. 0 1 2 0
  rvt =. '44044'
  assert. 0 0 1 1 2 _2 ('' run128_9) (,00);(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;(1e_3 (1)} cons);bkg;bk;Frow;sched;bkbeta;beta;rvt
  assert. 0 1 1 1 2 _6 ('' run128_9) (,01);(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;(1e_3 (1)} cons);bkg;bk;Frow;sched;bkbeta;beta;rvt
  assert. 0 2 4 1 2 _2 ('' run128_9) (,02);(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;(1e_3 (1)} cons);bkg;bk;Frow;sched;bkbeta;beta;rvt
  assert. 0 3 1 1 2 _2 ('' run128_9) (,03);(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;(1e_3 (1)} cons);bkg;bk;Frow;sched;bkbeta;beta;rvt
  bk =. dptoqp         0.1 0.5
  bkg=. 00 1
  bkbeta=.,0.25
  assert. 0 0 0 1 2 _1.6 ('' run128_9) (,00);(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;(1e_3 (1)} cons);bkg;bk;Frow;sched;bkbeta;beta;rvt
  assert. 0 1 0 1 2 _3 ('' run128_9) (,01);(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;(1e_3 (1)} cons);bkg;bk;Frow;sched;bkbeta;beta;rvt
  assert. 0 2 4 1 2 _2 ('' run128_9) (,02);(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;(1e_3 (1)} cons);bkg;bk;Frow;sched;bkbeta;beta;rvt

  NB. gradient with bound vars
  bk =. dptoqp         0.1 0.5 1 0
  bkg=. i. {: $ bk
  bkbeta=.0.25 0
  M =. dptoqp |: _4 ]\ 0.25 _0.1 1 0  _0.25 _0.8 0 0  0.25 _0.1 0.9 0  0.25 _0.1 0.9 0    NB. input by columns
  Frow =. _1 _1 _1 _1. 1.
  cons =. 1e_11 1e_25 1e_25 1e_11 1e_6 1e_25 _1 NB.  QpThresh,Col0Threshold,ColBk0Threshold,ColDangerPivot,ColOkPivot,Bk0Threshold,PriRow
  rvt =. '44044'
  assert. 0 0 1 2 8 2.145 ('' run128_9) 00 1;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched;bkbeta;beta;rvt
  assert. 0 0 1 2 7 2.145 ('' run128_9) 00 2;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched;bkbeta;beta;rvt
  assert. 0 3 1 2 8 1.955 ('' run128_9) 00 3;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched;bkbeta;beta;rvt
  M =. dptoqp |: _4 ]\ 0.25 _0.1 1 0  _0.25 _0.8 0 0  0.25 _0.1 0.9 0  0.25 _0.1 0.9 0.2    NB. input by columns
  assert. 0 3 3 2 8 1.995 ('' run128_9) 00 3;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched;bkbeta;beta;rvt
  M =. dptoqp |: _4 ]\ 0.25 _0.1 1 0  _0.25 _0.6 0 0  0.25 _0.1 0.9 0  0.25 1 0 0    NB. input by columns
  assert. 0 1 1 2 8 1.845 ('' run128_9) 00 1;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched;bkbeta;beta;rvt
  rvt =. '44144'
  M =. dptoqp |: _4 ]\ 0.25 _0.1 1 0  _0.25 _0.6 0 0  _0.25 0.2 _0.9 0  0.25 1 0 0    NB. input by columns
  assert. 0 0 1 2 7 2.145 ('' run128_9) 00 2;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched;bkbeta;beta;rvt
  M =. dptoqp |: _4 ]\ 0.25 _0.1 0.9 0  _0.25 _0.6 0 0  _0.25 0.2 _0.9 0  0.25 1 0 0    NB. input by columns
  assert. 0 0 1 2 7 1.955 ('' run128_9) 00 2;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched;bkbeta;beta;rvt
  M =. dptoqp |: _4 ]\ 0.25 _0.1 1 0  _0.25 _0.6 0 0  _0.25 0.2 _0.9 0  0.25 1 0 0    NB. input by columns - col 2 has the hidden 1
  assert. 0 0 1 2 7 2.145 ('' run128_9) 00 2;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched;bkbeta;beta;rvt
  M =. dptoqp |: _4 ]\ 0.25 _0.1 2 0  _0.25 _0.6 0 0  _0.25 0.2 _0.9 0  0.25 1 0 0    NB. input by columns  - hidden 1 overwhelmed
  assert. 0 2 1 2 8 3.015 ('' run128_9) 00 2;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched;bkbeta;beta;rvt
  M =. dptoqp |: _4 ]\ 0.25 _0.1 2 0  _0.25 _0.6 0 0  _0.25 0.1 _0.9 0  0.25 1 0 0    NB. input by columns
  assert. 0 2 1 2 8 2.955 ('' run128_9) 00 2;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched;bkbeta;beta;rvt
  M =. dptoqp |: _4 ]\ 0.25 _0.1 2 0  _0.25 _0.6 0 0  _0.25 0.1 _0.9 _0.5  0.25 1 0 0    NB. input by columns
  assert. 0 2 3 2 8 3.205 ('' run128_9) 00 2;(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched;bkbeta;beta;rvt

  NB. bound nonbasic column swap
  cons =. 1e_11 1e_25 1e_25 1e_11 1e_6 1e_25 _1 0 NB.  QpThresh,Col0Threshold,ColBk0Threshold,ColDangerPivot,ColOkPivot,Bk0Threshold,PriRow,BkOvershoot
  M =. dptoqp |: _4 ]\ 0.25 _0.1 1 0  1 1 1 1  1 1 1 1  0.25 1 0 0    NB. input by columns
  bk =. dptoqp         2. 2 2 2
  bkbeta=.0. 0
  bkg=. i. {: $ bk
  rvt =. '40044'
  sched =.4 $ 100
  beta=.1. 1.5 1 2 0
  Frow =. _1 _2 _1 _1. 1.
  assert. 0 2 4 1 4 _1 ('' run128_9) (,2);(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched;bkbeta;beta;rvt
  assert. 0 1 4 1 4 _3 ('' run128_9) (,01);(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched;bkbeta;beta;rvt
  assert. 0 1 4 2 8 _3 ('' run128_9) (1 2);(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched;bkbeta;beta;rvt
  assert. 0 1 4 2 8 _3 ('' run128_9) (2 1);(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched;bkbeta;beta;rvt

  NB. gradient nonbasic col swap - should never happen
  bk =. dptoqp         0.0 0.0 0 0
  bkg=. i. {: $ bk
  bkbeta=.0$0.
  beta=. ({:$Frow) $ 0.
  M =. dptoqp |: _4 ]\ 0.25 0.25 0.30 0.25  0.25 0.25 1.30 0.25   1 1 1 1    1 1 1 1    NB. input by columns
  Frow =. _1 _1 _1 _1. 1.
  sched =.4 $ 100
  rvt =. '00044'
  cons =. 1e_11 1e_25 1e_25 1e_11 1e_6 1e_25 _1 NB.  QpThresh,Col0Threshold,ColBk0Threshold,ColDangerPivot,ColOkPivot,Bk0Threshold,PriRow
  assert. 0 0 2 1 4 2.2775 ('' run128_9) (,00);(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched;bkbeta;beta;rvt
  assert. 0 1 2 1 4 3.8775 ('' run128_9) (,01);(,."1 (_2) ]\ 00 0);(0$00);(0$0.0);M;cons;bkg;bk;Frow;sched;bkbeta;beta;rvt

  end.  NB. one thread

  NB. end of tests, add a thread
  0 T. ''
end.

delth''  NB. make sure we end with an empty system

1
}}^:(+./ ('avx2';'avx512') +./@:E.&> <9!:14'') 1

NB.  128!:13 ck[;bk;colthresh,bkthresh] ------------------------
{{
NB. qp reciprocal mode
qpmulvecatom=: (|:@(|:@(1 1 0 0 { [) +/@:*"1!.1 (1 0 1 0 { ])))
assert. ((1.0=!.0[) *. (1e_30>!.0]))/@,@(,.  qpmulvecatom (128!:13))"1 (,. 1e_18 * (*  0 ?@:$~ $)) 2000 * 10000 ?@$ 0

savspr =: {{ (exp =: x) -: res =: 128!:13 arg =: y }}

NB. SPR calc, possibly multithreaded
while. 1 T. '' do. 55 T. '' end.
while. 4 > 1. T. '' do.
0 -: (128!:13) (,. 2. 0);(,. 1. 0);1e_9 1e_22
1 -: (128!:13) (|: _2 ]\ 2. 0 2 1e_20 );(|: _2 ]\ 1. 0 1. 0);1e_9 1e_22

NB. one SPR smaller than all the others
ftype =: 0
f =: {{
 cbhi =. 1. 0.5e_18 1. 0.25e_18 [ cblo =. 1. 0.50000000001e_18 1. 0.25e_18
 cbtbl =. cblo , cbhi *"1 0 y ?@$ 100   NB. 1 small SPR
 for. i. x do.
  lowloc =. 0 i.~ perm =. ?~ # (1 >. ?y) {. cbtbl  NB. shuffle & remember position of low
  'c b' =. (0 1&{ ; 2 3&{) |: perm { cbtbl 
  assert. lowloc savspr c;b;1e_9 1e_22
 end.
 1
}}

400 f 1000
400 f 10000  NB. multithreads

NB. random SPRs, all contenders
ftype =: 1
f =: {{
 locparts =. (}.~   ] i:!.0 {.) \:~ 4 * 1e_19 * _0.5 + y ?@$ 0  NB. min 2 LSBs separation; ensure min unique
 cbtbl =. (*   [: (0}~   1 >. {.) 100 ?@$~ #) 1.0 ,. locparts ,"(0 1)  1.0 , 1e_18 * _0.5 * ? 0  NB. different c, same b, scaled.  Smallest must not go to 0; others can
 for. i. x do.
  lowloc =. 0 i.~ perm =. ?~ # (1 >. ?y) {. cbtbl  NB. shuffle & remember position of low
  'c b' =. (0 1&{ ; 2 3&{) |: perm { cbtbl 
  assert.lowloc savspr c;b;1e_9 1e_22
 end.
 1
}}
400 f 1000
400 f 10000  NB. multithreads

NB. Random variation in all of chi and blo (which must be the same everywhere)
ftype =: 2
f =: {{
 locparts =. (}.~   ] i:!.0 {.) \:~ 4 * 1e_19 * _0.5 + y ?@$ 0  NB. min 2 LSBs separation; ensure min unique
 for. i. x do.
  cbtbl =. (1.0+?0) ,. locparts ,"(0 1)  (1.0+?0) , 1e_18 * _0.5 * ? 0  NB. different c, same b, scaled
  lowloc =. 0 i.~ perm =. ?~ # (1 >. ?y) {. cbtbl  NB. shuffle & remember position of low
  'c b' =. (0 1&{ ; 2 3&{) |: perm { cbtbl 
  assert. lowloc savspr c;b;1e_9 1e_22
 end.
 1
}}
400 f 1000
400 f 10000  NB. multithreads


NB. Some noncontenders
ftype =: 3
f =: {{
 locparts =. (}.~   ] i:!.0 {.) \:~ 4 * 1e_19 * _0.5 + y ?@$ 0  NB. min 2 LSBs separation; ensure min unique
 cbtbl =. 1.0 ,. locparts ,"(0 1)  1.0 , 1e_18 * _0.5 * ? 0  NB. different c, same b
 cbtbl =. (*   [: (0}~   1 >. {.) 100 ?@$~ #) cbtbl , 200 # ,: 1.0 0 2.0 0   NB. add noncontenders and scale, not allowing smallest to go to 0
 for. i. x do.
  lowloc =. ? # perm =. (2 >. ?y) ?&.:<: # cbtbl
  perm =. 00 lowloc} perm
  'c b' =. (0 1&{ ; 2 3&{) |: perm { cbtbl 
  assert. lowloc savspr c;b;1e_9 1e_22
 end.
 1
}}
400 f 1000
400 f 10000  NB. multithreads

0 T. 0  NB. allocate a worker thread
end.   NB. end of loop with different # threads
while. 1 T. '' do. 55 T. '' end.  NB. clear threads

assert. 'limit error' -: (128!:13) etx (|: _2 ]\ 0. 0 0 0 );(|: _2 ]\ 1. 0 2. 1e_20);1e_9 1e_22  NB. no SPRs
assert. 'domain error' -: (128!:13) etx 'abc'
assert. 'domain error' -: (128!:13) etx 0 1
assert. 'domain error' -: (128!:13) etx 2 3
assert. 'rank error' -: (128!:13) etx 0. + i. 2 3
assert. 'length error' -: (128!:13) etx 1. 2 3
assert. 'length error' -: (128!:13) etx 1;2;3;4
assert. 'length error' -: (128!:13) etx 1;2
assert. 'domain error' -: (128!:13) etx (1,:0);(1.,:0.);1. 1.
assert. 'domain error' -: (128!:13) etx (4,:0);(1.,:0.);1. 1.
assert. 'domain error' -: (128!:13) etx (1.,:0.);(1,:0);1. 1.
assert. 'domain error' -: (128!:13) etx (1.,:0.);(4,:0);1. 1.
assert. 'domain error' -: (128!:13) etx (1.,:0.);(1.,:0.);1 1
assert. 'domain error' -: (128!:13) etx (1.,:0.);(1.,:0.);2 2
assert. 'rank error' -: (128!:13) etx (,:1.,:0.);(1.,:0.);1. 1.
assert. 'rank error' -: (128!:13) etx (1.,:0.);(,:1.,:0.);1. 1.
assert. 'rank error' -: (128!:13) etx (1.,:0.);(1.,:0.);,:1. 1.
assert. 'length error' -: (128!:13) etx (0,1.,:0.);(1.,:0.);1. 1.
assert. 'length error' -: (128!:13) etx (1.,:0.);(0,1.,:0.);1. 1.
assert. 'length error' -: (128!:13) etx (1.,:0.);(1.,:0.);0 1. 1.

1
}}^:(+./ ('avx2';'avx512') +./@:E.&> <9!:14'') 1


NB. (prx;pcx;pivotcolnon0;newrownon0;absfuzz) 128!:12 Qk ------------------------

epdefuzzsub =: {{ ([: (*"_1 _   u <!.0 |@{.) epsub) }}
NB. normal precision
f =: 1:`({{
 while. 1 T. '' do. 55 T. '' end.
 while. 2 > 1. T. '' do.
  siz =. y  NB. size of Qk
  'r c' =. 2 ?@$ >:siz  NB. size of modified area
  prx =. 00 + r ? siz [ pcx =. 00 + c ? siz  NB. indexes of mods
  pivotcolnon0 =.r ?@$ 0 [ newrownon0 =. c ?@$ 0
  absfuzz =. 1e_14 * ? 0  NB. tolerance
  Qk =. (siz,siz) ?@$ 0
  expQk=. (((<prx;pcx) { Qk) ((*  absfuzz <!.0 |)@:-) pivotcolnon0 */ newrownon0) (<prx;pcx)} preQk =. memu Qk
  Qk =. (prx;pcx;pivotcolnon0;newrownon0;absfuzz) 128!:12 Qk
  if. -. r =. 1e_13 > >./ , | Qk - expQk do. 13!:8]4 [ 'prx__ pcx__ pivotcolnon0__ newrownon0__ absfuzz__ expQk__ preQk__ Qk__' =: prx;pcx;pivotcolnon0;newrownon0;absfuzz;expQk;preQk;Qk end.
  0 T. 0  NB. allocate a worker thread
 end.
 while. 1 T. '' do. 55 T. '' end.
 r 
}}"0)@.(+./ ('avx2';'avx512') +./@:E.&> <9!:14'')
f i. 65

NB. quad precision
f =: 1:`({{
 epmul =. (|:~ (_1 |. i.@#@$)) @: (((0 0 1 1{[) +/@:*"1!.1 (0 1 0 1{]))"1&(0&|:))
 epadd =. (|:~ (_1 |. i.@#@$)) @: ((1.0"0 +/@:*"1!.1 ])@,"1&(0&|:))
 epsub =. (epadd -)
NB. obsolete  epdefuzzsub =. ((]: * >.)&:|&{. ((<!.0 |@{.) *"_ _1 ]) epsub)
 epcanon =. (epadd   0 $~ $)
 siz =. y  NB. size of Qk
 while. 1 T. '' do. 55 T. '' end.
 while. 2 > 1. T. '' do.
  'r c' =. 2 ?@$ >:siz  NB. size of modified area
  prx =. 00 + r ? siz [ pcx =. 00 + c ? siz  NB. indexes of mods
  pivotcolnon0 =. (] {~ r ? #) (, -) 10 ^ (+   3 * *) 6. * _0.5 + r ?@$ 0  NB. random values 1e3 to 1e6 and 1e_3 to 1e_6, both signs
  pivotcolnon0 =. epcanon (,:   ] * (2^_53) * _0.5 + 0 ?@$~ $) pivotcolnon0  NB. append extended part
  pivotcolnon0 =. 0. + pivotcolnon0  NB. force to float
  newrownon0 =. (] {~ c ? #) (, -) 10 ^ (+   3 * *) 6. * _0.5 + c ?@$ 0  NB. random values 1e3 to 1e6 and 1e_3 to 1e_6, both signs
  newrownon0 =. epcanon (,:   ] * (2^_53) * _0.5 + 0 ?@$~ $) newrownon0  NB. append extended part
  newrownon0 =. 0. + newrownon0  NB. force to float
  absfuzz =. 1e_25 * ? 0  NB. tolerance
  Qk =. (*  0.25 < 0 ?@$~ $) 1e6 * (siz,siz) ?@$ 0.
  Qk =. epcanon (,:   ] * (2^_53) * _0.5 + 0 ?@$~ $) Qk  NB. append extended part
  expQk=. (((<a:;prx;pcx) { Qk) (absfuzz epdefuzzsub) (c #"0 pivotcolnon0) epmul (r&#@,:"1 newrownon0)) (<a:;prx;pcx)} preQk =. memu Qk
  Qk =. (prx;pcx;({.pivotcolnon0);({.newrownon0);absfuzz) 128!:12 Qk
  if. -. 1e_30 > >./ re =. , | (+/  expQk epsub Qk) % (| +/ Qk) >. (| +/ preQk) >. (| +/ expQk) do. 13!:8]4 [ 'r__ c__ re__ prx__ pcx__ pivotcolnon0__ newrownon0__ absfuzz__ expQk__ preQk__ Qk__' =: r;c;re;prx;pcx;pivotcolnon0;newrownon0;absfuzz;expQk;preQk;Qk end.
  0 T. 0  NB. allocate a worker thread
 end.
 while. 1 T. '' do. 55 T. '' end.
 1
}}"0)@.(+./ ('avx2';'avx512') +./@:E.&> <9!:14'')

NB. quad precision Qk but dp row and col
f =: 1:`({{
 epmul =. (|:~ (_1 |. i.@#@$)) @: (((0 0 1 1{[) +/@:*"1!.1 (0 1 0 1{]))"1&(0&|:))
 epadd =. (|:~ (_1 |. i.@#@$)) @: ((1.0"0 +/@:*"1!.1 ])@,"1&(0&|:))
 epsub =. (epadd -)
NB. obsolete  epdefuzzsub =. ((]: * >.)&:|&{. ((<!.0 |@{.) *"_ _1 ]) epsub)
 epcanon =. (epadd   0 $~ $)
 siz =. y  NB. size of Qk
 while. 1 T. '' do. 55 T. '' end.
 while. 2 > 1. T. '' do.
  'r c' =. 2 ?@$ >:siz  NB. size of modified area
  prx =. 00 + r ? siz [ pcx =. 00 + c ? siz  NB. indexes of mods
  pivotcolnon0 =. (] {~ r ? #) (, -) 10 ^ (+   3 * *) 6. * _0.5 + r ?@$ 0  NB. random values 1e3 to 1e6 and 1e_3 to 1e_6, both signs
  pivotcolnon0 =. 2 {. ,: pivotcolnon0  NB. append extended part - all 0
  pivotcolnon0 =. 0. + pivotcolnon0  NB. force to float
  newrownon0 =. (] {~ c ? #) (, -) 10 ^ (+   3 * *) 6. * _0.5 + c ?@$ 0  NB. random values 1e3 to 1e6 and 1e_3 to 1e_6, both signs
  newrownon0 =. 2 {. ,: newrownon0  NB. append extended part
  newrownon0 =. 0. + newrownon0  NB. force to float
  absfuzz =. 1e_25 * ? 0  NB. tolerance
  Qk =. (*  0.25 < 0 ?@$~ $) 1e6 * (siz,siz) ?@$ 0.
  Qk =. epcanon (,:   ] * (2^_53) * _0.5 + 0 ?@$~ $) Qk  NB. append extended part
  expQk=. (((<a:;prx;pcx) { Qk) (absfuzz epdefuzzsub) (c #"0 pivotcolnon0) epmul (r&#@,:"1 newrownon0)) (<a:;prx;pcx)} preQk =. memu Qk
  Qk =. (prx;pcx;pivotcolnon0;newrownon0;absfuzz) 128!:12 Qk
  if. -. 1e_30 > >./ re =. , | (+/  expQk epsub Qk) % (| +/ Qk) >. (| +/ preQk) >. (| +/ expQk) do. 13!:8]4 [ 'r__ c__ re__ prx__ pcx__ pivotcolnon0__ newrownon0__ absfuzz__ expQk__ preQk__ Qk__' =: r;c;re;prx;pcx;pivotcolnon0;newrownon0;absfuzz;expQk;preQk;Qk end.
  0 T. 0  NB. allocate a worker thread
 end.
 while. 1 T. '' do. 55 T. '' end.
 1
}}"0)@.(+./ ('avx2';'avx512') +./@:E.&> <9!:14'')


f i. 65

NB. avoid repeated indexes
f =: 1:`({{
 ck =. 0. + i. 10
 prcr =. ;@:((<@:({.~  1 i.~ 0 1 2 3 ~: i.~))"1) (#: i.@:(*/)) 4 4 4 4
 prcrnub =. ~. prcr
 prcl =. (#prcr) # 01
 newrownon0 =. >: (#prcr) ?@$ 0
 mplr =. ($prcr) ?@$ 0
 ckchg =. prcr +//. (prcl # newrownon0) * mplr
 upd=. ckchg -~ prcrnub { ck
 expck=. upd prcrnub} preck =. memu ck
 ck =. (00;prcr;(,1.);(prcl #"01 newrownon0);(mplr)) 128!:12 ck
 if. -. r =. 1e_13 > >./ , | ck - expck do. 13!:8]4 [ ' prcr__  newrownon0__  expck__ preck__ ck__' =: prcr;newrownon0;expck;preck;ck end.
 1
}})@.(+./ ('avx2';'avx512') +./@:E.&> <9!:14'')
f ''


NB. (0;pcx;pivotcolnon0;newrownon0;absfuzz) 128!:12 bk ------------------------
NB. normal precision
f =: 1:`({{
 siz =. y  NB. size of bk
 r =. 1 [ c =. ? >:siz  NB. size of modified area
 pcx =. 00 + c ? siz  NB. indexes of mods
 pivotcolnon0 =. r ?@$ 0 [ newrownon0 =. c ?@$ 0
 absfuzz =. 1e_14 * ? 0  NB. tolerance
 bk =. siz ?@$ 0
 expbk=. (((<<pcx) { bk) ((*  absfuzz <!.0 |)@:-) (c # pivotcolnon0) * newrownon0) (<<pcx)} prebk =. memu bk
 bk =. (00;pcx;pivotcolnon0;newrownon0;absfuzz) 128!:12 bk
 if. -. r =. 1e_13 > >./ , | bk - expbk do. 13!:8]4 [ 'pcx__ pivotcolnon0__ newrownon0__ absfuzz__ expbk__ prebk__ bk__' =: pcx;pivotcolnon0;newrownon0;absfuzz;expbk;prebk;bk end.
 r 
}}"0)@.(+./ ('avx2';'avx512') +./@:E.&> <9!:14'')
f i. 65

NB. quad precision
f =: 1:`({{
 epmul =. (|:~ (_1 |. i.@#@$)) @: (((0 0 1 1{[) +/@:*"1!.1 (0 1 0 1{]))"1&(0&|:))
 epadd =. (|:~ (_1 |. i.@#@$)) @: ((1.0"0 +/@:*"1!.1 ])@,"1&(0&|:))
 epsub =. (epadd -)
 epcanon =. (epadd   0 $~ $)
 siz =. y  NB. size of Qk
 r =. 1 [ c =. ? >:siz  NB. size of modified area
 pcx =. 00 + c ? siz  NB. indexes of mods
 pivotcolnon0 =.r ?@$ 0 [ newrownon0 =. c ?@$ 0
 pivotcolnon0 =. (] {~ r ? #) (, -) 10 ^ (+   3 * *) 6. * _0.5 + r ?@$ 0  NB. random values 1e3 to 1e6 and 1e_3 to 1e_6, both signs
 pivotcolnon0 =. epcanon (,:   ] * (2^_53) * _0.5 + 0 ?@$~ $) pivotcolnon0  NB. append extended part
 pivotcolnon0 =. 0. + pivotcolnon0  NB. force to float
 newrownon0 =. (] {~ c ? #) (, -) 10 ^ (+   3 * *) 6. * _0.5 + c ?@$ 0  NB. random values 1e3 to 1e6 and 1e_3 to 1e_6, both signs
 newrownon0 =. epcanon (,:   ] * (2^_53) * _0.5 + 0 ?@$~ $) newrownon0  NB. append extended part
 newrownon0 =. 0. + newrownon0  NB. force to float
 absfuzz =. 1e_25 * ? 0  NB. tolerance
 bk =. (*  0.25 < 0 ?@$~ $) 1e6 * siz ?@$ 0.
 bk =. epcanon (,:   ] * (2^_53) * _0.5 + 0 ?@$~ $) bk  NB. append extended part
 expbk=. (((<a:;pcx) { bk) (absfuzz epdefuzzsub) (c #"1 pivotcolnon0) epmul (newrownon0)) (<a:;pcx)} prebk=. memu bk
 bk =. (00;pcx;pivotcolnon0;newrownon0;absfuzz) 128!:12 bk
 if. -. 1e_30 > >./ re =. , | (+/  expbk epsub bk) % (| +/ bk) >. (| +/ prebk) >. (| +/ expbk) do. 13!:8]4 [ 'r__ c__ re__ pcx__ pivotcolnon0__ newrownon0__ absfuzz__ expbk__ prebk__ bk__' =: r;c;re;pcx;pivotcolnon0;newrownon0;absfuzz;expbk;prebk;bk end.
 1
}}"0)@.(+./ ('avx2';'avx512') +./@:E.&> <9!:14'')

f i. 65



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

count =. */@$@>
prod  =. */\.@}.@(,&1)
copy  =. */@[ $&> prod@[ (#,)&.> ]
catalog =. ;@:($&.>) $ count <"1@|:@copy ]

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

NB.2 3 -: 7{"1] 3 (<1 7)} 2 5e9$2
NB.usually not enough memory for ^^

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
(<1 2;1 2) ({ -: }."1@}.@]) 2r5+i. 3 3  NB. failed in 32-bit because cell is 8 bytes long
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

40 (i.@] (-. -: ({~ <@<@<)) ?@$)"0 (100+2 ^ i. 8)
(40+2 ^ i. 8) (i.@] (-. -: ({~ <@<@<)) ?@$)"0 (100)
(40+2 ^ i. 8) (i.@] (-. -: ({~ <@<@<)) (?@#@[) -~ ?@$)"0 (100)


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



 
4!:55 ;:'a adot1 adot2 sdot0 arg b catalog copy count epdefuzzsub exp f fr from ftype i j qpmulvecatom res run128_9 savx savy savref savres savspr'
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


epilog''

