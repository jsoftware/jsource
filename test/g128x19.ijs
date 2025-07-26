prolog './g128x19.ijs'

NB. **** 128!:19

e128x19 =: {{
if. '' -: $ 8{:: y do.   NB. col/spr
 qy =: ([: (15!:18) 11 c. +/@:(11&c.))&.>&.(3 6 7&{) y
 qres =: 128!:19 qy
 dres =. ((] ,: 8 c. -) 8&c.) 7{::qy  NB. convert column back to dp
else.  NB. gradient
 qy =: ([: (15!:18) 11 c. +/@:(11&c.))&.>&.(3&{) y
 qres =: 128!:19 qy
 dres =. 7{::qy  NB. cutoff is already dp
 end.
 qres ; dres   NB. col 7 is the one that is modified
}}

NB. 128!:9  Ax;Am;Av;(M, shape 2,m,n);RVT;bndrowmask;bk;z;ndx;parms;bkbeta ---------------------------------------------------------
NB. y is 128!:9 parms, x is expected result (empty if we have to check column values).  m is parms
NB. Save args, run 128!:9, check result.  For multithreads, expand M/bk/Frow to big enough to engage threading (leaving last row/col harmless)
NB. and expand bkg and ndx to  make enough work to keep the threads busy
run128_9 =: {{
epadd =. (|:~ (_1 |. i.@#@$)) @: ((1.0"0 +/@:*"1!.1 ])@,"1&(0&|:))
epsub =. (epadd -)
savx =: x [ savy =: y
nthr =. 1 T. ''
opts =. m
if. '' -: $ colx =. 8 {::y do.
  NB. onecol+SPR
  NB. y is Ax;Am;Av;(M, shape 2,m,n);RVT;bndrowmask;bk;z;ndx;parms;bkbeta;beta  where ndx is an atom  (1st 8 blocks don't move)
  NB. here RVT is chars and must be converted to mask, same for bndrowmask
  NB. We expect bkbeta to have the correct result item count - extended to batch boundary including Fk if present
  'Ax Am Av Qk rvt bndrowmask bk ndx parms bkbeta' =. 0 1 2 3 4 5 6 8 9 10 { y  NB. don't reassign z lest it lose KNOWNNAMED
  NB.   parms is #cols in Qk(flagged),maxAx,Col0Threshold,Store0Thresh,x,ColDangerPivot,ColOkPivot[Imp,ColOkPivotNonImp],Bk0Threshold,BkOvershoot,MinSPR,PriRow
  if. 11=#parms do. parms =. (2 (6}) 11$01) # parms end.  NB. if test omits NonImp, make it equal Imp
  origparms =: parms  NB. debug
  nflagged =. 0{parms  NB. # rows of Qk, with fk flag
  nrows =. 1{$Qk    NB. calculate all rows of result including Fk if any
  ncol =. ({: $ Qk)  NB. number of columns in basis
  if. 0~:nthr do.   NB. multiple threads: expand Qk with 0 rows to make size threadable, scramble rows in the data.  Keep bndrowmask/bk/bkbeta corresponding to same values
    NB. Create the vector of sources for each new row of Qk etc.  We will first add a harmless row and then draw, where _1 is harmless and _2 is the Fk row if present
    nrows =. #drawvec =. ,&_2^:(nflagged<0) ({~ ?~@#) (12000 + ? 50) {.!._1 i. nrows - (nflagged<0)  NB. modify #rows to match the unpadded block
    NB. modify the inputs
    Qk =. drawvec {"1 _1 Qk ,"_1 (0.)
    bndrowmask =. drawvec { bndrowmask , '4'
    drawvec =. ({.!._1~ >.&.(%&4)@#) drawvec NB. pad bk/bkbeta/z to batch bdy
    bk =. drawvec {"1 _1 bk ,"_1 (0.)
    bkbeta =. drawvec { bkbeta , 0.
NB. obsolete     parms =. (-^:(nflagged < 0) nrows) 0} parms  NB. restore Fk flag to new length
    y =. (Qk;bk;parms;bkbeta) 3 6 9 10} y
  else.   NB. One thread.  just pad bk/bkbeta to cacheline bdy
    bk =. (#bkbeta) {."1 bk [ bkbeta =. ({.~ >.&.(%&4)@#) bkbeta
  end.
  if. colx<ncol do. savref   =: ref =. colx {"1 Qk  NB. basic column, just fetch it
  else.
    qkcols =. (colx-ncol) { Ax  NB. start,length of column indexes/weights
    savref   =: ref =. |: (,~ qkcols ];.0 Av) +/@:*"1!.1 ,.~/ (qkcols ];.0 Am){"1 Qk  NB. fetch cols and combine them
  end.
  if. '19' e.~ colx { rvt do. ref =. - ref end.  NB. change sign of column if enforcing
  savref =: ref =. (3{parms) (((< |) {.) *"1 ]) ref  NB. Clear values below the Store0 threshold
  NB. convert bitmasks to internal form
  rvt =. {&a.@:#.@:(|."1) ($~  8 ,~ >.@(8 %~ #)) , ,./ rvt e."1 '19',:'01'  NB. create values in littleendian order, 2 bits/col (enforced is LSB)
  bndrowmask =. {&a.@:#.@:(|."1) _8 ]\ , (|."1) 0 2 1 |: 16 4&$"1 ($!.0~  64 ,~ >.@(64 %~ #)) 0 ,~ bndrowmask e. '01'  NB. 0 is for Fk
  y =. (rvt;bndrowmask) 4 5} y
  NB. Transpose Qk.  Extend Qk, to cacheline row length, with 0 extend (bk/bkbeta padded already).  Change parms[0 1] to #valid columns of Qkt(flagged).  No max # weights in a dot-product
  parms =. ((-^:(nflagged<0) 1{$3{::y)) 0} parms
  y =. (((#bkbeta) {."1 |:"2)&.> 3 { y) 3} y  NB. Turn Qk into Qkt
  y =. (<parms) 9} y
  y =. (15!:18&.> 3 6 10 { y) 3 6 10} y  NB. cache-align the blocks that need it
  z =. (2, #bkbeta) 15!:18 (0.)  NB. result area, big enough to hold final batch including fk if present
  'savres zz' =: e128x19 savy =: (<z) 7} y
  if. 4 ~: {. x do.  NB. if the column was aborted, don't check the values
    assert. 0 = +./ 128!:5 , zz
    assert. 1e_25 > >./ +/ | ref epsub&(nrows&{."1) zz  NB. require match on valid values
  end.
  if. #x do.  NB. if SPR result known, check it
    if. 0=nthr do.
      assert. x -: savres  NB. single-threaded - expect exact result
    elseif. #opts do.
      assert. opts e.~ (({:$opts){.0 4) { savres  NB. multithreaded, with alternative results; match as many as are given
    else.
      assert. x -:&(0 4&{) savres  NB. multithreaded, match type and minimizer
    end.
  end.
else.   NB. gradient
  NB.  y is Ax;Am;Av;(M, shape 2,m,n);RVT;bndrowmask;(sched);cutoffinfo;ndx;parms;Frow  where ndx is a list
  NB.   parms is #cols in Qk(flagged),maxAx,Col0Threshold,expandQk,x,MinGradient   expandqk is internal here, means 'put each row into its own test block'
  'Ax Am Av Qk rvt bndrowmask sched ndx parms Frow' =. 0 1 2 3 4 5 6 8 9 10 { y  NB. don't reassign z lest it lose KNOWNNAMED
  NB. We don't support Fk flagging in these tests but we pick it up in the random test
  nrows =. 1{$Qk    NB. calculate all rows of result
  ncol =. ({: $ Qk)  NB. number of columns in basis
  if. 0~:nthr do.   NB. multiple threads: expand Qk with 0 rows to make size threadable, scramble rows in the data.  Keep bndrowmask/bk/bkbeta corresponding to same values
    NB. Create the vector of sources for each new row of Qk etc.  We will first add a harmless row and then draw, where _1 is harmless
    nrows =. #drawvec =. ({~ ?~@#) (1200 + ? 50) {.!._1 i. nrows  NB. update count of rows
  elseif. 3 { parms do.   NB. user requests Qk expansion
   NB. spread rows by NPAR to get accurate value for cutoff.  We check cutoff one time in 64 atoms (16 batches) (subject to change)
   nrows =. #drawvec =. 1j63 #!._1 i. nrows
  else.
    drawvec =. i. nrows  NB. keep input unchanged
  end.
  Qk =. drawvec {"1 _1 Qk ,"_1 (0.)
  bndrowmask =. drawvec { bndrowmask , '4'
NB. obsolete   parms =. nrows 0} parms  NB. restore Fk flag to new length
  y =. (<Qk) 3} y
  NB. convert bitmasks to internal form
  rvt =. {&a.@:#.@:(|."1) ($~  8 ,~ >.@(8 %~ #)) , ,./ rvt e."1 '19',:'01'  NB. create values in littleendian order, 2 bits/col (enforced is LSB)
  bndrowmask =. {&a.@:#.@:(|."1) _8 ]\ , (|."1) 0 2 1 |: 16 4&$"1 ($~  64 ,~ >.@(64 %~ #)) 0 ,~ bndrowmask e. '01'  NB. 0 is for Fk
  y =. (rvt;bndrowmask) 4 5} y
  NB. Transpose Qk.  Extend Qk, to cacheline row length, with 0 extend.  Change parms[0 1] to #valid columns of Qkt(flagged) and max # weights in a dot-product
  parms =. ((1{$3{::y) , (>./ 0, , 1 {"2 Ax)) 0 1} parms
  y =. (((>.&.(%&4) 0{parms) {.!.100."1 |:"2)&.> 3 { y) 3} y  NB. Turn Qk into Qkt, filling row with 100.
  y =. (<parms) 9} y
  y =. (15!:18&.> 3 { y) 3} y  NB. cache-align the blocks that need it
  savy =: y
  'savres zz' =: e128x19 y
  if. #x do.  NB. if SPR result known, check it
    'res cut' =. x
    if. 0=nthr do.
      assert. res -: savres  NB. single-threaded - expect exact result
      assert. cut -: zz  NB. cutoff info too
    elseif. #opts do.
      assert. opts e.~ 0 4&{ savres  NB. multithreaded, with alternative results
    else.
      assert. res -:&(0 4&{) savres  NB. multithreaded, match type and minimizer
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
  Ax =. ,."1 (,.~  [: |.!.0 +/\) 2 2 [ Am =. 2 1  0 4 [ Av =. 0.9 0.1  0 1e_20
  M =. (,:   ] * 1e_20 * i.@#) 0. + (i. 6) ,~ =/~ i. 6  NB. extra row of Fk, used sometimes
  rvt =. '44444444' [ bndrowmask =. '014' {~ 8 ?@$ 3  NB. rvt must match total # columns; bndrowmask the rows in Qk incl Fk
  parms =.        6    0.      0.           1e_25         0.              0.             0.         0.          0.     __      _1
  bkbeta =. (>.&.(%&4) 6) $ 0.
  bk =. ,:~ bkbeta
  beta =. (#rvt) $ 0.  NB. one beta per column
  assert. '' ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';00;parms;bkbeta;beta 
  assert. '' ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';00;(_7 (0}) parms);bkbeta;beta 
  assert. '' ('' run128_9) Ax;Am;Av;M;('1' 0} rvt);bndrowmask;bk;'';00;parms;bkbeta;beta 
  assert. '' ('' run128_9) Ax;Am;Av;M;('9' 0} rvt);bndrowmask;bk;'';00;parms;bkbeta;beta 
  assert. '' ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';01;parms;bkbeta;beta 
  assert. '' ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';01;(_7 (0}) parms);bkbeta;beta 
  assert. '' ('' run128_9) Ax;Am;Av;M;('1' 1} rvt);bndrowmask;bk;'';01;parms;bkbeta;beta 
  assert. '' ('' run128_9) Ax;Am;Av;M;('9' 1} rvt);bndrowmask;bk;'';01;parms;bkbeta;beta 
  assert. '' ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';5;parms;bkbeta;beta 
  assert. '' ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';5;(_7 (0}) parms);bkbeta;beta 
  assert. '' ('' run128_9) Ax;Am;Av;M;('1' 5} rvt);bndrowmask;bk;'';5;parms;bkbeta;beta 
  assert. '' ('' run128_9) Ax;Am;Av;M;('9' 5} rvt);bndrowmask;bk;'';5;parms;bkbeta;beta 
  assert. '' ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';6;parms;bkbeta;beta 
  assert. '' ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';6;(_7 (0}) parms);bkbeta;beta 
  assert. '' ('' run128_9) Ax;Am;Av;M;('1' 6} rvt);bndrowmask;bk;'';6;parms;bkbeta;beta 
  assert. '' ('' run128_9) Ax;Am;Av;M;('9' 6} rvt);bndrowmask;bk;'';6;parms;bkbeta;beta 
  assert. '' ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';7;(1e_10 (3}) parms);bkbeta;beta   NB. Store threshold
  NB. Test every different length.  Different size of M are not so important
  for_l. (2 + i. (*1 T. ''){51 5) , (250 + ?@$/ (*1 T. ''){10 100,:20 1000) do.  NB. l is #rows in Qk including Fk if any
    'nQcol nAcol' =. 10 5  NB. number of cols in Qk, A0
    M =. (*  0.25 < 0 ?@$~ $)  _0.5 + (l,nQcol) ?@$ 0  NB. random values of random sizes, with 25% 0s
    M =. 0. + epcanon (,:   ] * (2^_53) * _0.5 + 0 ?@$~ $) M  NB. append extended part, force to float
    Ax =. ,."1 (,.~    [: |.!.00 +/\) >: nAcol ?@$ nQcol   NB. start,len for each Acol
    'Am Av' =. (+/ , {: Ax) ?@$&.> nQcol,0  NB. random columns and weights 
    NB. y is Ax;Am;Av;(M, shape 2,m,n);RVT;bndrowmask;bk;z;ndx;parms;bkbeta;beta  where ndx is an atom  (1st 8 blocks don't move)
    NB.   parms is #cols,maxAx,Col0Threshold,Store0Thresh,x,ColDangerPivot,ColOkPivot,Bk0Threshold,BkOvershoot,MinSPR,PriRow
    parms =.          _    0.      0.           1e_25      0       0.             0.         0.          0.       __    _1.
    rvt =. '014' {~ (nQcol+nAcol) ?@$ 3
    bndrowmask =. '014' {~ l ?@$ 3
    bkbeta =. (>.&.(%&4) l) $ 0.
    bk =. ,:~ bkbeta
    beta =. (#rvt) $ 0.  NB. one beta per column
    for_ndx. i. #rvt do.  NB. for each basic and non-basic column
      assert. '' ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';ndx;((_1&-^:(?2) nQcol) 0} parms);bkbeta;beta 
    end.
  end.

  NB. SPR.  We need only a single column for each test
  Ax =. 0 2 1 $ 00 [ Am =. 0$00 [ Av =. 0$0.
  M =. 2 {. ,: ,.                1   1e_30  _1   _1    1    1    1    1
  bkbeta =. (>.&.(%&4) 1 { $M) $ 0.    0.   1.2  2.7  1.1   0    0    0
  bk =. 2 {. ,: ($bkbeta) $     0.5    0     1    2    1    1    1    1
  bndrowmask =.                 '4',  '4',  '4', '4', '4', '4', '4', '4' 
  rvt =. ,'4'
  beta =. (#rvt) $ 0.4
  bkbeta =. (0 ?@$~ #bndrowix) bndrowix} bkbeta [ bndrowix =. bndrowmask I.@e. '01'  NB. random bkbeta where not used
  NB.   parms is #cols(flagged),maxAx,Col0Threshold,Store0Thresh,x,ColDangerPivot,ColOkPivot,Bk0Threshold,BkOvershoot,MinSPR,PriRow
  parms =.              1         0.      1e_25           1e_25    0      1e_11        1e_6       1e_12         0.       __   _1.
  assert. 0 0 1 8 0.5 ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';00;parms;bkbeta;beta
  assert. 0 0 1 8 0.5 ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';00;(_2 (0)} parms);bkbeta;beta
  assert. 0 7 1 8 0.5 ('' run128_9) Ax;Am;Av;(1 |."_1 M);rvt;bndrowmask;(1 |."_1 bk);'';00;parms;bkbeta;beta
  assert. 0 4 1 8 1 ('' run128_9) Ax;Am;Av;(1 |."_1 M);rvt;bndrowmask;(1 |."_1 bk);'';00;(_2 (0)} parms);bkbeta;beta
  assert. 0 4 1 8 1 ('' run128_9) Ax;Am;Av;(1 }."_1 (1) |."_1  M);rvt;bndrowmask;(1 |."_1 bk);'';00;parms;bkbeta;beta
  assert. 0 4 1 8 1 ('' run128_9) Ax;Am;Av;(1 }."_1 (1) |."_1  M);rvt;bndrowmask;(1 |."_1 bk);'';00;(_2 (0)} parms);bkbeta;beta
  assert. 0 6 1 8 0.5 ('' run128_9) Ax;Am;Av;(2 |."_1 M);rvt;bndrowmask;(2 |."_1 bk);'';00;(parms);bkbeta;beta
  assert. 0 4 1 8 1 ('' run128_9) Ax;Am;Av;(2 }."_1 (1) |."_1  M);rvt;bndrowmask;(2 |."_1 bk);'';00;parms;bkbeta;beta
  assert. 0 4 1 8 1 ('' run128_9) Ax;Am;Av;(2 }."_1 (1) |."_1  M);rvt;bndrowmask;(2 |."_1 bk);'';00;(_2 (0)} parms);bkbeta;beta
  assert. 0 5 1 8 0.5 ('' run128_9) Ax;Am;Av;(3 |."_1 M);rvt;bndrowmask;(3 |."_1 bk);'';00;(parms);bkbeta;beta
  assert. 0 4 1 8 1 ('' run128_9) Ax;Am;Av;(3 }."_1 (1) |."_1  M);rvt;bndrowmask;(3 |."_1 bk);'';00;parms;bkbeta;beta
  assert. 0 1 1 8 1 ('' run128_9) Ax;Am;Av;(3 }."_1 (1) |."_1  M);rvt;bndrowmask;(3 |."_1 bk);'';00;(_2 (0)} parms);bkbeta;beta
  assert. 0 4 1 8 0.5 ('' run128_9) Ax;Am;Av;(4 |."_1 M);rvt;bndrowmask;(4 |."_1 bk);'';00;(parms);bkbeta;beta
  assert. 0 0 1 4 1 ('' run128_9) Ax;Am;Av;(4 }."_1 (1) |."_1  M);rvt;bndrowmask;(_4 (}."1) 4 |."_1 bk);'';00;parms;(_4 }. bkbeta);beta
  assert. 0 0 1 4 1 ('' run128_9) Ax;Am;Av;(4 }."_1 (1) |."_1  M);rvt;bndrowmask;(4 |."_1 bk);'';00;(_2 (0)} parms);bkbeta;beta
  assert. 0 3 1 8 0.5 ('' run128_9) Ax;Am;Av;(5 |."_1 M);rvt;bndrowmask;(5 |."_1 bk);'';00;(parms);bkbeta;beta
  assert. 0 0 1 4 1 ('' run128_9) Ax;Am;Av;(5 }."_1 (1) |."_1  M);rvt;bndrowmask;(_4 (}."1) 5 |."_1 bk);'';00;parms;(_4 }. bkbeta);beta
  assert. 0 0 1 4 1 ('' run128_9) Ax;Am;Av;(5 }."_1 (1) |."_1  M);rvt;bndrowmask;(_4 (}."1) 5 |."_1 bk);'';00;(_2 (0)} parms);(_4 }. bkbeta);beta
  assert. 0 2 1 8 0.5 ('' run128_9) Ax;Am;Av;(6 |."_1 M);rvt;bndrowmask;(6 |."_1 bk);'';00;(parms);bkbeta;beta
  assert. 0 0 1 4 1 ('' run128_9) Ax;Am;Av;(6 }."_1 (1) |."_1  M);rvt;bndrowmask;(_4 (}."1) 6 |."_1 bk);'';00;parms;(_4 }. bkbeta);beta
  assert. 0 0 1 4 1 ('' run128_9) Ax;Am;Av;(6 }."_1 (1) |."_1  M);rvt;bndrowmask;(_4 (}."1) 6 |."_1 bk);'';00;(_2 (0)} parms);(_4 }. bkbeta);beta
  assert. 0 1 1 8 0.5 ('' run128_9) Ax;Am;Av;(7 |."_1 M);rvt;bndrowmask;(7 |."_1 bk);'';00;(parms);bkbeta;beta
  assert. 0 0 1 4 1 ('' run128_9) Ax;Am;Av;(7 }."_1 (1) |."_1  M);rvt;bndrowmask;(_4 (}."1) 7 |."_1 bk);'';00;parms;(_4 }. bkbeta);beta
NB. obsolete empty  assert. 0 0 1 4 1 ('' run128_9) Ax;Am;Av;(7 }."_1 (1) |."_1  M);rvt;bndrowmask;(_4 (}."1) 7 |."_1 bk);'';00;(_2 (0)} parms);(_4 }. bkbeta);beta

  NB. bound column has hidden row with SPR=beta
  assert. 0 8 1 8 0.4 ('' run128_9) Ax;Am;Av;M;('0' 0} rvt);bndrowmask;bk;'';00;(parms);bkbeta;beta
  beta =. (#rvt) $ 0.75
  assert. 0 0 1 8 0.5 ('' run128_9) Ax;Am;Av;M;('0' 0} rvt);bndrowmask;bk;'';00;(parms);bkbeta;beta
  NB. bound enforced complements the column
  beta =. (#rvt) $ 0.4
  assert. 0 8 1 8 0.4 ('' run128_9) Ax;Am;Av;M;('1' 0} rvt);bndrowmask;bk;'';00;(parms);bkbeta;beta
  beta =. (#rvt) $ 0.75
  assert. 0 8 1 8 0.75 ('' run128_9) Ax;Am;Av;M;('1' 0} rvt);bndrowmask;bk;'';00;(parms);bkbeta;beta
  beta =. (#rvt) $ 1.5
  assert. 0 2 1 8 1.0 ('' run128_9) Ax;Am;Av;M;('1' 0} rvt);bndrowmask;bk;'';00;(parms);bkbeta;beta

  NB. Bound row allows both values
  assert. 0 2 1 8 0.2 ('' run128_9) Ax;Am;Av;M;rvt;('0' 2} bndrowmask);bk;'';00;(parms);bkbeta;beta
  assert. 0 2 1 8 0.2 ('' run128_9) Ax;Am;Av;M;rvt;('1' 2} bndrowmask);bk;'';00;(parms);bkbeta;beta
  assert. 0 0 1 8 0.5 ('' run128_9) Ax;Am;Av;M;rvt;('0' 3} bndrowmask);bk;'';00;(parms);bkbeta;beta
  assert. 0 0 1 8 0.5 ('' run128_9) Ax;Am;Av;M;('0' 0} rvt);('0' 4} bndrowmask);bk;'';00;(parms);bkbeta;beta  NB. nonenforcing
  assert. 0 4 1 8 0.1 ('' run128_9) Ax;Am;Av;M;('1' 0} rvt);('0' 4} bndrowmask);bk;'';00;(parms);bkbeta;beta  NB. enforcing col
  for_r. i. 8 do. assert. (0, (8 | 2-r), 1 8 0.2) ('' run128_9) Ax;Am;Av;(r |."_1 M);rvt;(r |. '1' 2} bndrowmask);(r |."_1 bk);'';00;(parms);(r |. bkbeta);beta end.

  NB. first col not taken unless valid
  M =. 2 {. ,: ,.               1. 0 0 0  0 1. 0 0
  bk =. 2 {. ,: ($bkbeta) $     1.
  assert. 0 5 1 8 1 ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';00;(parms);bkbeta;beta

  Ax =. 0 2 1 $ 00 [ Am =. 0$00 [ Av =. 0$0.
  M =. 2 {. ,: ,.                2.   1    1    1    2    1    1    1
  bkbeta =. (>.&.(%&4) 1 { $M) $ 0.
  bk =. 2 {. ,: ($bkbeta) $     1.
  bndrowmask =. (1 { $ M) $     '4'
  rvt =. ,'4'
  beta =. (#rvt) $ 0.4
  bkbeta =. (0 ?@$~ #bndrowix) bndrowix} bkbeta [ bndrowix =. bndrowmask I.@e. '01'  NB. random bkbeta where not used
  NB.   parms is #cols(flagged),maxAx,Col0Threshold,Store0Thresh,x,ColDangerPivot,ColOkPivot,Bk0Threshold,BkOvershoot,MinSPR,PriRow
  parms =.             1          0.      1e_25           1e_25    0      1e_11        1e_6       1e_12         0.        __    _1.

  NB. tie goes to last, if not in same batch
  assert. 0 4 1 8 0.5 ('' run128_9) Ax;Am;Av;M;rvt;('0' 2} bndrowmask);bk;'';00;(parms);bkbeta;beta
  M =. 2 {. ,: ,.                2.   2    1    1    1    1    1    1
  assert. 0 0 1 8 0.5 ('' run128_9) Ax;Am;Av;M;rvt;('0' 2} bndrowmask);bk;'';00;(parms);bkbeta;beta
  M =. 2 {. ,: ,.                2.   1    1    1    1    2    1    1
  assert. 0 5 1 8 0.5 ('' run128_9) Ax;Am;Av;M;rvt;('0' 2} bndrowmask);bk;'';00;(parms);bkbeta;beta
  M =. 2 {. ,: ,.                2.   1    1    1    1    1    2    1
  assert. 0 6 1 8 0.5 ('' run128_9) Ax;Am;Av;M;rvt;('0' 2} bndrowmask);bk;'';00;(parms);bkbeta;beta
  M =. 2 {. ,: ,.                2.   1    1    1    1    1    1    2
  assert. 0 7 1 8 0.5 ('' run128_9) Ax;Am;Av;M;rvt;('0' 2} bndrowmask);bk;'';00;(parms);bkbeta;beta

  NB. thresholds
  NB.   parms is #cols(flagged),maxAx,Col0Threshold,Store0Thresh,x,ColDangerPivot,ColOkPivot,Bk0Threshold,BkOvershoot,MinSPR,PriRow
  M =. 2 {. ,: ,.              8 $ 1e_4
  bk =. 2 {. ,: ($bkbeta) $    0.5 , 7 # 1.
  assert. 0 0 1 8 5000 ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';00;(parms);bkbeta;beta
  assert. 4 _1 1 8 _ ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';00;(1e_3 (2)} parms);bkbeta;beta
  assert. 1 0 1 8 5000 ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';00;(1e_2 (6)} parms);bkbeta;beta
  assert. 1 0 1 8 5000 ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';00;(1e_3 1e_2 (5 6)} parms);bkbeta;beta
  assert. 4 _2 0 0 0 ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';00;(0.75 0. (7 9)} parms);bkbeta;beta  NB. row 0 now below threshold but 0 cuts off, 0 SPR
  assert. 2 0 1 8 _10000 ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';00;(0.75 (7)} parms);bkbeta;beta  NB. row 0 now below threshold, nonimp
  M =. 2 {. ,: ,.              8 $ 1000.
  bk =. 2 {. ,: ($bkbeta) $    8 $ 1.
  assert. 0 4 1 8 0.001 ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';00;(parms);bkbeta;beta
NB. The following not changed since 128!:9 and the tests are not proted
NB. NB. put nothing here! savy is carried over to the next line
NB.   NB. Verify random pickup of 0 bk values.  This kludge relies on a side effect left by run128_9
NB.   if. 0 = 1 T. '' do.  NB. in multithreaded we don't know what run128_9 does; and single-threaded test is good enough
NB.     tests =. (#~   [: +./"1 </"2) 0.2 0.5 >"(1 2) (100 2,1{$M) ?@$ 0  NB. first row zaps M, second zaps bk.  Remove tests that leave no places where M is not set and bk is
NB.     for_mb. tests do.
NB.       'mx bx' =. <@I."1 mb  NB. indexes to zap
NB.       savy =: (<(15!:18) 2 1 |: 1e_12 (<0;mx)} M) 3} savy  NB. make some values of M small
NB.       savy =: (<(15!:18) 0. (<0;bx)} bk) 6} savy  NB. make some values of bk 0
NB.       pickup =. (1{$M)$00 [ z =. (2, #bkbeta) 15!:18 (0.)
NB.       for. i. 200 do.
NB.         assert. 2 = 0 { res =. 128!:9 (<z) 7} savy
NB.         pickup =. (1{res) (>:@{`[`])} pickup
NB.       end.
NB.       assert. (</ mb) -: * pickup  NB. verify that the filled places are exactly the ones that zapped bk but not m
NB.     end.
NB.   end.
  Ax =. 0 2 1 $ 00 [ Am =. 0$00 [ Av =. 0$0.
  M =. 2 {. ,: ,.               1e_12 1e_10  ,(-2^_36),  _1e_11   0   0   0   0
  bkbeta =. (>.&.(%&4) 1 { $M) $ 0.    0   ,(1+2^_36), 1.2    0   0   0   0
  bk =. 2 {. ,: ($bkbeta) $     1e_3   1       1       1     0   0   0   0
  bndrowmask =. (1 { $ M) $     '4'
  rvt =. ,'4'
  beta =. (#rvt) $ 2.0
  NB.   parms is #cols(flagged),maxAx,Col0Threshold,Store0Thresh,x,ColDangerPivot,ColOkPivot,Bk0Threshold,BkOvershoot,MinSPR,PriRow
  parms =.            1           0.      1e_15           1e_25    0      1e_15        1e_6       1e_12         0.       __   _1.
  assert. 1 0 1 8 1e9 ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';00;(parms);bkbeta;beta  NB. dangerous pivot
  assert. 1 0 1 8 1e9 ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';00;(2e_12 (5)} parms);bkbeta;beta  NB. below dangerous ignored
  assert. 1 1 1 8 1e10 ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';00;(2e_12 2e_3 (2 7)} parms);bkbeta;beta  NB. col 0 disabled
  assert. 0 8 1 8 2 ('' run128_9) Ax;Am;Av;M;('0' 0} rvt);bndrowmask;bk;'';00;(2e_12 2e_3 (2 7)} parms);bkbeta;beta  NB. bound row has hidden SPR=beta
  assert. 0 8 1 8 2 ('' run128_9) Ax;Am;Av;M;('1' 0} rvt);bndrowmask;bk;'';00;(2e_12 2e_3 (2 7)} parms);bkbeta;beta  NB. bound row has hidden SPR=beta
  assert. 1 2 1 8 1 ('' run128_9) Ax;Am;Av;M;('1' 0} rvt);bndrowmask;((2^_36) (<0 2)} bk);'';00;(2e_12 (2)} parms);bkbeta;beta  NB. enforcing value comes up
  assert. 1 2 1 8 1 ('' run128_9) Ax;Am;Av;M;(rvt);('0' 2} bndrowmask);(bk);'';00;(2e_12 (2)} parms);bkbeta;beta  NB. enforcing value comes up

  M =. 2 {. ,: ,.               1e_14  0    0    0    0    0    0    0
  bk =. 2 {. ,: ($bkbeta) $       1.   0    0    0    0    0    0    0
  assert. 1 0 1 8 1e14 ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';00;(parms);bkbeta;beta  NB. dangerous pivot
  M =. 2 {. ,: ,.               1e_14  0    0    0  1e_14  0    0    0
  bk =. 2 {. ,: ($bkbeta) $       1.   0    0    0    0    0    0    0
  assert. 3 4 1 8 _1e14 ((,:3) run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';00;(parms);bkbeta;beta  NB. dangerous, then nonimproving with no safe pivot
  M =. 2 {. ,: ,.               1e_14  0    0    0  1e_14  0  1e_5   0
  assert. 2 6 1 8 _100000 ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';00;(parms);bkbeta;beta  NB. dangerous, then nonimproving safe

  NB. Maximum nonimproving pivot
  Ax =. 0 2 1 $ 00 [ Am =. 0$00 [ Av =. 0$0.
  M =. 2 {. ,: ,.               1e_12 1e_5  ,_1e_4,  _1e_11   0   0   0   0
  bkbeta =. (>.&.(%&4) 1 { $M) $ 0.    0   ,(1+2^_36), 1.2    0   0   0   0
  bk =. 2 {. ,: ($bkbeta) $     1e_13   1       1       1     0   0   0   0
  bndrowmask =. (1 { $ M) $     '4'
  rvt =. ,'4'
  beta =. (#rvt) $ 2.0
  NB.   parms is #cols(flagged),maxAx,Col0Threshold,Store0Thresh,x,ColDangerPivot,ColOkPivot,Bk0Threshold,BkOvershoot,MinSPR,PriRow
  parms =.            1           0.      1e_15           1e_25    0      1e_15        1e_6       1e_12         0.       __   _1.
  assert. 3 0 1 8 _1e12 ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';00;(parms);bkbeta;beta  NB. dangerous nonimp pivot
  assert. 2 1 1 8 _1e5 ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;(_1e_6 (<0 1)} bk);'';00;(parms);bkbeta;beta  NB. nondangerous pivot
  assert. 3 0 1 8 _1e12 ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;(1e_6 (<0 1)} bk);'';00;(parms);bkbeta;beta  NB. dangerous pivot
  assert. 2 1 1 8 _1e5 ('' run128_9) Ax;Am;Av;M;rvt;('0' 2} bndrowmask);(1e_14 (<0 1)} bk);'';00;(parms);bkbeta;beta  NB. nondangerous pivot
  assert. 2 2 1 8 _1e4 ('' run128_9) Ax;Am;Av;M;rvt;('0' 2} bndrowmask);(1e_14 (<0 1)} bk);'';00;(parms);((1+1e_13) 2} bkbeta);beta  NB. nondangerous pivot
  NB. Repeat with different Imp/NonImp thresholds
  NB.   parms is #cols(flagged),maxAx,Col0Threshold,Store0Thresh,x,ColDangerPivot,ColOkPivotImp,ColOkPivotNonImp,Bk0Threshold,BkOvershoot,MinSPR,PriRow
  parms =.            1           0.      1e_15           1e_25    0      1e_15        1e_6           1e_13           1e_12         0.       __   _1.
  assert. 2 0 1 8 _1e12 ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';00;(parms);bkbeta;beta  NB. nondangerous nonimp pivot
  assert. 3 1 1 8 _1e5 ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;(_1e_6 (<0 1)} bk);'';00;(1e_3 (7}) parms);bkbeta;beta  NB. nondangerous pivot
  NB.   parms is #cols(flagged),maxAx,Col0Threshold,Store0Thresh,x,ColDangerPivot,ColOkPivot,Bk0Threshold,BkOvershoot,MinSPR,PriRow
  parms =.            1           0.      1e_15           1e_25    0      1e_15        1e_6       1e_12         0.       __   _1.


  NB. column audit
  cutoffinfo =. (2 ,~ #rvt) $ 0.
  assert. 'index error' -: 1 0 1 8 _1e12 ('' run128_9) etx Ax;Am;Av;M;rvt;bndrowmask;(1e_6 (<0 1)} bk);'';01;(parms);bkbeta;beta
  Ax =. 1 2 1 $ 00 1 [ Am =. 1$00 [ Av =. 1$1.
  rvt =. ,'44'
  beta =. (#rvt) $ 2.0
  assert. 2 1 1 8 _1e5 ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;(_1e_6 (<0 1)} bk);'';01;(parms);bkbeta;beta  NB. nondangerous pivot
  assert. 'index error' -: 1 0 1 8 _1e12 ('' run128_9) etx Ax;Am;Av;M;rvt;bndrowmask;(1e_6 (<0 1)} bk);'';2;(parms);bkbeta;beta

  NB. gradient mode
NB.  y is Ax;Am;Av;(M, shape 2,m,n);RVT;bndrowmask;(sched);cutoffinfo;ndx;parms;Frow  where ndx is a list
  Ax =. 1 2 1 $ 0 2 [ Am =. 2$00 1 [ Av =. 2$2. 5.  NB. Ax must not be empty
  M =. ,:   1.    1    _1    1   NB. Input by rows, i. e. NOT transposed
  M =. M ,  _1.    1   _1   1
  M =. M ,  _1.    1   1    1
  M =. M ,  1.    1    1    _1
  M =. M ,  4 4$0. 0 1 0 * 5 4 3 2
  M =. 2 {. ,: M
  Frow =.  _1.     _2   _3   _4 _7
  bndrowmask =. (1 { $ M) $     '4'
  rvt =. '44444'
NB.   parms is #cols(flagged),maxAx,Col0Threshold,expandQk (testcase option),MinGradient/MinGradImp,x
  parms =.           4          0.      1e_25        1                                  0           0    NB. We expand each row to its own comparison block
  cutoffinfo =. (2 ,~ #rvt) $ 0.
  assert. (0 0 1 512 0.44721359549995792770; _2 ]\ 512 5 0 0 0 0 0 0 0 0 ) ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;'';cutoffinfo;(,00);(parms);Frow
  cutoffinfo =. (2 ,~ #rvt) $ 0.
  assert. (0 4 1 1024 0.64715022892943396826; _2 ]\ 0 0 0 0 0 0 0 0 512 117 ) ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;'';cutoffinfo;(,4);(parms);Frow
  cutoffinfo =. (2 ,~ #rvt) $ 0.
  assert. (0 3 4 1856 1.788854381999831; _2 ]\ 512 5 512 5 320 14 512 5 0 0 ) ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;'';cutoffinfo;0 1 2 3;(parms);Frow
  Frow =.  _4.     _3   _2   _1 _1
  cutoffinfo =. (2 ,~ #rvt) $ 0.
  assert. (0 0 4 704 1.788854381999831; _2 ]\ 512 5 128 3 64 2 0 1 0 0) ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;'';cutoffinfo;0 1 2 3;(parms);Frow
  if. 0 = 1 T. '' do.  NB. can't chain cutoffinfo multithreaded, because the problems change size
    NB. roll cutoffinfo into the next test, to verify it is picked up
    assert. (0 0 4 1216 1.788854381999831; _2 ]\ 512 5 512 5 384 23 512 5 0 0) ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;'';cutoffinfo;3 2 1 0;(parms);Frow  NB. reversed col order
  end.
  NB. bound column adds 1 to total
  cutoffinfo =. (2 ,~ #rvt) $ 0.
  assert. (0 0 4 1920 1.788854381999831; _2 ]\ 512 5 512 6 384 23 512 5 0 0) ('' run128_9) Ax;Am;Av;M;('0' 1} rvt);bndrowmask;'';cutoffinfo;3 2 1 0;(parms);Frow  NB. reversed col order
  NB. bound row doubles the contribution of the row
  Frow =.  _1.     _2   _3   _4 _1
  cutoffinfo =. (2 ,~ #rvt) $ 0.
  assert. (0 3 4 1856 1.788854381999831; _2 ]\ 512 5 512 5 320 23 512 5 0 0) ('' run128_9) Ax;Am;Av;M;rvt;('0' 4} bndrowmask);'';cutoffinfo;0 1 2 3;(parms);Frow

  NB. bndrowmask in detail
  M =. 2 {. ,: 4 {."1 ,. >: i. 128
  Frow =.  ,_1.
  bndrowmask =. (1 { $ M) $     '4'
  rvt =. ,'4'
  rvtb =. {&a.@:#.@:(|."1) ($~  8 ,~ >.@(8 %~ #)) , ,./ rvt e."1 '19',:'01'  NB. create values in littleendian order, 2 bits/col (enforced is LSB)
  bndrowmaskb =. {&a.@:#.@:(|."1) _8 ]\ , (|."1) 0 2 1 |: 16 4&$"1 ($!.0~  64 ,~ >.@(64 %~ #)) 0 ,~ (0) 0} 128 $ 0    NB. 0 is for Fk
NB.   parms is #cols(flagged),maxAx,Col0Threshold,expandQk (testcase option),MinGradient/MinGradImp,x
  parms =.           128          0.      1e_25        1                                  0           0    NB. We expand each row to its own comparison block
  assert. 0 0 1 128 1.18907409169273915565e_3 -: 128!:19 Ax;Am;Av;(15!:18 (11) c. |: {. M);rvtb;bndrowmaskb;'';((2 ,~ #rvt) $ 0.);(,00);(parms);Frow
  bndrowmaskb =. {&a.@:#.@:(|."1) _8 ]\ , (|."1) 0 2 1 |: 16 4&$"1 ($!.0~  64 ,~ >.@(64 %~ #)) 0 ,~ (1) 127} 128 $ 0    NB. 0 is for Fk
  assert. 0 0 1 128 1.17553622522872849160e_3 -: 128!:19 Ax;Am;Av;(15!:18 (11) c. |: {. M);rvtb;bndrowmaskb;'';((2 ,~ #rvt) $ 0.);(,00);(parms);Frow

  NB. MinGradient: 4{parms is mingrad.  Finish any column with higher gradient
  Ax =. 1 2 1 $ 00 1 [ Am =. 1$00 [ Av =. 1$0.  NB. Ax must not be empty
  M =. ,:   1.    1    1    1   NB. Input by rows, i. e. NOT transposed
  M =. M ,  1.    1    1    1
  M =. M ,  1.    1    1    1
  M =. M ,  1.    1    1    1
  M =. M ,  0.    0    5    0
  M =. M ,  3 4$0.
  M =. 2 {. ,: M
  Frow =.  _4.     _3   _2   _1
  bndrowmask =. (1 { $ M) $     '4'
  rvt =. '44444'
NB.   parms is #cols(flagged),maxAx,Col0Threshold,expandQk (testcase option),MinGradient/MinGradImp,x
  parms =.            4         0.      1e_25        1                                  0           0    NB. We expand each row to its own comparison block
  cutoffinfo =. (2 ,~ #rvt) $ 0.
  assert. (0 0 4 704  1.788854381999831; _2 ]\ 512 5 128 3 64 2 0 1 0 0) ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;'';cutoffinfo;0 1 2 3;(parms);Frow
  cutoffinfo =. (2 ,~ #rvt) $ 0.
  assert. (0 _1 4 1856 0; _2 ]\ 512 5 512 5 320 30 512 5 0 0) ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;'';cutoffinfo;0 1 2 3;(0.4 (4}) parms);Frow
  cutoffinfo =. (2 ,~ #rvt) $ 0.
  assert. (0 _1 4 1536 0; _2 ]\ 512 5 512 5 320 30 192 4 0 0) ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;'';cutoffinfo;0 1 2 3;(0.51 (4}) parms);Frow

  NB. MinGradImp
  cutoffinfo =. (2 ,~ #rvt) $ 0.
  assert. (0 0 4 1088  1.788854381999831; _2 ]\ 512 5 512 5 64 2 0 1 0 0) ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;'';cutoffinfo;1 0 2 3;(parms);Frow
  cutoffinfo =. (2 ,~ #rvt) $ 0.
  assert. (0 1 4 704  1.341640786499873838; _2 ]\ 192 4 512 5 0 1 0 1 0 0) ((0 1.341640786499873838,:0 1.788854381999831) run128_9) Ax;Am;Av;M;rvt;bndrowmask;'';cutoffinfo;1 0 2 3;(_0.4 (4}) parms);Frow
  cutoffinfo =. (2 ,~ #rvt) $ 0.
  assert. (0 1 4 832 1.341640786499873838; _2 ]\ 256 5 512 5 64 2 0 1 0 0) ((0 1.341640786499873838,:0 1.788854381999831) run128_9) Ax;Am;Av;M;rvt;bndrowmask;'';cutoffinfo;1 0 2 3;(_0.3 (4}) parms);Frow


  NB. column audit
  cutoffinfo =. (2 ,~ #rvt) $ 0.
NB.   parms is #cols(flagged),maxAx,Col0Threshold,expandQk (testcase option),MinGradient/MinGradImp,x
  parms =.            4         0.      1e_25        0                                  0           0    NB. We expand each row to its own comparison block
  assert. 'index error' -: (0 0 0 0 0; _2 ]\ 10$0) ('' run128_9) etx Ax;Am;Av;M;rvt;bndrowmask;'';cutoffinfo;0 1 2 5;(parms);Frow

  NB. Large random test
  nqcols=.20
  for_nrow. (2000 + 1 ?@$ 512) , (128 + 10 ?@$ 512) do.  NB. varying column length of Qk.  Few cols of Qk, many of A0.  One long col to multithread small # columns
    Qktqp =. 15!:18 (11) c. {. Qkt =. 15!:18 ] 2 {. ,: (>.&.(%&4) nrow) {."1 >: (nqcols,nrow) ?@$ 0  NB. use all positive values to avoid massive cancellation
    Ax =. ,"0 (,.~   [: |.!.0 +/\) 1 >. (256 + ? 255) ?@$ nqcols   NB. array of start,length.  Big enough for multiple reservations
    Am =. ((<1 0)&{"2 Ax) ;@:(?&.>) nqcols  NB. column #s
    Av =. >: ((<1 0)&{"2 Ax) ;@:(?@$&.>) 0  NB. weights, all positive
NB.   parms is #cols(flagged),maxAx,                x,expandQk (testcase option),MinGradient/MinGradImp,x
    parms =.     nrow,      (>./((<1 0)&{"2 Ax)), 0.,0.                               0.             0.
    for_nqacols. (0 ,. 1 + i. 32) , |: 0 1 >. 20 ?@$"0 (nqcols , #Ax) do.  NB. pairs of #Q cols, #Acols.  Acols may not be empty.  Test small # columns
      qacols =. nqacols ?&.> (nqcols , #Ax)  NB. Qk col#s ; A0 col #s
      Yt =. '014' {~ 0.06 0.1 I. nrow ?@$ 0  NB. Random types, mostly normal
      nttcols =. (, +&nqcols)&>/ qacols   NB. column indexes in NTT
      frow =. - 0.2 + (nqcols + #Ax) ?@$ 0  NB. random negative frow
      Vt =. '014' {~ 0.06 0.1 I. (nqcols + #Ax) ?@$ 0  NB. Random types, mostly normal
      NB. Find the largest gradient.  This is what 128!:9 does in gradient mode (not including MinGrad)
      qkcols =. (<0;0{::qacols) { Qkt  NB. cols of Qk
      akcols =. ([: (];.0&Av +/@:* Qkt {~ [: < 0 ; ];.0&Am) {&Ax)"0 ] 1{::qacols  NB. cols of Ak, weighted by Av
      grads =. (|nttcols { frow) % %: ('01' e.~ nttcols { Vt) + 1. + +/"1 (1. + Yt e. '01') *"1 *: nrow {."1 qkcols,akcols
      maxgrad =. grads {~ col =. (i. >./) grads  NB. expected col & gradient
      col =. col { nttcols  NB. convert index in list to value
      NB. end of 128!:9 emulation
      cuti =. (2 ,~ #Vt) $ 0.   NB. init no info on column
      rvt =. {&a.@:#.@:(|."1) ($~  8 ,~ >.@(8 %~ #)) , ,./ Vt e."1 '19',:'01'  NB. create values in littleendian order, 2 bits/col (enforced is LSB)
      bndrowmask =. {&a.@:#.@:(|."1) _8 ]\ , (|."1) 0 2 1 |: 16 4&$"1 ($~  64 ,~ >.@(64 %~ #)) 0 ,~ Yt e. '01'  NB. 0 is for Fk
      'rc9 col9 ncol ndot grad9' =. 128!:19 Ax;Am;Av;Qktqp;rvt;bndrowmask;'';cuti;nttcols;parms;frow
NB. rerun      'rc9 col9 ncol ndot grad9' =. 128!:19 Ax;Am;Av;Qktqp;rvt;bndrowmask;'';((2 ,~ #Vt) $ 0.);nttcols;parms;frow
      assert. (col,maxgrad) -: col9,grad9
    end.
  end.


  NB. end of tests, add a thread
  0 T. ''
end.

delth''  NB. make sure we end with an empty system

1
}} 1

NB. *** 128!:9  (deprecated)

NB. 128!:9  Ax;Am;Av;(M, shape 2,m,n);RVT;bndrowmask;bk;z;ndx;parms;bkbeta ---------------------------------------------------------
NB. y is 128!:9 parms, x is expected result (empty if we have to check column values).  m is parms
NB. Save args, run 128!:9, check result.  For multithreads, expand M/bk/Frow to big enough to engage threading (leaving last row/col harmless)
NB. and expand bkg and ndx to  make enough work to keep the threads busy
run128_9 =: {{
epadd =. (|:~ (_1 |. i.@#@$)) @: ((1.0"0 +/@:*"1!.1 ])@,"1&(0&|:))
epsub =. (epadd -)
savx =: x [ savy =: y
nthr =. 1 T. ''
opts =. m
if. '' -: $ colx =. 8 {::y do.
  NB. onecol+SPR
  NB. y is Ax;Am;Av;(M, shape 2,m,n);RVT;bndrowmask;bk;z;ndx;parms;bkbeta;beta  where ndx is an atom  (1st 8 blocks don't move)
  NB. here RVT is chars and must be converted to mask, same for bndrowmask
  NB. We expect bkbeta to have the correct result item count - extended to batch boundary including Fk if present
  'Ax Am Av Qk rvt bndrowmask bk ndx parms bkbeta' =. 0 1 2 3 4 5 6 8 9 10 { y  NB. don't reassign z lest it lose KNOWNNAMED
  NB.   parms is #cols in Qk(flagged),maxAx,Col0Threshold,Store0Thresh,x,ColDangerPivot,ColOkPivot[Imp,ColOkPivotNonImp],Bk0Threshold,BkOvershoot,MinSPR,PriRow
  if. 11=#parms do. parms =. (2 (6}) 11$01) # parms end.  NB. if test omits NonImp, make it equal Imp
  origparms =: parms  NB. debug
  nflagged =. 0{parms  NB. # rows of Qk, with fk flag
  nrows =. 1{$Qk    NB. calculate all rows of result including Fk if any
  ncol =. ({: $ Qk)  NB. number of columns in basis
  if. 0~:nthr do.   NB. multiple threads: expand Qk with 0 rows to make size threadable, scramble rows in the data.  Keep bndrowmask/bk/bkbeta corresponding to same values
    NB. Create the vector of sources for each new row of Qk etc.  We will first add a harmless row and then draw, where _1 is harmless and _2 is the Fk row if present
    nrows =. #drawvec =. ,&_2^:(nflagged<0) ({~ ?~@#) (12000 + ? 50) {.!._1 i. nrows - (nflagged<0)  NB. modify #rows to match the unpadded block
    NB. modify the inputs
    Qk =. drawvec {"1 _1 Qk ,"_1 (0.)
    bndrowmask =. drawvec { bndrowmask , '4'
    drawvec =. ({.!._1~ >.&.(%&4)@#) drawvec NB. pad bk/bkbeta/z to batch bdy
    bk =. drawvec {"1 _1 bk ,"_1 (0.)
    bkbeta =. drawvec { bkbeta , 0.
NB. obsolete     parms =. (-^:(nflagged < 0) nrows) 0} parms  NB. restore Fk flag to new length
    y =. (Qk;bk;parms;bkbeta) 3 6 9 10} y
  else.   NB. One thread.  just pad bk/bkbeta to cacheline bdy
    bk =. (#bkbeta) {."1 bk [ bkbeta =. ({.~ >.&.(%&4)@#) bkbeta
  end.
  if. colx<ncol do. savref   =: ref =. colx {"1 Qk  NB. basic column, just fetch it
  else.
    qkcols =. (colx-ncol) { Ax  NB. start,length of column indexes/weights
    savref   =: ref =. |: (,~ qkcols ];.0 Av) +/@:*"1!.1 ,.~/ (qkcols ];.0 Am){"1 Qk  NB. fetch cols and combine them
  end.
  if. '19' e.~ colx { rvt do. ref =. - ref end.  NB. change sign of column if enforcing
  savref =: ref =. (3{parms) (((< |) {.) *"1 ]) ref  NB. Clear values below the Store0 threshold
  NB. convert bitmasks to internal form
  rvt =. {&a.@:#.@:(|."1) ($~  8 ,~ >.@(8 %~ #)) , ,./ rvt e."1 '19',:'01'  NB. create values in littleendian order, 2 bits/col (enforced is LSB)
  bndrowmask =. {&a.@:#.@:(|."1) _8 ]\ , (|."1) 0 2 1 |: 16 4&$"1 ($~  64 ,~ >.@(64 %~ #)) 0 ,~ bndrowmask e. '01'  NB. 0 is for Fk
  y =. (rvt;bndrowmask) 4 5} y
  NB. Transpose Qk.  Extend Qk, to cacheline row length, with 0 extend (bk/bkbeta padded already).  Change parms[0 1] to #valid columns of Qkt(flagged).  No max # weights in a dot-product
  parms =. ((-^:(nflagged<0) 1{$3{::y)) 0} parms
  y =. (((#bkbeta) {."1 |:"2)&.> 3 { y) 3} y  NB. Turn Qk into Qkt
  y =. (<parms) 9} y
  y =. (15!:18&.> 3 6 10 { y) 3 6 10} y  NB. cache-align the blocks that need it
  z =. (2, #bkbeta) 15!:18 (0.)  NB. result area, big enough to hold final batch including fk if present
  savres   =: 128!:9 savy =: (<z) 7} y
  if. 4 ~: {. x do.  NB. if the column was aborted, don't check the values
    assert. 0 = +./ 128!:5 , z
    assert. 1e_25 > >./ +/ | ref epsub&(nrows&{."1) z  NB. require match on valid values
  end.
  if. #x do.  NB. if SPR result known, check it
    if. 0=nthr do.
      assert. x -: savres  NB. single-threaded - expect exact result
    elseif. #opts do.
      assert. opts e.~ (({:$opts){.0 4) { savres  NB. multithreaded, with alternative results; match as many as are given
    else.
      assert. x -:&(0 4&{) savres  NB. multithreaded, match type and minimizer
    end.
  end.
else.   NB. gradient
  NB.  y is Ax;Am;Av;(M, shape 2,m,n);RVT;bndrowmask;(sched);cutoffinfo;ndx;parms;Frow  where ndx is a list
  NB.   parms is #cols in Qk(flagged),maxAx,Col0Threshold,expandQk,x,MinGradient   expandqk is internal here, means 'put each row into its own test block'
  'Ax Am Av Qk rvt bndrowmask sched ndx parms Frow' =. 0 1 2 3 4 5 6 8 9 10 { y  NB. don't reassign z lest it lose KNOWNNAMED
  NB. We don't support Fk flagging in these tests but we pick it up in the random test
  nrows =. 1{$Qk    NB. calculate all rows of result
  ncol =. ({: $ Qk)  NB. number of columns in basis
  if. 0~:nthr do.   NB. multiple threads: expand Qk with 0 rows to make size threadable, scramble rows in the data.  Keep bndrowmask/bk/bkbeta corresponding to same values
    NB. Create the vector of sources for each new row of Qk etc.  We will first add a harmless row and then draw, where _1 is harmless
    nrows =. #drawvec =. ({~ ?~@#) (1200 + ? 50) {.!._1 i. nrows  NB. update count of rows
  elseif. 3 { parms do.   NB. user requests Qk expansion
   NB. spread rows by NPAR to get accurate value for cutoff.  We check cutoff one time in 64 atoms (16 batches) (subject to change)
   nrows =. #drawvec =. 1j63 #!._1 i. nrows
  else.
    drawvec =. i. nrows  NB. keep input unchanged
  end.
  Qk =. drawvec {"1 _1 Qk ,"_1 (0.)
  bndrowmask =. drawvec { bndrowmask , '4'
NB. obsolete   parms =. nrows 0} parms  NB. restore Fk flag to new length
  y =. (<Qk) 3} y
  NB. convert bitmasks to internal form
  rvt =. {&a.@:#.@:(|."1) ($~  8 ,~ >.@(8 %~ #)) , ,./ rvt e."1 '19',:'01'  NB. create values in littleendian order, 2 bits/col (enforced is LSB)
  bndrowmask =. {&a.@:#.@:(|."1) _8 ]\ , (|."1) 0 2 1 |: 16 4&$"1 ($~  64 ,~ >.@(64 %~ #)) 0 ,~ bndrowmask e. '01'  NB. 0 is for Fk
  y =. (rvt;bndrowmask) 4 5} y
  NB. Transpose Qk.  Extend Qk, to cacheline row length, with 0 extend.  Change parms[0 1] to #valid columns of Qkt(flagged) and max # weights in a dot-product
  parms =. ((1{$3{::y) , (>./ 0, , 1 {"2 Ax)) 0 1} parms
  y =. (((>.&.(%&4) 0{parms) {.!.100."1 |:"2)&.> 3 { y) 3} y  NB. Turn Qk into Qkt, filling row with 100.
  y =. (<parms) 9} y
  y =. (15!:18&.> 3 { y) 3} y  NB. cache-align the blocks that need it
  savy =: y
  savres   =: 128!:9 y
  if. #x do.  NB. if SPR result known, check it
    'res cut' =. x
    if. 0=nthr do.
      assert. res -: savres  NB. single-threaded - expect exact result
      assert. cut -: 7{::y  NB. cutoff info too
    elseif. #opts do.
      assert. opts e.~ 0 4&{ savres  NB. multithreaded, with alternative results
    else.
      assert. res -:&(0 4&{) savres  NB. multithreaded, match type and minimizer
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
  Ax =. ,."1 (,.~  [: |.!.0 +/\) 2 2 [ Am =. 2 1  0 4 [ Av =. 0.9 0.1  0 1e_20
  M =. (,:   ] * 1e_20 * i.@#) 0. + (i. 6) ,~ =/~ i. 6  NB. extra row of Fk, used sometimes
  rvt =. '44444444' [ bndrowmask =. '014' {~ 8 ?@$ 3  NB. rvt must match total # columns; bndrowmask the rows in Qk incl Fk
  parms =.        6    0.      0.           1e_25         0.              0.             0.         0.          0.     __      _1
  bkbeta =. (>.&.(%&4) 6) $ 0.
  bk =. ,:~ bkbeta
  beta =. (#rvt) $ 0.  NB. one beta per column
  assert. '' ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';00;parms;bkbeta;beta 
  assert. '' ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';00;(_7 (0}) parms);bkbeta;beta 
  assert. '' ('' run128_9) Ax;Am;Av;M;('1' 0} rvt);bndrowmask;bk;'';00;parms;bkbeta;beta 
  assert. '' ('' run128_9) Ax;Am;Av;M;('9' 0} rvt);bndrowmask;bk;'';00;parms;bkbeta;beta 
  assert. '' ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';01;parms;bkbeta;beta 
  assert. '' ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';01;(_7 (0}) parms);bkbeta;beta 
  assert. '' ('' run128_9) Ax;Am;Av;M;('1' 1} rvt);bndrowmask;bk;'';01;parms;bkbeta;beta 
  assert. '' ('' run128_9) Ax;Am;Av;M;('9' 1} rvt);bndrowmask;bk;'';01;parms;bkbeta;beta 
  assert. '' ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';5;parms;bkbeta;beta 
  assert. '' ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';5;(_7 (0}) parms);bkbeta;beta 
  assert. '' ('' run128_9) Ax;Am;Av;M;('1' 5} rvt);bndrowmask;bk;'';5;parms;bkbeta;beta 
  assert. '' ('' run128_9) Ax;Am;Av;M;('9' 5} rvt);bndrowmask;bk;'';5;parms;bkbeta;beta 
  assert. '' ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';6;parms;bkbeta;beta 
  assert. '' ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';6;(_7 (0}) parms);bkbeta;beta 
  assert. '' ('' run128_9) Ax;Am;Av;M;('1' 6} rvt);bndrowmask;bk;'';6;parms;bkbeta;beta 
  assert. '' ('' run128_9) Ax;Am;Av;M;('9' 6} rvt);bndrowmask;bk;'';6;parms;bkbeta;beta 
  assert. '' ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';7;(1e_10 (3}) parms);bkbeta;beta   NB. Store threshold
  NB. Test every different length.  Different size of M are not so important
  for_l. (2 + i. (*1 T. ''){51 5) , (250 + ?@$/ (*1 T. ''){10 100,:20 1000) do.  NB. l is #rows in Qk including Fk if any
    'nQcol nAcol' =. 10 5  NB. number of cols in Qk, A0
    M =. (*  0.25 < 0 ?@$~ $)  _0.5 + (l,nQcol) ?@$ 0  NB. random values of random sizes, with 25% 0s
    M =. 0. + epcanon (,:   ] * (2^_53) * _0.5 + 0 ?@$~ $) M  NB. append extended part, force to float
    Ax =. ,."1 (,.~    [: |.!.00 +/\) >: nAcol ?@$ nQcol   NB. start,len for each Acol
    'Am Av' =. (+/ , {: Ax) ?@$&.> nQcol,0  NB. random columns and weights 
    NB. y is Ax;Am;Av;(M, shape 2,m,n);RVT;bndrowmask;bk;z;ndx;parms;bkbeta;beta  where ndx is an atom  (1st 8 blocks don't move)
    NB.   parms is #cols,maxAx,Col0Threshold,Store0Thresh,x,ColDangerPivot,ColOkPivot,Bk0Threshold,BkOvershoot,MinSPR,PriRow
    parms =.          _    0.      0.           1e_25      0       0.             0.         0.          0.       __    _1.
    rvt =. '014' {~ (nQcol+nAcol) ?@$ 3
    bndrowmask =. '014' {~ l ?@$ 3
    bkbeta =. (>.&.(%&4) l) $ 0.
    bk =. ,:~ bkbeta
    beta =. (#rvt) $ 0.  NB. one beta per column
    for_ndx. i. #rvt do.  NB. for each basic and non-basic column
      assert. '' ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';ndx;((_1&-^:(?2) nQcol) 0} parms);bkbeta;beta 
    end.
  end.

  NB. SPR.  We need only a single column for each test
  Ax =. 0 2 1 $ 00 [ Am =. 0$00 [ Av =. 0$0.
  M =. 2 {. ,: ,.                1   1e_30  _1   _1    1    1    1    1
  bkbeta =. (>.&.(%&4) 1 { $M) $ 0.    0.   1.2  2.7  1.1   0    0    0
  bk =. 2 {. ,: ($bkbeta) $     0.5    0     1    2    1    1    1    1
  bndrowmask =.                 '4',  '4',  '4', '4', '4', '4', '4', '4' 
  rvt =. ,'4'
  beta =. (#rvt) $ 0.4
  bkbeta =. (0 ?@$~ #bndrowix) bndrowix} bkbeta [ bndrowix =. bndrowmask I.@e. '01'  NB. random bkbeta where not used
  NB.   parms is #cols(flagged),maxAx,Col0Threshold,Store0Thresh,x,ColDangerPivot,ColOkPivot,Bk0Threshold,BkOvershoot,MinSPR,PriRow
  parms =.              1         0.      1e_25           1e_25    0      1e_11        1e_6       1e_12         0.       __   _1.
  assert. 0 0 1 8 0.5 ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';00;parms;bkbeta;beta
  assert. 0 0 1 8 0.5 ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';00;(_2 (0)} parms);bkbeta;beta
  assert. 0 7 1 8 0.5 ('' run128_9) Ax;Am;Av;(1 |."_1 M);rvt;bndrowmask;(1 |."_1 bk);'';00;parms;bkbeta;beta
  assert. 0 4 1 8 1 ('' run128_9) Ax;Am;Av;(1 |."_1 M);rvt;bndrowmask;(1 |."_1 bk);'';00;(_2 (0)} parms);bkbeta;beta
  assert. 0 4 1 8 1 ('' run128_9) Ax;Am;Av;(1 }."_1 (1) |."_1  M);rvt;bndrowmask;(1 |."_1 bk);'';00;parms;bkbeta;beta
  assert. 0 4 1 8 1 ('' run128_9) Ax;Am;Av;(1 }."_1 (1) |."_1  M);rvt;bndrowmask;(1 |."_1 bk);'';00;(_2 (0)} parms);bkbeta;beta
  assert. 0 6 1 8 0.5 ('' run128_9) Ax;Am;Av;(2 |."_1 M);rvt;bndrowmask;(2 |."_1 bk);'';00;(parms);bkbeta;beta
  assert. 0 4 1 8 1 ('' run128_9) Ax;Am;Av;(2 }."_1 (1) |."_1  M);rvt;bndrowmask;(2 |."_1 bk);'';00;parms;bkbeta;beta
  assert. 0 4 1 8 1 ('' run128_9) Ax;Am;Av;(2 }."_1 (1) |."_1  M);rvt;bndrowmask;(2 |."_1 bk);'';00;(_2 (0)} parms);bkbeta;beta
  assert. 0 5 1 8 0.5 ('' run128_9) Ax;Am;Av;(3 |."_1 M);rvt;bndrowmask;(3 |."_1 bk);'';00;(parms);bkbeta;beta
  assert. 0 4 1 8 1 ('' run128_9) Ax;Am;Av;(3 }."_1 (1) |."_1  M);rvt;bndrowmask;(3 |."_1 bk);'';00;parms;bkbeta;beta
  assert. 0 1 1 8 1 ('' run128_9) Ax;Am;Av;(3 }."_1 (1) |."_1  M);rvt;bndrowmask;(3 |."_1 bk);'';00;(_2 (0)} parms);bkbeta;beta
  assert. 0 4 1 8 0.5 ('' run128_9) Ax;Am;Av;(4 |."_1 M);rvt;bndrowmask;(4 |."_1 bk);'';00;(parms);bkbeta;beta
  assert. 0 0 1 4 1 ('' run128_9) Ax;Am;Av;(4 }."_1 (1) |."_1  M);rvt;bndrowmask;(_4 (}."1) 4 |."_1 bk);'';00;parms;(_4 }. bkbeta);beta
  assert. 0 0 1 4 1 ('' run128_9) Ax;Am;Av;(4 }."_1 (1) |."_1  M);rvt;bndrowmask;(4 |."_1 bk);'';00;(_2 (0)} parms);bkbeta;beta
  assert. 0 3 1 8 0.5 ('' run128_9) Ax;Am;Av;(5 |."_1 M);rvt;bndrowmask;(5 |."_1 bk);'';00;(parms);bkbeta;beta
  assert. 0 0 1 4 1 ('' run128_9) Ax;Am;Av;(5 }."_1 (1) |."_1  M);rvt;bndrowmask;(_4 (}."1) 5 |."_1 bk);'';00;parms;(_4 }. bkbeta);beta
  assert. 0 0 1 4 1 ('' run128_9) Ax;Am;Av;(5 }."_1 (1) |."_1  M);rvt;bndrowmask;(_4 (}."1) 5 |."_1 bk);'';00;(_2 (0)} parms);(_4 }. bkbeta);beta
  assert. 0 2 1 8 0.5 ('' run128_9) Ax;Am;Av;(6 |."_1 M);rvt;bndrowmask;(6 |."_1 bk);'';00;(parms);bkbeta;beta
  assert. 0 0 1 4 1 ('' run128_9) Ax;Am;Av;(6 }."_1 (1) |."_1  M);rvt;bndrowmask;(_4 (}."1) 6 |."_1 bk);'';00;parms;(_4 }. bkbeta);beta
  assert. 0 0 1 4 1 ('' run128_9) Ax;Am;Av;(6 }."_1 (1) |."_1  M);rvt;bndrowmask;(_4 (}."1) 6 |."_1 bk);'';00;(_2 (0)} parms);(_4 }. bkbeta);beta
  assert. 0 1 1 8 0.5 ('' run128_9) Ax;Am;Av;(7 |."_1 M);rvt;bndrowmask;(7 |."_1 bk);'';00;(parms);bkbeta;beta
  assert. 0 0 1 4 1 ('' run128_9) Ax;Am;Av;(7 }."_1 (1) |."_1  M);rvt;bndrowmask;(_4 (}."1) 7 |."_1 bk);'';00;parms;(_4 }. bkbeta);beta
NB. obsolete empty  assert. 0 0 1 4 1 ('' run128_9) Ax;Am;Av;(7 }."_1 (1) |."_1  M);rvt;bndrowmask;(_4 (}."1) 7 |."_1 bk);'';00;(_2 (0)} parms);(_4 }. bkbeta);beta

  NB. bound column has hidden row with SPR=beta
  assert. 0 8 1 8 0.4 ('' run128_9) Ax;Am;Av;M;('0' 0} rvt);bndrowmask;bk;'';00;(parms);bkbeta;beta
  beta =. (#rvt) $ 0.75
  assert. 0 0 1 8 0.5 ('' run128_9) Ax;Am;Av;M;('0' 0} rvt);bndrowmask;bk;'';00;(parms);bkbeta;beta
  NB. bound enforced complements the column
  beta =. (#rvt) $ 0.4
  assert. 0 8 1 8 0.4 ('' run128_9) Ax;Am;Av;M;('1' 0} rvt);bndrowmask;bk;'';00;(parms);bkbeta;beta
  beta =. (#rvt) $ 0.75
  assert. 0 8 1 8 0.75 ('' run128_9) Ax;Am;Av;M;('1' 0} rvt);bndrowmask;bk;'';00;(parms);bkbeta;beta
  beta =. (#rvt) $ 1.5
  assert. 0 2 1 8 1.0 ('' run128_9) Ax;Am;Av;M;('1' 0} rvt);bndrowmask;bk;'';00;(parms);bkbeta;beta

  NB. Bound row allows both values
  assert. 0 2 1 8 0.2 ('' run128_9) Ax;Am;Av;M;rvt;('0' 2} bndrowmask);bk;'';00;(parms);bkbeta;beta
  assert. 0 2 1 8 0.2 ('' run128_9) Ax;Am;Av;M;rvt;('1' 2} bndrowmask);bk;'';00;(parms);bkbeta;beta
  assert. 0 0 1 8 0.5 ('' run128_9) Ax;Am;Av;M;rvt;('0' 3} bndrowmask);bk;'';00;(parms);bkbeta;beta
  assert. 0 0 1 8 0.5 ('' run128_9) Ax;Am;Av;M;('0' 0} rvt);('0' 4} bndrowmask);bk;'';00;(parms);bkbeta;beta  NB. nonenforcing
  assert. 0 4 1 8 0.1 ('' run128_9) Ax;Am;Av;M;('1' 0} rvt);('0' 4} bndrowmask);bk;'';00;(parms);bkbeta;beta  NB. enforcing col
  for_r. i. 8 do. assert. (0, (8 | 2-r), 1 8 0.2) ('' run128_9) Ax;Am;Av;(r |."_1 M);rvt;(r |. '1' 2} bndrowmask);(r |."_1 bk);'';00;(parms);(r |. bkbeta);beta end.

  NB. first col not taken unless valid
  M =. 2 {. ,: ,.               1. 0 0 0  0 1. 0 0
  bk =. 2 {. ,: ($bkbeta) $     1.
  assert. 0 5 1 8 1 ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';00;(parms);bkbeta;beta

  Ax =. 0 2 1 $ 00 [ Am =. 0$00 [ Av =. 0$0.
  M =. 2 {. ,: ,.                2.   1    1    1    2    1    1    1
  bkbeta =. (>.&.(%&4) 1 { $M) $ 0.
  bk =. 2 {. ,: ($bkbeta) $     1.
  bndrowmask =. (1 { $ M) $     '4'
  rvt =. ,'4'
  beta =. (#rvt) $ 0.4
  bkbeta =. (0 ?@$~ #bndrowix) bndrowix} bkbeta [ bndrowix =. bndrowmask I.@e. '01'  NB. random bkbeta where not used
  NB.   parms is #cols(flagged),maxAx,Col0Threshold,Store0Thresh,x,ColDangerPivot,ColOkPivot,Bk0Threshold,BkOvershoot,MinSPR,PriRow
  parms =.             1          0.      1e_25           1e_25    0      1e_11        1e_6       1e_12         0.        __    _1.

  NB. tie goes to last, if not in same batch
  assert. 0 4 1 8 0.5 ('' run128_9) Ax;Am;Av;M;rvt;('0' 2} bndrowmask);bk;'';00;(parms);bkbeta;beta
  M =. 2 {. ,: ,.                2.   2    1    1    1    1    1    1
  assert. 0 0 1 8 0.5 ('' run128_9) Ax;Am;Av;M;rvt;('0' 2} bndrowmask);bk;'';00;(parms);bkbeta;beta
  M =. 2 {. ,: ,.                2.   1    1    1    1    2    1    1
  assert. 0 5 1 8 0.5 ('' run128_9) Ax;Am;Av;M;rvt;('0' 2} bndrowmask);bk;'';00;(parms);bkbeta;beta
  M =. 2 {. ,: ,.                2.   1    1    1    1    1    2    1
  assert. 0 6 1 8 0.5 ('' run128_9) Ax;Am;Av;M;rvt;('0' 2} bndrowmask);bk;'';00;(parms);bkbeta;beta
  M =. 2 {. ,: ,.                2.   1    1    1    1    1    1    2
  assert. 0 7 1 8 0.5 ('' run128_9) Ax;Am;Av;M;rvt;('0' 2} bndrowmask);bk;'';00;(parms);bkbeta;beta

  NB. thresholds
  NB.   parms is #cols(flagged),maxAx,Col0Threshold,Store0Thresh,x,ColDangerPivot,ColOkPivot,Bk0Threshold,BkOvershoot,MinSPR,PriRow
  M =. 2 {. ,: ,.              8 $ 1e_4
  bk =. 2 {. ,: ($bkbeta) $    0.5 , 7 # 1.
  assert. 0 0 1 8 5000 ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';00;(parms);bkbeta;beta
  assert. 4 _1 1 8 _ ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';00;(1e_3 (2)} parms);bkbeta;beta
  assert. 1 0 1 8 5000 ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';00;(1e_2 (6)} parms);bkbeta;beta
  assert. 1 0 1 8 5000 ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';00;(1e_3 1e_2 (5 6)} parms);bkbeta;beta
  assert. 4 _2 0 0 0 ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';00;(0.75 0. (7 9)} parms);bkbeta;beta  NB. row 0 now below threshold but 0 cuts off, 0 SPR
  assert. 2 0 1 8 _10000 ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';00;(0.75 (7)} parms);bkbeta;beta  NB. row 0 now below threshold, nonimp
  M =. 2 {. ,: ,.              8 $ 1000.
  bk =. 2 {. ,: ($bkbeta) $    8 $ 1.
  assert. 0 4 1 8 0.001 ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';00;(parms);bkbeta;beta
NB. put nothing here! savy is carried over to the next line
  NB. Verify random pickup of 0 bk values.  This kludge relies on a side effect left by run128_9
  if. 0 = 1 T. '' do.  NB. in multithreaded we don't know what run128_9 does; and single-threaded test is good enough
    tests =. (#~   [: +./"1 </"2) 0.2 0.5 >"(1 2) (100 2,1{$M) ?@$ 0  NB. first row zaps M, second zaps bk.  Remove tests that leave no places where M is not set and bk is
    for_mb. tests do.
      'mx bx' =. <@I."1 mb  NB. indexes to zap
      savy =: (<(15!:18) 2 1 |: 1e_12 (<0;mx)} M) 3} savy  NB. make some values of M small
      savy =: (<(15!:18) 0. (<0;bx)} bk) 6} savy  NB. make some values of bk 0
      pickup =. (1{$M)$00 [ z =. (2, #bkbeta) 15!:18 (0.)
      for. i. 200 do.
        assert. 2 = 0 { res =. 128!:9 (<z) 7} savy
        pickup =. (1{res) (>:@{`[`])} pickup
      end.
      assert. (</ mb) -: * pickup  NB. verify that the filled places are exactly the ones that zapped bk but not m
    end.
  end.
  Ax =. 0 2 1 $ 00 [ Am =. 0$00 [ Av =. 0$0.
  M =. 2 {. ,: ,.               1e_12 1e_10  ,(-2^_36),  _1e_11   0   0   0   0
  bkbeta =. (>.&.(%&4) 1 { $M) $ 0.    0   ,(1+2^_36), 1.2    0   0   0   0
  bk =. 2 {. ,: ($bkbeta) $     1e_3   1       1       1     0   0   0   0
  bndrowmask =. (1 { $ M) $     '4'
  rvt =. ,'4'
  beta =. (#rvt) $ 2.0
  NB.   parms is #cols(flagged),maxAx,Col0Threshold,Store0Thresh,x,ColDangerPivot,ColOkPivot,Bk0Threshold,BkOvershoot,MinSPR,PriRow
  parms =.            1           0.      1e_15           1e_25    0      1e_15        1e_6       1e_12         0.       __   _1.
  assert. 1 0 1 8 1e9 ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';00;(parms);bkbeta;beta  NB. dangerous pivot
  assert. 1 0 1 8 1e9 ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';00;(2e_12 (5)} parms);bkbeta;beta  NB. below dangerous ignored
  assert. 1 1 1 8 1e10 ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';00;(2e_12 2e_3 (2 7)} parms);bkbeta;beta  NB. col 0 disabled
  assert. 0 8 1 8 2 ('' run128_9) Ax;Am;Av;M;('0' 0} rvt);bndrowmask;bk;'';00;(2e_12 2e_3 (2 7)} parms);bkbeta;beta  NB. bound row has hidden SPR=beta
  assert. 0 8 1 8 2 ('' run128_9) Ax;Am;Av;M;('1' 0} rvt);bndrowmask;bk;'';00;(2e_12 2e_3 (2 7)} parms);bkbeta;beta  NB. bound row has hidden SPR=beta
  assert. 1 2 1 8 1 ('' run128_9) Ax;Am;Av;M;('1' 0} rvt);bndrowmask;((2^_36) (<0 2)} bk);'';00;(2e_12 (2)} parms);bkbeta;beta  NB. enforcing value comes up
  assert. 1 2 1 8 1 ('' run128_9) Ax;Am;Av;M;(rvt);('0' 2} bndrowmask);(bk);'';00;(2e_12 (2)} parms);bkbeta;beta  NB. enforcing value comes up

  M =. 2 {. ,: ,.               1e_14  0    0    0    0    0    0    0
  bk =. 2 {. ,: ($bkbeta) $       1.   0    0    0    0    0    0    0
  assert. 1 0 1 8 1e14 ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';00;(parms);bkbeta;beta  NB. dangerous pivot
  M =. 2 {. ,: ,.               1e_14  0    0    0  1e_14  0    0    0
  bk =. 2 {. ,: ($bkbeta) $       1.   0    0    0    0    0    0    0
  assert. 3 4 1 8 _1e14 ((,:3) run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';00;(parms);bkbeta;beta  NB. dangerous, then nonimproving with no safe pivot
  M =. 2 {. ,: ,.               1e_14  0    0    0  1e_14  0  1e_5   0
  assert. 2 6 1 8 _100000 ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';00;(parms);bkbeta;beta  NB. dangerous, then nonimproving safe

  NB. Maximum nonimproving pivot
  Ax =. 0 2 1 $ 00 [ Am =. 0$00 [ Av =. 0$0.
  M =. 2 {. ,: ,.               1e_12 1e_5  ,_1e_4,  _1e_11   0   0   0   0
  bkbeta =. (>.&.(%&4) 1 { $M) $ 0.    0   ,(1+2^_36), 1.2    0   0   0   0
  bk =. 2 {. ,: ($bkbeta) $     1e_13   1       1       1     0   0   0   0
  bndrowmask =. (1 { $ M) $     '4'
  rvt =. ,'4'
  beta =. (#rvt) $ 2.0
  NB.   parms is #cols(flagged),maxAx,Col0Threshold,Store0Thresh,x,ColDangerPivot,ColOkPivot,Bk0Threshold,BkOvershoot,MinSPR,PriRow
  parms =.            1           0.      1e_15           1e_25    0      1e_15        1e_6       1e_12         0.       __   _1.
  assert. 3 0 1 8 _1e12 ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';00;(parms);bkbeta;beta  NB. dangerous nonimp pivot
  assert. 2 1 1 8 _1e5 ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;(_1e_6 (<0 1)} bk);'';00;(parms);bkbeta;beta  NB. nondangerous pivot
  assert. 3 0 1 8 _1e12 ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;(1e_6 (<0 1)} bk);'';00;(parms);bkbeta;beta  NB. dangerous pivot
  assert. 2 1 1 8 _1e5 ('' run128_9) Ax;Am;Av;M;rvt;('0' 2} bndrowmask);(1e_14 (<0 1)} bk);'';00;(parms);bkbeta;beta  NB. nondangerous pivot
  assert. 2 2 1 8 _1e4 ('' run128_9) Ax;Am;Av;M;rvt;('0' 2} bndrowmask);(1e_14 (<0 1)} bk);'';00;(parms);((1+1e_13) 2} bkbeta);beta  NB. nondangerous pivot
  NB. Repeat with different Imp/NonImp thresholds
  NB.   parms is #cols(flagged),maxAx,Col0Threshold,Store0Thresh,x,ColDangerPivot,ColOkPivotImp,ColOkPivotNonImp,Bk0Threshold,BkOvershoot,MinSPR,PriRow
  parms =.            1           0.      1e_15           1e_25    0      1e_15        1e_6           1e_13           1e_12         0.       __   _1.
  assert. 2 0 1 8 _1e12 ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';00;(parms);bkbeta;beta  NB. nondangerous nonimp pivot
  assert. 3 1 1 8 _1e5 ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;(_1e_6 (<0 1)} bk);'';00;(1e_3 (7}) parms);bkbeta;beta  NB. nondangerous pivot
  NB.   parms is #cols(flagged),maxAx,Col0Threshold,Store0Thresh,x,ColDangerPivot,ColOkPivot,Bk0Threshold,BkOvershoot,MinSPR,PriRow
  parms =.            1           0.      1e_15           1e_25    0      1e_15        1e_6       1e_12         0.       __   _1.


  NB. column audit
  cutoffinfo =. (2 ,~ #rvt) $ 0.
  assert. 'index error' -: 1 0 1 8 _1e12 ('' run128_9) etx Ax;Am;Av;M;rvt;bndrowmask;(1e_6 (<0 1)} bk);'';01;(parms);bkbeta;beta
  Ax =. 1 2 1 $ 00 1 [ Am =. 1$00 [ Av =. 1$1.
  rvt =. ,'44'
  beta =. (#rvt) $ 2.0
  assert. 2 1 1 8 _1e5 ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;(_1e_6 (<0 1)} bk);'';01;(parms);bkbeta;beta  NB. nondangerous pivot
  assert. 'index error' -: 1 0 1 8 _1e12 ('' run128_9) etx Ax;Am;Av;M;rvt;bndrowmask;(1e_6 (<0 1)} bk);'';2;(parms);bkbeta;beta

  NB. gradient mode
NB.  y is Ax;Am;Av;(M, shape 2,m,n);RVT;bndrowmask;(sched);cutoffinfo;ndx;parms;Frow  where ndx is a list
  Ax =. 1 2 1 $ 0 2 [ Am =. 2$00 1 [ Av =. 2$2. 5.  NB. Ax must not be empty
  M =. ,:   1.    1    _1    1   NB. Input by rows, i. e. NOT transposed
  M =. M ,  _1.    1   _1   1
  M =. M ,  _1.    1   1    1
  M =. M ,  1.    1    1    _1
  M =. M ,  0.    0    5    0
  M =. M ,  3 4$0.
  M =. 2 {. ,: M
  Frow =.  _1.     _2   _3   _4 _7
  bndrowmask =. (1 { $ M) $     '4'
  rvt =. '44444'
NB.   parms is #cols(flagged),maxAx,Col0Threshold,expandQk (testcase option),MinGradient/MinGradImp,x
  parms =.           4          0.      1e_25        1                                  0           0    NB. We expand each row to its own comparison block
  cutoffinfo =. (2 ,~ #rvt) $ 0.
  assert. (0 0 1 512 0.44721359549995792770; _2 ]\ 512 5 0 0 0 0 0 0 0 0 ) ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;'';cutoffinfo;(,00);(parms);Frow
  cutoffinfo =. (2 ,~ #rvt) $ 0.
  assert. (0 4 1 1024 0.64715022892943396826; _2 ]\ 0 0 0 0 0 0 0 0 512 117 ) ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;'';cutoffinfo;(,4);(parms);Frow
  cutoffinfo =. (2 ,~ #rvt) $ 0.
  assert. (0 3 4 1856 1.788854381999831; _2 ]\ 512 5 512 5 320 30 512 5 0 0 ) ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;'';cutoffinfo;0 1 2 3;(parms);Frow
  Frow =.  _4.     _3   _2   _1 _1
  cutoffinfo =. (2 ,~ #rvt) $ 0.
  assert. (0 0 4 704 1.788854381999831; _2 ]\ 512 5 128 3 64 2 0 1 0 0) ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;'';cutoffinfo;0 1 2 3;(parms);Frow
  if. 0 = 1 T. '' do.  NB. can't chain cutoffinfo multithreaded, because the problems change size
    NB. roll cutoffinfo into the next test, to verify it is picked up
    assert. (0 0 4 1152 1.788854381999831; _2 ]\ 512 5 512 5 320 30 512 5 0 0) ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;'';cutoffinfo;3 2 1 0;(parms);Frow  NB. reversed col order
  end.
  NB. bound column adds 1 to total
  cutoffinfo =. (2 ,~ #rvt) $ 0.
  assert. (0 0 4 1856 1.788854381999831; _2 ]\ 512 5 512 6 320 30 512 5 0 0) ('' run128_9) Ax;Am;Av;M;('0' 1} rvt);bndrowmask;'';cutoffinfo;3 2 1 0;(parms);Frow  NB. reversed col order
  NB. bound row doubles the contribution of the row
  Frow =.  _1.     _2   _3   _4 _1
  cutoffinfo =. (2 ,~ #rvt) $ 0.
  assert. (0 3 4 1856 1.788854381999831; _2 ]\ 512 5 512 5 320 55 512 5 0 0) ('' run128_9) Ax;Am;Av;M;rvt;('0' 4} bndrowmask);'';cutoffinfo;0 1 2 3;(parms);Frow


  NB. MinGradient: 4{parms is mingrad.  Finish any column with higher gradient
  Ax =. 1 2 1 $ 00 1 [ Am =. 1$00 [ Av =. 1$0.  NB. Ax must not be empty
  M =. ,:   1.    1    1    1   NB. Input by rows, i. e. NOT transposed
  M =. M ,  1.    1    1    1
  M =. M ,  1.    1    1    1
  M =. M ,  1.    1    1    1
  M =. M ,  0.    0    5    0
  M =. M ,  3 4$0.
  M =. 2 {. ,: M
  Frow =.  _4.     _3   _2   _1
  bndrowmask =. (1 { $ M) $     '4'
  rvt =. '44444'
NB.   parms is #cols(flagged),maxAx,Col0Threshold,expandQk (testcase option),MinGradient/MinGradImp,x
  parms =.            4         0.      1e_25        1                                  0           0    NB. We expand each row to its own comparison block
  cutoffinfo =. (2 ,~ #rvt) $ 0.
  assert. (0 0 4 704  1.788854381999831; _2 ]\ 512 5 128 3 64 2 0 1 0 0) ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;'';cutoffinfo;0 1 2 3;(parms);Frow
  cutoffinfo =. (2 ,~ #rvt) $ 0.
  assert. (0 _1 4 1856 0; _2 ]\ 512 5 512 5 320 30 512 5 0 0) ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;'';cutoffinfo;0 1 2 3;(0.4 (4}) parms);Frow
  cutoffinfo =. (2 ,~ #rvt) $ 0.
  assert. (0 _1 4 1536 0; _2 ]\ 512 5 512 5 320 30 192 4 0 0) ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;'';cutoffinfo;0 1 2 3;(0.51 (4}) parms);Frow

  NB. MinGradImp
  cutoffinfo =. (2 ,~ #rvt) $ 0.
  assert. (0 0 4 1088  1.788854381999831; _2 ]\ 512 5 512 5 64 2 0 1 0 0) ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;'';cutoffinfo;1 0 2 3;(parms);Frow
  cutoffinfo =. (2 ,~ #rvt) $ 0.
  assert. (0 1 4 704  1.341640786499873838; _2 ]\ 192 4 512 5 0 1 0 1 0 0) ((0 1.341640786499873838,:0 1.788854381999831) run128_9) Ax;Am;Av;M;rvt;bndrowmask;'';cutoffinfo;1 0 2 3;(_0.4 (4}) parms);Frow
  cutoffinfo =. (2 ,~ #rvt) $ 0.
  assert. (0 1 4 832 1.341640786499873838; _2 ]\ 256 5 512 5 64 2 0 1 0 0) ((0 1.341640786499873838,:0 1.788854381999831) run128_9) Ax;Am;Av;M;rvt;bndrowmask;'';cutoffinfo;1 0 2 3;(_0.3 (4}) parms);Frow


  NB. column audit
  cutoffinfo =. (2 ,~ #rvt) $ 0.
NB.   parms is #cols(flagged),maxAx,Col0Threshold,expandQk (testcase option),MinGradient/MinGradImp,x
  parms =.            4         0.      1e_25        0                                  0           0    NB. We expand each row to its own comparison block
  assert. 'index error' -: (0 0 0 0 0; _2 ]\ 10$0) ('' run128_9) etx Ax;Am;Av;M;rvt;bndrowmask;'';cutoffinfo;0 1 2 5;(parms);Frow

  NB. Large random test
  nqcols=.20
  for_nrow. (2000 + 1 ?@$ 512) , (128 + 10 ?@$ 512) do.  NB. varying column length of Qk.  Few cols of Qk, many of A0.  One long col to multithread small # columns
    Qkt =. 15!:18 ] 2 {. ,: (>.&.(%&4) nrow) {."1 >: (nqcols,nrow) ?@$ 0  NB. use all positive values to avoid massive cancellation
    Ax =. ,"0 (,.~   [: |.!.0 +/\) 1 >. (256 + ? 255) ?@$ nqcols   NB. array of start,length.  Big enough for multiple reservations
    Am =. ((<1 0)&{"2 Ax) ;@:(?&.>) nqcols  NB. column #s
    Av =. >: ((<1 0)&{"2 Ax) ;@:(?@$&.>) 0  NB. weights, all positive
NB.   parms is #cols(flagged),maxAx,                x,expandQk (testcase option),MinGradient/MinGradImp,x
    parms =.     nrow,      (>./((<1 0)&{"2 Ax)), 0.,0.                               0.             0.
    for_nqacols. (0 ,. 1 + i. 32) , |: 0 1 >. 20 ?@$"0 (nqcols , #Ax) do.  NB. pairs of #Q cols, #Acols.  Acols may not be empty.  Test small # columns
      qacols =. nqacols ?&.> (nqcols , #Ax)  NB. Qk col#s ; A0 col #s
      Yt =. '014' {~ 0.06 0.1 I. nrow ?@$ 0  NB. Random types, mostly normal
      nttcols =. (, +&nqcols)&>/ qacols   NB. column indexes in NTT
      frow =. - 0.2 + (nqcols + #Ax) ?@$ 0  NB. random negative frow
      Vt =. '014' {~ 0.06 0.1 I. (nqcols + #Ax) ?@$ 0  NB. Random types, mostly normal
      NB. Find the largest gradient.  This is what 128!:9 does in gradient mode (not including MinGrad)
      qkcols =. (<0;0{::qacols) { Qkt  NB. cols of Qk
      akcols =. ([: (];.0&Av +/@:* Qkt {~ [: < 0 ; ];.0&Am) {&Ax)"0 ] 1{::qacols  NB. cols of Ak, weighted by Av
      grads =. (|nttcols { frow) % %: ('01' e.~ nttcols { Vt) + 1. + +/"1 (1. + Yt e. '01') *"1 *: nrow {."1 qkcols,akcols
      maxgrad =. grads {~ col =. (i. >./) grads  NB. expected col & gradient
      col =. col { nttcols  NB. convert index in list to value
      NB. end of 128!:9 emulation
      cuti =. (2 ,~ #Vt) $ 0.   NB. init no info on column
      rvt =. {&a.@:#.@:(|."1) ($~  8 ,~ >.@(8 %~ #)) , ,./ Vt e."1 '19',:'01'  NB. create values in littleendian order, 2 bits/col (enforced is LSB)
      bndrowmask =. {&a.@:#.@:(|."1) _8 ]\ , (|."1) 0 2 1 |: 16 4&$"1 ($~  64 ,~ >.@(64 %~ #)) 0 ,~ Yt e. '01'  NB. 0 is for Fk
      'rc9 col9 ncol ndot grad9' =. 128!:9 Ax;Am;Av;Qkt;rvt;bndrowmask;'';cuti;nttcols;parms;frow
      assert. (col,maxgrad) -: col9,grad9
    end.
  end.


  NB. end of tests, add a thread
  0 T. ''
end.

delth''  NB. make sure we end with an empty system

1
}} 1

NB. **** 128!:13  (deprecated)

NB. 128!:13 ck[;bk;colthresh,bkthresh] ------------------------
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
}} 1


NB. **** 128!:12  (deprecated)

NB. (prx;pcx;pivotcolnon0;newrownon0;absfuzz) 128!:12 Qk ------------------------

epdefuzzsub =: {{ ([: (*"_1 _   u <!.0 |@{.) epsub) }}
NB. normal precision
f =: {{
 while. 1 T. '' do. 55 T. '' end.
 while. 4 > 1. T. '' do.
  siz =. y  NB. size of Qk
  'r c' =. 2 ?@$ >:siz  NB. size of modified area
  prx =. 00 + r ? siz [ pcx =. 00 + c ? siz  NB. indexes of mods
  pivotcolnon0 =.r ?@$ 0 [ newrownon0 =. c ?@$ 0
  absfuzz =. 1e_14 * ? 0  NB. tolerance
  Qk =. (>.&.(%&4) siz) {."1 (siz,siz) ?@$ 0
  expQk=. (((<prx;pcx) { Qk) ((*  absfuzz <!.0 |)@:-) pivotcolnon0 */ newrownon0) (<prx;pcx)} preQk =. memu Qk
  Qk =. (prx;pcx;pivotcolnon0;newrownon0;absfuzz) 128!:12 Qk
  if. -. r =. 1e_11 > >./ , | Qk - expQk do. 13!:8]4 [ 'prx__ pcx__ pivotcolnon0__ newrownon0__ absfuzz__ expQk__ preQk__ Qk__' =: prx;pcx;pivotcolnon0;newrownon0;absfuzz;expQk;preQk;Qk end.
  0 T. 0  NB. allocate a worker thread
 end.
 while. 1 T. '' do. 55 T. '' end.
 r 
}}"0
f i. 65

NB. quad precision Qk but dp row and col - no longer used
f =: {{
 epmul =. (|:~ (_1 |. i.@#@$)) @: (((0 0 1 1{[) +/@:*"1!.1 (0 1 0 1{]))"1&(0&|:))
 epadd =. (|:~ (_1 |. i.@#@$)) @: ((1.0"0 +/@:*"1!.1 ])@,"1&(0&|:))
 epsub =. (epadd -)
NB. obsolete  epdefuzzsub =. ((]: * >.)&:|&{. ((<!.0 |@{.) *"_ _1 ]) epsub)
 epcanon =. (epadd   0 $~ $)
 siz =. y  NB. size of Qk
 while. 1 T. '' do. 55 T. '' end.
 while. 4 > 1. T. '' do.
  'r c' =. 2 ?@$ >:siz  NB. size of modified area
  prx =. 00 + r ? siz [ pcx =. 00 + c ? siz  NB. indexes of mods
  pivotcolnon0 =. (] {~ r ? #) (, -) 10 ^ (+   3 * *) 6. * _0.5 + r ?@$ 0  NB. random values 1e3 to 1e6 and 1e_3 to 1e_6, both signs
  pivotcolnon0 =. epcanon (,:   ] * (2^_53) * _0.5 + 0 ?@$~ $) pivotcolnon0  NB. append extended part
  pivotcolnon0 =. 0. + pivotcolnon0  NB. force to float
  newrownon0 =. (] {~ c ? #) (, -) 10 ^ (+   3 * *) 6. * _0.5 + c ?@$ 0  NB. random values 1e3 to 1e6 and 1e_3 to 1e_6, both signs
  newrownon0 =. epcanon (,:   ] * (2^_53) * _0.5 + 0 ?@$~ $) newrownon0  NB. append extended part
  newrownon0 =. 0. + newrownon0  NB. force to float
  absfuzz =. 1e_25 * ? 0  NB. tolerance
  Qk =. (>.&.(%&4) siz) {."1 (*  0.25 < 0 ?@$~ $) 1e6 * (siz,siz) ?@$ 0.
  Qk =. epcanon (,:   ] * (2^_53) * _0.5 + 0 ?@$~ $) Qk  NB. append extended part
  Qk1 =. memu Qk
  expQk=. (((<a:;prx;pcx) { Qk) (absfuzz epdefuzzsub) (c #"0 pivotcolnon0) epmul (r&#@,:"1 newrownon0)) (<a:;prx;pcx)} preQk =. memu Qk
  Qk =. (prx;pcx;({.pivotcolnon0);({.newrownon0);absfuzz) 128!:12 Qk
  if. -. 1e_30 > >./ re =. , | (+/  expQk epsub Qk) % (| +/ Qk) >. (| +/ preQk) >. (| +/ expQk) do. 13!:8]4 [ 'r__ c__ re__ prx__ pcx__ pivotcolnon0__ newrownon0__ absfuzz__ expQk__ preQk__ Qk__' =: r;c;re;prx;pcx;pivotcolnon0;newrownon0;absfuzz;expQk;preQk;Qk end.
  
  0 T. 0  NB. allocate a worker thread
 end.
 while. 1 T. '' do. 55 T. '' end.
 1
}}"0


NB. quad precision everything, also testing batch mode
f =: {{
 epmul =. (|:~ (_1 |. i.@#@$)) @: (((0 0 1 1{[) +/@:*"1!.1 (0 1 0 1{]))"1&(0&|:))
 epadd =. (|:~ (_1 |. i.@#@$)) @: ((1.0"0 +/@:*"1!.1 ])@,"1&(0&|:))
 epsub =. (epadd -)
NB. obsolete  epdefuzzsub =. ((]: * >.)&:|&{. ((<!.0 |@{.) *"_ _1 ]) epsub)
 epcanon =. (epadd   0 $~ $)
 siz =. y  NB. size of Qk
 while. 1 T. '' do. 55 T. '' end.
 while. 4 > 1. T. '' do.
  'r c' =. 2 ?@$ >:siz  NB. size of modified area
  prx =. 00 + r ? siz [ pcx =. 00 + c ? siz  NB. indexes of mods
  pivotcolnon0 =. (] {~ r ? #) (, -) 10 ^ (+   3 * *) 6. * _0.5 + r ?@$ 0  NB. random values 1e3 to 1e6 and 1e_3 to 1e_6, both signs
  pivotcolnon0 =. epcanon (,:   ] * (2^_53) * _0.5 + 0 ?@$~ $) pivotcolnon0  NB. append extended part - all 0
  pivotcolnon0 =. 0. + pivotcolnon0  NB. force to float
  newrownon0 =. (] {~ c ? #) (, -) 10 ^ (+   3 * *) 6. * _0.5 + c ?@$ 0  NB. random values 1e3 to 1e6 and 1e_3 to 1e_6, both signs
  newrownon0 =. epcanon (,:   ] * (2^_53) * _0.5 + 0 ?@$~ $) newrownon0  NB. append extended part
  newrownon0 =. 0. + newrownon0  NB. force to float
  absfuzz =. 1e_30 >. 1e_25 * ? 0  NB. tolerance
  Qk =. (>.&.(%&4) siz) {."1 (*  0.25 < 0 ?@$~ $) 1e6 * (siz,siz) ?@$ 0.
  Qk =. epcanon (,:   ] * (2^_53) * _0.5 + 0 ?@$~ $) Qk  NB. append extended part
  expQk=. (((<a:;prx;pcx) { Qk) (absfuzz epdefuzzsub) (c #"0 pivotcolnon0) epmul (r&#@,:"1 newrownon0)) (<a:;prx;pcx)} preQk =. memu Qk
  Qk =. (prx;pcx;pivotcolnon0;newrownon0;absfuzz) 128!:12 Qk
  Qk1 =. 15!:18 preQk
  if. -. 1e_30 > >./ re =. , | (+/  expQk epsub Qk) % (| +/ Qk) >. (| +/ preQk) >. (| +/ expQk) do. 13!:8]4 [ 'r__ c__ re__ prx__ pcx__ pivotcolnon0__ newrownon0__ absfuzz__ expQk__ preQk__ Qk__' =: r;c;re;prx;pcx;pivotcolnon0;newrownon0;absfuzz;expQk;preQk;Qk end.
  qktrow =. newrownon0 (<a:;pcx)} (0 2 { $Qk) (15!:18) 0.  NB. scatter the non0s throughout a simulated row (must be aligned)
  pcxbatch =. ~. _4 (17 b.) /:~ pcx  NB. unique batch indexes
  Qk1 =. (prx;pcxbatch;pivotcolnon0;qktrow;-absfuzz) 128!:12 Qk1
  if. -. Qk1 -: Qk do. 13!:8]4 [ 'r__ c__ re__ prx__ pcx__ pivotcolnon0__ newrownon0__ absfuzz__ expQk__ preQk__ Qk__ Qk1__ qktrow__ pcxbatch__' =: r;c;re;prx;pcx;pivotcolnon0;newrownon0;absfuzz;expQk;preQk;Qk;Qk1;qktrow;pcxbatch end.
  0 T. 0  NB. allocate a worker thread
 end.
 while. 1 T. '' do. 55 T. '' end.
 1
}}"0


f i. 65

NB. 31-digit relative max
{{
 epmul =. (|:~ (_1 |. i.@#@$)) @: (((0 0 1 1{[) +/@:*"1!.1 (0 1 0 1{]))"1&(0&|:))
 epadd =. (|:~ (_1 |. i.@#@$)) @: ((1.0"0 +/@:*"1!.1 ])@,"1&(0&|:))
 epsub =. (epadd -)
origQk =. memu Qk =. (4 4 $ 1. 10. 1e16 1e20) ,: (|: 4 4 $ 1e_20 1e_18 1e_16 1e_10)
rows =. 1. 1 1 1 [ cols =. 1. 10. 1e16 1e20
expQkhi =. (0:^:(<  1e_25 + 1e_31 * |))"0 {. Qk epsub 0. ,:~ 4 $ ,: cols
Qk =. ((i. 4);(i. 4);(rows,:0.);(cols,:0.);1e_25) 128!:12 Qk
NB. obsolete smoutput 0j_20 ": {. origQk
NB. obsolete smoutput 0j_20 ": {. origQk epsub  0. ,:~ 4 $ ,: cols
NB. obsolete smoutput 0j_20 ": {. Qk
NB. obsolete smoutput 0j_20 ": expQkhi
NB. obsolete smoutput 0j_20 ": ({.Qk)-expQkhi
(1 1 1 0 *./ 0 0 1 1) -: 0.={. Qk
}} 0


NB. avoid repeated indexes
f =: {{
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
 if. -. r =. 1e_11 > >./ , | ck - expck do. 13!:8]4 [ ' prcr__  newrownon0__  expck__ preck__ ck__' =: prcr;newrownon0;expck;preck;ck end.
 1
}}
f ''


NB. (0;pcx;pivotcolnon0;newrownon0;absfuzz) 128!:12 bk ------------------------
NB. normal precision
f =: {{
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
}}"0
f i. 65

NB. quad precision
f =: {{
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
}}"0

f i. 65


NB. **** 128!:22

NB. (prx;pcx;pivotcolnon0;newrownon0;absfuzz) 128!:22 Qkt ------------------------

epdefuzzsub =: {{ ([: (*"_1 _   u <!.0 |@{.) epsub) }}
e128x22 =: [: ((] ,: 8 c. -) 8&c.)   ((2&{.) , (11 c. +/@:(11&c.))&.>@(2 3&{) , {:)@[ 128!:22 (11) c. (15!:18)@:memu@:(+/@:(11&c.))@]

NB. quad precision everything, also testing batch mode
f =: {{
 epmul =. (|:~ (_1 |. i.@#@$)) @: (((0 0 1 1{[) +/@:*"1!.1 (0 1 0 1{]))"1&(0&|:))
 epadd =. (|:~ (_1 |. i.@#@$)) @: ((1.0"0 +/@:*"1!.1 ])@,"1&(0&|:))
 epsub =. (epadd -)
NB. obsolete  epdefuzzsub =. ((]: * >.)&:|&{. ((<!.0 |@{.) *"_ _1 ]) epsub)
 epcanon =. (epadd   0 $~ $)
 siz =. y  NB. size of Qk
 while. 1 T. '' do. 55 T. '' end.
 while. 4 > 1. T. '' do.
  'r c' =. 2 ?@$ >:siz  NB. size of modified area
  prx =. 00 + r ? siz [ pcx =. 00 + c ? siz  NB. indexes of mods
  pivotcolnon0 =. (] {~ r ? #) (, -) 10 ^ (+   3 * *) 6. * _0.5 + r ?@$ 0  NB. random values 1e3 to 1e6 and 1e_3 to 1e_6, both signs
  pivotcolnon0 =. epcanon (,:   ] * (2^_53) * _0.5 + 0 ?@$~ $) pivotcolnon0  NB. append extended part - all 0
  pivotcolnon0 =. 0. + pivotcolnon0  NB. force to float
  newrownon0 =. (] {~ c ? #) (, -) 10 ^ (+   3 * *) 6. * _0.5 + c ?@$ 0  NB. random values 1e3 to 1e6 and 1e_3 to 1e_6, both signs
  newrownon0 =. epcanon (,:   ] * (2^_53) * _0.5 + 0 ?@$~ $) newrownon0  NB. append extended part
  newrownon0 =. 0. + newrownon0  NB. force to float
  absfuzz =. 1e_30 >. 1e_25 * ? 0  NB. tolerance
  Qk =. (>.&.(%&4) >: siz) {."1 (*  0.25 < 0 ?@$~ $) 1e6 * (siz,siz) ?@$ 0.
  Qk =. epcanon (,:   ] * (2^_53) * _0.5 + 0 ?@$~ $) Qk  NB. append extended part
  expQk=. (((<a:;prx;pcx) { Qk) (absfuzz epdefuzzsub) (c #"0 pivotcolnon0) epmul (r&#@,:"1 newrownon0)) (<a:;prx;pcx)} preQk =. memu Qk
  Qk =. (prx;pcx;pivotcolnon0;newrownon0;absfuzz) e128x22 Qk
  Qk1 =. 15!:18 preQk
  if. -. 1e_30 > >./ re =. , | (+/  expQk epsub Qk) % (| +/ Qk) >. (| +/ preQk) >. (| +/ expQk) do. 13!:8]4 [ 'r__ c__ re__ prx__ pcx__ pivotcolnon0__ newrownon0__ absfuzz__ expQk__ preQk__ Qk__' =: r;c;re;prx;pcx;pivotcolnon0;newrownon0;absfuzz;expQk;preQk;Qk end.
  qktrow =. newrownon0 (<a:;pcx)} (0 2 { $Qk) (15!:18) 0.  NB. scatter the non0s throughout a simulated row (must be aligned)
  pcxbatch =. ~. _4 (17 b.) /:~ pcx  NB. unique batch indexes
  Qk1 =. (prx;pcxbatch;pivotcolnon0;qktrow;-absfuzz) e128x22 Qk1
  if. -. Qk1 -: Qk do. 13!:8]4 [ 'r__ c__ re__ prx__ pcx__ pivotcolnon0__ newrownon0__ absfuzz__ expQk__ preQk__ Qk__ Qk1__ qktrow__ pcxbatch__' =: r;c;re;prx;pcx;pivotcolnon0;newrownon0;absfuzz;expQk;preQk;Qk;Qk1;qktrow;pcxbatch end.
  0 T. 0  NB. allocate a worker thread
 end.
 while. 1 T. '' do. 55 T. '' end.
 1
}}"0


f >: i. 64

NB. 31-digit relative max
{{
 epmul =. (|:~ (_1 |. i.@#@$)) @: (((0 0 1 1{[) +/@:*"1!.1 (0 1 0 1{]))"1&(0&|:))
 epadd =. (|:~ (_1 |. i.@#@$)) @: ((1.0"0 +/@:*"1!.1 ])@,"1&(0&|:))
 epsub =. (epadd -)
Qk =. (4 4 $ 1. 10. 1e16 1e20) ,: (|: 4 4 $ 1e_20 1e_18 1e_16 1e_10)
rows =. 1. 1 1 1 [ cols =. 1. 10. 1e16 1e20
expQkhi =. (0:^:(<  1e_25 + 1e_31 * |))"0 {. Qk epsub 0. ,:~ 4 $ ,: cols
Qk =. }:"1 ((i. 4);(i. 4);(rows,:0.);(cols,:0.);1e_25) e128x22 5 {."1 Qk
(1 1 1 0 *./ 0 0 1 1) -: 0.={. Qk
}} 0


NB. avoid repeated indexes
f =: {{
 ck =. 0. + i. 10
 prcr =. ;@:((<@:({.~  1 i.~ 0 1 2 3 ~: i.~))"1) (#: i.@:(*/)) 4 4 4 4
 prcrnub =. ~. prcr
 prcl =. (#prcr) # 01
 newrownon0 =. >: (#prcr) ?@$ 0
 mplr =. ($prcr) ?@$ 0
 ckchg =. prcr +//. (prcl # newrownon0) * mplr
 upd=. ckchg -~ prcrnub { ck
 expck=. upd prcrnub} preck =. memu ck
 ck =. {. (00;prcr;(1. 0);(0,:~prcl #"01 newrownon0);(mplr)) e128x22 ck,:0
 if. -. r =. 1e_11 > >./ , | ck - expck do. 13!:8]4 [ ' prcr__  newrownon0__  expck__ preck__ ck__' =: prcr;newrownon0;expck;preck;ck end.
 1
}}
f ''

NB. **** 128!:22

NB. (0;pcx;pivotcolnon0;newrownon0;absfuzz) 128!:22 bk ------------------------
NB. quad precision
f =: {{
 epmul =. (|:~ (_1 |. i.@#@$)) @: (((0 0 1 1{[) +/@:*"1!.1 (0 1 0 1{]))"1&(0&|:))
 epadd =. (|:~ (_1 |. i.@#@$)) @: ((1.0"0 +/@:*"1!.1 ])@,"1&(0&|:))
 epsub =. (epadd -)
 epcanon =. (epadd   0 $~ $)
 siz =. y  NB. size of Qk
 r =. 1 [ c =. ? >:siz  NB. size of modified area
 pcx =. 00 + c ? siz  NB. indexes of mods
 pivotcolnon0 =. r ?@$ 0 [ newrownon0 =. c ?@$ 0
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
 bk =. (00;pcx;({."1 pivotcolnon0);newrownon0;absfuzz) e128x22 bk
 if. -. 1e_30 > >./ re =. , | (+/  expbk epsub bk) % (| +/ bk) >. (| +/ prebk) >. (| +/ expbk) do. 13!:8]4 [ 'r__ c__ re__ pcx__ pivotcolnon0__ newrownon0__ absfuzz__ expbk__ prebk__ bk__' =: r;c;re;pcx;pivotcolnon0;newrownon0;absfuzz;expbk;prebk;bk end.
 1
}}"0

f i. 65

epilog''

