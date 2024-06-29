NB. geev   eigenvalues and eigenvectors of a square matrix

coclass 'jlapack'

NB. =========================================================
NB.*geev v eigenvalues and eigenvectors of a square matrix
NB.
NB. syntax:
NB.   'L V R'=. [RMASK] geev A
NB. where
NB.   A     - N-by-N matrix
NB.   RMASK - values to return bit mask: each bit corresponds
NB.           to value should be returned: 2b100 means
NB.           "only L", 2b001 means "only R", 2b111 (default)
NB.           means "all", 2b000 is prohibited
NB.   L     - N-by-N matrix of left eigenvectors
NB.   V     - N-vector of eigenvalues
NB.   R     - N-by-N matrix of right eigenvectors
NB.   N     >= 0
NB.
NB. if:
NB.   'L V R'=. geev A
NB. then
NB.   (A mp R)       is  (V *"1 R)   is  (R mp (diag V))
NB.   ((+|:L) mp A)  is  (V * +|:L)  is  ((diag V) mp (+|:L))

geev=: (2b111&$: : (4 : 0)) " 0 2

y=. z2d y
ic=. iscomplex y
zero=. ic {:: dzero ; zzero
routine=. ic { 'dgeev' ,: 'zgeev'
iox=. ic { 7 14 ,: 5 6

if. (-. 0 1 -: x I. 1 8) +. ((0 ~: #@$) +. (0 -: ]) +. (0 ~: L.)) x do.
  error routine;'RMASK should be an integer in range [1,7]'
end.

vsquare y

sa=. |.$y
'isvl isvr'=. isv=. 0 ~: 2b100 2b001 (17 b.) x
'svli svri'=. isv { 0 0 ,: sa

'jobvl jobvr'=. isv { 'NV'
n=. #y
a=. zero + |:y
lda=. ldvl=. ldvr=. 1 >. n
if. ic do.
  w=. n$zero
else.
  wr=. wi=. n$zero
end.
vl=. svli$zero
vr=. svri$zero
lwork=. 1 >. 34*n
work=. lwork$zero
if. ic do.
  rwork=. (+: n) $ dzero
end.
info=. izero

arg=. iox xtoken 'jobvl;jobvr;n;a;lda;wr;wi;w;vl;ldvl;vr;ldvr;work;lwork;rwork;info'

(cutarg arg)=. routine call , each ".arg

if. 0~:info=. fixint info do.
  error routine;'info result: ',":info return.
end.

vl=. |:svli$vl
vr=. |:svri$vr

if. -. ic do.
  w=. wr
  if. 1 e. wi ~: 0 do.
    if. 2b010 (17 b.) x do.
      w=. w j. wi
    end.
    if. isvl +. isvr do.
      cx=. I. wi ~: 0
      if. isvl do.
        vl=. cx cxpair vl
      end.
      if. isvr do.
        vr=. cx cxpair vr
      end.
    end.
  end.
end.

({. @: > ^: (1=#)) (I. _3 {. #: x) { vl;w;vr
)

NB. =========================================================
NB.*tgeev v test geev

tgeev=: 3 : 0
match=. matchclean;;
smoutput 'L V R'=. geev y
smoutput a=. (clean y mp R) match (clean V *"1 R)
smoutput b=. (clean (+|:L) mp y) match (clean V * +|:L)
(0 pick a) *. 0 pick b
)

NB. =========================================================
NB. test matrices:

testgeev=: 3 : 0
m0=. 0 0$0
m1=. ?.6 6$10
m2=. 0 0$zzero
m3=. j./ ?.2 6 6$10
tgeev &> m0;m1;m2;m3
)
