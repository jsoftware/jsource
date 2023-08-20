prolog './g520.ijs'


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
  NB.   parms is #cols in Qk(flagged),maxAx,Col0Threshold,Store0Thresh,x,ColDangerPivot,ColOkPivot,Bk0Threshold,BkOvershoot,MinSPR,PriRow
  origparms =: parms  NB. debug
  nflagged =. 0{parms  NB. # columns of Qk, with fk flag
  nrows =. (1 { $ Qk)   NB. calculate all rows of result
  ncol =. (|nflagged) - (nflagged<0)  NB. number of columns in basis
  if. 0~:nthr do.   NB. multiple threads: expand Qk with 0 rows to make size threadable, scramble rows in the data.  Keep bndrowmask/bk/bkbeta corresponding to same values
    NB. Create the vector of sources for each new row of Qk etc.  We will first add a harmless row and then draw, where _1 is harmless and _2 is the Fk row if present
    nrows =. #drawvec =. ,&_2^:(nflagged<0) ({~ ?~@#) (1200 + ? 50) {.!._1 i. nrows - (nflagged<0)  NB. modify #rows to match the unpadded block
    NB. modify the inputs
    Qk =. drawvec {"1 _1 Qk ,"_1 (0.)
    bndrowmask =. drawvec { bndrowmask , '4'
    drawvec =. ({.!._1~ >.&.(%&4)@#) drawvec NB. pad bk/bkbeta/z to batch bdy
    bk =. drawvec {"1 _1 bk ,"_1 (0.)
    bkbeta =. drawvec { bkbeta , 0.
NB. obsolete     parms =. (-^:(nflagged < 0) nrows) 0} parms  NB. retore Fk flag to new length
    y =. (Qk;bk;parms;bkbeta) 3 6 9 10} y
  end.
  if. colx<ncol do. savref   =: ref =. colx {"1 Qk  NB. basic column, just fetch it
  else.
    qkcols =. (colx-ncol) { Ax  NB. start,length of column indexes/weights
    savref   =: ref =. |: (,~ qkcols ];.0 Av) +/@:*"1!.1 ,.~/ (qkcols ];.0 Am){"1 Qk  NB. fetch cols and combine them
  end.
  if. '19' e.~ colx { rvt do. ref =. - ref end.  NB. change sign of column if enforcing
  ref =. (3{parms) (((< |) {.) *"1 ]) ref  NB. Clear values below the Store0 threshold
  NB. convert bitmasks to internal form
  rvt =. {&a.@:#.@:(|."1) ($~  8 ,~ >.@(8 %~ #)) , ,./ rvt e."1 '19',:'01'  NB. create values in littleendian order, 2 bits/col (enforced is LSB)
  bndrowmask =. {&a.@:#.@:(|."1) _8 ]\ , (|."1) 0 2 1 |: 16 4&$"1 ($~  64 ,~ >.@(64 %~ #)) 0 ,~ bndrowmask e. '01'  NB. 0 is for Fk
  y =. (rvt;bndrowmask) 4 5} y
  y =. (15!:18&.> 3 6 10 { y) 3 6 10} y  NB. cache-align the blocks that need it
  z =. (2, #bkbeta) 15!:18 (0.)  NB. result area, big enough to hold final batch including fk if present
  savres   =: 128!:9 savy =: (<z) 7} y
  if. 4 ~: {. x do.  NB. if the column was aborted, don't check the values
    assert. 0 = +./ 128!:5 , z
    assert. 1e_25 > >./ | +/ ref epsub&((nrows-nflagged<0)&{."1) z  NB. require match on valid values
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
  nrows =. 1 { $ Qk  NB. number of valid rows in Qk
  if. 0~:nthr do.   NB. multiple threads: expand Qk with 0 rows to make size threadable, scramble rows in the data.  Keep bndrowmask/bk/bkbeta corresponding to same values
    NB. Create the vector of sources for each new row of Qk etc.  We will first add a harmless row and then draw, where _1 is harmless
    nrows =. #drawvec =. ({~ ?~@#) (1200 + ? 50) {.!._1 i. nrows  NB. update count of rows
  elseif. 3 { parms do.   NB. 
   NB. spread rows by NPAR to get accurate value for cutoff.  We check cutoff one time in 64 atoms (16 batches) (subject to change)
   nrows =. #drawvec =. 1j63 #!._1 i. nrows
  else.
    drawvec =. i. nrows  NB. keep input unchanged
  end.
  Qk =. drawvec {"1 _1 Qk ,"_1 (0.)
  bndrowmask =. drawvec { bndrowmask , '4'
  parms =. nrows 0} parms  NB. retore Fk flag to new length
  y =. (Qk;parms) 3 9} y
  NB. convert bitmasks to internal form
  rvt =. {&a.@:#.@:(|."1) ($~  8 ,~ >.@(8 %~ #)) , ,./ rvt e."1 '19',:'01'  NB. create values in littleendian order, 2 bits/col (enforced is LSB)
  bndrowmask =. {&a.@:#.@:(|."1) _8 ]\ , (|."1) 0 2 1 |: 16 4&$"1 ($~  64 ,~ >.@(64 %~ #)) 0 ,~ bndrowmask e. '01'  NB. 0 is for Fk
  y =. (rvt;bndrowmask) 4 5} y
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
  for_l. 2 + i. (*1 T. ''){51 5 do.  NB. l is #rows in Qk including Fk if any
    'nQcol nAcol' =. 10 15  NB. number of cols in Qk, A0
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
  assert. 3 _1 1 8 _ ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';00;(1e_3 (2)} parms);bkbeta;beta
  assert. 1 0 1 8 5000 ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';00;(1e_2 (6)} parms);bkbeta;beta
  assert. 3 0 1 8 5000 ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';00;(1e_3 1e_2 (5 6)} parms);bkbeta;beta
  assert. 4 _1 0 0 0 ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';00;(0.75 0. (7 9)} parms);bkbeta;beta  NB. row 0 now below threshold but 0 cuts off, unbounded
  assert. 2 0 1 8 _10000 ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';00;(0.75 (7)} parms);bkbeta;beta  NB. row 0 now below threshold, nonimp
  M =. 2 {. ,: ,.              8 $ 1000.
  bk =. 2 {. ,: ($bkbeta) $    8 $ 1.
  assert. 0 4 1 8 0.001 ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';00;(parms);bkbeta;beta
NB. put nothing here! savy is carried over to the next line
  NB. Verify random pickup of 0 bk values.  This kludge relies on a side effect left by run128_9
  if. 0 = 1 T. '' do.  NB. in multithreaded we don't know what run128_9 does; and single-threaded test is good enough
    tests =. (#~   [: +./"1 </"2) 0.2 0.5 >"(1 2) (100 2,{:$M) ?@$ 0  NB. first row zaps M, second zaps bk.  Remove tests that leave no places where M is not set and bk is
    for_mb. tests do.
      'mx bx' =. <@I."1 mb  NB. indexes to zap
      savy =: (<(15!:18) 1e_12 (<0;mx)} M) 3} savy  NB. make some values of M small
      savy =: (<(15!:18) 0. (<0;bx)} bk) 6} savy  NB. make some values of bk 0
      pickup =. ({:$M)$00 [ z =. (2, #bkbeta) 15!:18 (0.)
      for. i. 200 do.
        assert. 2 = 0 { res =. 128!:9 (<z) 7} savy
        pickup =. (1{res) (>:@{`[`])} pickup
      end.
      assert. (</ mb) = * pickup  NB. verify that the filled places are exactly the ones that zapped bk but not m
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
  assert. 3 0 1 8 1e9 ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';00;(2e_12 (5)} parms);bkbeta;beta  NB. below dangerous ignored
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
  assert. 1 4 1 8 _1e14 ((,:1) run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';00;(parms);bkbeta;beta  NB. dangerous, then nonimproving with no safe pivot
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
  assert. 1 0 1 8 _1e12 ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;bk;'';00;(parms);bkbeta;beta  NB. dangerous pivot
  assert. 2 1 1 8 _1e5 ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;(_1e_6 (<0 1)} bk);'';00;(parms);bkbeta;beta  NB. nondangerous pivot
  assert. 1 0 1 8 _1e12 ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;(1e_6 (<0 1)} bk);'';00;(parms);bkbeta;beta  NB. dangerous pivot
  assert. 2 1 1 8 _1e5 ('' run128_9) Ax;Am;Av;M;rvt;('0' 2} bndrowmask);(1e_14 (<0 1)} bk);'';00;(parms);bkbeta;beta  NB. nondangerous pivot
  assert. 2 2 1 8 _1e4 ('' run128_9) Ax;Am;Av;M;rvt;('0' 2} bndrowmask);(1e_14 (<0 1)} bk);'';00;(parms);((1+1e_13) 2} bkbeta);beta  NB. nondangerous pivot

  NB. gradient mode
NB.  y is Ax;Am;Av;(M, shape 2,m,n);RVT;bndrowmask;(sched);cutoffinfo;ndx;parms;Frow  where ndx is a list
  Ax =. 0 2 1 $ 00 [ Am =. 0$00 [ Av =. 0$0.
  M =. ,:   1.    1    _1    1   NB. Input by rows, i. e. NOT transposed
  M =. M ,  _1.    1   _1   1
  M =. M ,  _1.    1   1    1
  M =. M ,  1.    1    1    _1
  M =. M ,  0.    0    5    0
  M =. M ,  3 4$0.
  M =. 2 {. ,: M
  Frow =.  _1.     _2   _3   _4
  bndrowmask =. (1 { $ M) $     '4'
  rvt =. '4444'
NB.   parms is #cols(flagged),maxAx,Col0Threshold,expandQk (testcase option),MinGradient/MinGradImp,x
  parms =.           4          0.      1e_25        1                                  0           0    NB. We expand each row to its own comparison block
  cutoffinfo =. (2 ,~ #rvt) $ 0.
  assert. (0 3 4 1856 1.788854381999831; _2 ]\ _512 5 _512 5 _320 30 _512 5 ) ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;'';cutoffinfo;0 1 2 3;(parms);Frow
  Frow =.  _4.     _3   _2   _1
  cutoffinfo =. (2 ,~ #rvt) $ 0.
  assert. (0 0 4 704 1.788854381999831; _2 ]\ _512 5 _128 3 64 2 0 1) ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;'';cutoffinfo;0 1 2 3;(parms);Frow
  if. 0 = 1 T. '' do.  NB. can't chain cutoffinfo multithreaded, because the problems change size
    NB. roll cutoffinfo into the next test, to verify it is picked up
    assert. (0 0 4 1152 1.788854381999831; _2 ]\ _512 5 _512 5 _320 30 _512 5) ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;'';cutoffinfo;3 2 1 0;(parms);Frow  NB. reversed col order
  end.
  NB. bound column adds 1 to total
  cutoffinfo =. (2 ,~ #rvt) $ 0.
  assert. (0 0 4 1856 1.788854381999831; _2 ]\ _512 5 _512 6 _320 30 _512 5) ('' run128_9) Ax;Am;Av;M;('0' 1} rvt);bndrowmask;'';cutoffinfo;3 2 1 0;(parms);Frow  NB. reversed col order
  NB. bound row doubles the contribution of the row
  Frow =.  _1.     _2   _3   _4
  cutoffinfo =. (2 ,~ #rvt) $ 0.
  assert. (0 3 4 1856 1.788854381999831; _2 ]\ _512 5 _512 5 _320 55 _512 5) ('' run128_9) Ax;Am;Av;M;rvt;('0' 4} bndrowmask);'';cutoffinfo;0 1 2 3;(parms);Frow


  NB. unbounded problem
  cutoffinfo =. (2 ,~ #rvt) $ 0.
  assert. (4 1 1 512 0; _2 ]\ _512 5 0 0 0 0 0 0) ('' run128_9) Ax;Am;Av;M;('1' 1} rvt);bndrowmask;'';cutoffinfo;0 1 2 3;(parms);Frow
  NB. unbounded problem cut off at start
  Frow =.  _1.     _2   _3   _4
  cutoffinfo =. (2 ,~ #rvt) $ 0.
  assert. (0 3 4 640 1.788854381999831; _2 ]\ 0 1 0 2 128 3 _512 5) ('' run128_9) Ax;Am;Av;M;('1' 1} rvt);bndrowmask;'';cutoffinfo;3 2 1 0;(parms);Frow
  if. 0 = 1 T. '' do.  NB. can't chain cutoffinfo multithreaded, because the problems change size
    NB. roll cutoffinfo into the next test, to verify it is picked up
    assert. (4 1 1 384 0; _2 ]\ 0 1 0 2 _512 30 _512 5) ('' run128_9) Ax;Am;Av;M;('1' 1} rvt);bndrowmask;'';cutoffinfo;2 1 0;(parms);Frow
  end.
  NB. unbounded problem cut off before detection
  Frow =.  _1.     _3   _3   _4
  cutoffinfo =. (2 ,~ #rvt) $ 0.
  assert. (0 3 4 704 1.788854381999831; _2 ]\ 0 1 64 3 128 3 _512 5) ('' run128_9) Ax;Am;Av;M;('1' 1} rvt);bndrowmask;'';cutoffinfo;3 2 1 0;(parms);Frow
  if. 0 = 1 T. '' do.  NB. can't chain cutoffinfo multithreaded, because the problems change size
    NB. roll cutoffinfo into the next test, to verify it is picked up
    assert. (4 1 1 320 0; _2 ]\ 0 1 64 3 _512 30 _512 5) ('' run128_9) Ax;Am;Av;M;('1' 1} rvt);bndrowmask;'';cutoffinfo;2 1 0;(parms);Frow
  end.

  NB. MinGradient
  Ax =. 0 2 1 $ 00 [ Am =. 0$00 [ Av =. 0$0.
  M =. ,:   1.    1    1    1   NB. Input by rows, i. e. NOT transposed
  M =. M ,  1.    1    1    1
  M =. M ,  1.    1    1    1
  M =. M ,  1.    1    1    1
  M =. M ,  0.    0    5    0
  M =. M ,  3 4$0.
  M =. 2 {. ,: M
  Frow =.  _4.     _3   _2   _1
  bndrowmask =. (1 { $ M) $     '4'
  rvt =. '4444'
NB.   parms is #cols(flagged),maxAx,Col0Threshold,expandQk (testcase option),MinGradient/MinGradImp,x
  parms =.            4         0.      1e_25        1                                  0           0    NB. We expand each row to its own comparison block
  cutoffinfo =. (2 ,~ #rvt) $ 0.
  assert. (0 0 4 704  1.788854381999831; _2 ]\ _512 5 _128 3 _64 2 0 1 ) ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;'';cutoffinfo;0 1 2 3;(parms);Frow
  cutoffinfo =. (2 ,~ #rvt) $ 0.
  assert. (0 3 4 1856  1.788854381999831; _2 ]\ _512 5 _512 5 _320 30 _512 5 ) ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;'';cutoffinfo;0 1 2 3;(0.4 (4}) parms);Frow
  cutoffinfo =. (2 ,~ #rvt) $ 0.
  assert. (0 1 4 1536  1.788854381999831; _2 ]\ _512 5 _512 5 _320 30 _192 4 ) ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;'';cutoffinfo;0 1 2 3;(0.51 (4}) parms);Frow

  NB. MinGradImp
  cutoffinfo =. (2 ,~ #rvt) $ 0.
  assert. (0 0 4 1088  1.788854381999831; _2 ]\ _512 5 _512 5 _64 2 0 1 ) ('' run128_9) Ax;Am;Av;M;rvt;bndrowmask;'';cutoffinfo;1 0 2 3;(parms);Frow
  cutoffinfo =. (2 ,~ #rvt) $ 0.
  assert. (0 1 4 704  1.341640786499873838; _2 ]\ _192 4 _512 5 0 1 0 1 ) ((0 1.341640786499873838,:0 1.788854381999831) run128_9) Ax;Am;Av;M;rvt;bndrowmask;'';cutoffinfo;1 0 2 3;(_0.4 (4}) parms);Frow
  cutoffinfo =. (2 ,~ #rvt) $ 0.
  assert. (0 1 4 832 1.341640786499873838; _2 ]\ _256 5 _512 5 _64 2 0 1 ) ((0 1.341640786499873838,:0 1.788854381999831) run128_9) Ax;Am;Av;M;rvt;bndrowmask;'';cutoffinfo;1 0 2 3;(_0.3 (4}) parms);Frow

NB. scaf need a way to test many threads in gradient mode

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
 while. 4 > 1. T. '' do.
  siz =. y  NB. size of Qk
  'r c' =. 2 ?@$ >:siz  NB. size of modified area
  prx =. 00 + r ? siz [ pcx =. 00 + c ? siz  NB. indexes of mods
  pivotcolnon0 =.r ?@$ 0 [ newrownon0 =. c ?@$ 0
  absfuzz =. 1e_14 * ? 0  NB. tolerance
  Qk =. (>.&.(%&4) siz) {."1 (siz,siz) ?@$ 0
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
  Qk =. (>.&.(%&4) siz) {."1 (*  0.25 < 0 ?@$~ $) 1e6 * (siz,siz) ?@$ 0.
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

(i.64) -: (<<<_1) { i.65
(i.64) -: (<<<64) { i.65
(i.128) -: (<<<_1) { i.129
(i.128) -: (<<<128) { i.129

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
(130 + i. 64) -: (<1;0;<<_1) { i. 2 2 65
4 -: a: { 4
(-: a:&{) i. 4 5
(-: a:&{) 'abc'
(-: a:&{) o. i. 5 6 7


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

NB. virtualing of complex forms
a =: i. 2 3 4 10000
4000 > 7!:2 '1 { a'
4000 > 7!:2 '0 1 { a'
4000 > 7!:2 '(<1) { a'
4000 > 7!:2 '(<0 1) { a'
4000 > 7!:2 '(<1;<a:) { a'
4000 > 7!:2 '(<0 1;<a:) { a'
4000 > 7!:2 '(<1;1 2) { a'
4000 > 7!:2 '(<1;2) { a'
4000 > 7!:2 '(<1;2;<<0 _1) { a'
4000 > 7!:2 '(<1;2;2;<<0 _1) { a'
4000 > 7!:2 '(<0 2) { a'

NB. inplacing
a =: 10000 ?@$ 1000
1000 > (7!:2 '(i. 1024) { a') -  (7!:2 '(i. 1024)')
1000 > (7!:2 '(60 60 ?@$ #a) { a') -  (7!:2 '(60 60 ?@$ #a)')
a =: 10000 ?@$ 0
1000 > (7!:2 '(i. 1024) { a') -  (7!:2 '(i. 1024)')
1000 <`>@.IF64 (7!:2 '(60 60 ?@$ #a) { a') -  (7!:2 '(60 60 ?@$ #a)')
1000 < (7!:2 '(60 6 ?@$ #a) {"1 _ a') -  (7!:2 '(60 6 ?@$ #a)')

NB. returning w in full as extreme virtualing
a =: 3 10000 ?@$ 0
5000 > 7!:2 'a =: 0 1 2 { a'


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



 
4!:55 ;:'a adot1 adot2 sdot0 arg b catalog copy count epdefuzzsub exp f fr from ftype i j origparms qpmulvecatom res run128_9 savx savy savref savres savspr'
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

