NB. throw./catcht. ------------------------------------------------------

f=: 3 : 0
 if. y do.
  throw.
 end.
 0
)

(<'throw.') e. {:"1 t=: 1 (5!:7) <'f'

f=: 3 : 0
 try.
  1
  2
  3
 catcht.
  4
 end.
  :
 try. 5 catcht. 6 end.
)

(;:'try. catcht.') e. {:"1 t=: 1 (5!:7) <'f'

(;:'try. catcht.') e. {:"1 t=: 2 (5!:7) <'f'

main=: 3 : 0
 try.
  sub y
 catcht.
  select. type_jthrow_
   case. 'aaaa' do. 'catcht aaaa'  
   case. 'bbb'  do. 'catcht bbb'   
   case. 'cc'   do. 'catcht cc' 
   case.        do. throw.   NB. handled by higher-level catcht. (if any)
  end.
  return.
 end.
)

sub=: 3 : 0
 if. y<0  do. type_jthrow_=: 'aaaa' throw. end.
 if. y<4  do. type_jthrow_=: 'bbb'  throw. end.
 if. y<8  do. type_jthrow_=: 'cc'   throw. end.
 if. y=99 do. type_jthrow_=: 'd'    throw. end.
 (":y),' not cut back'
)

'catcht aaaa'     -: main _1
'catcht bbb'      -: main 2
'catcht cc'       -: main 6
'10 not cut back' -: main 10

sub _1   NB. throw without catcht

main1=: 3 : 0   NB. throw with adverse
 try. 
  sub :: 0:  y
 catcht.
  select. type_jthrow_
   case. 'aaaa' do. 'catcht aaaa'  
   case. 'bbb'  do. 'catcht bbb'   
   case. 'cc'   do. 'catcht cc' 
   case.        do. throw.   NB. handled by higher-level catcht. (if any)
  end.
  return.
 end.
)

'catcht aaaa'     -: main1 _1
'catcht bbb'      -: main1 2
'catcht cc'       -: main1 6
'10 not cut back' -: main1 10

main2=: 3 : 0   NB. throw with try/catch
 try.
  *'a'
 catch.
  try.
   sub y
  catcht.
   select. type_jthrow_
    case. 'aaaa' do. 'catcht aaaa'  
    case. 'bbb'  do. 'catcht bbb'   
    case. 'cc'   do. 'catcht cc' 
    case.        do. throw.   NB. handled by higher-level catcht. (if any)
   end.
   return.
  end.
 end.
)

'catcht aaaa'     -: main2 _1
'catcht bbb'      -: main2 2
'catcht cc'       -: main2 6
'10 not cut back' -: main2 10

main3=: 3 : 0
 try. 
  main4 y
 catcht.
  select. type_jthrow_
   case. 'd'    do. 'catcht d'  
   case.        do. throw.   NB. handled by higher-level catcht. (if any)
  end.
  return.
 end.
)

main4=: 3 : 0   NB. catcht at multiple levels
 try. 
  sub y
 catcht.
  select. type_jthrow_
   case. 'aaaa' do. 'catcht aaaa'  
   case. 'bbb'  do. 'catcht bbb'   
   case. 'cc'   do. 'catcht cc' 
   case.        do. throw.   NB. handled by higher-level catcht. (if any)
  end.
 end.
)

'catcht aaaa'     -: main3 _1
'catcht bbb'      -: main3 2
'catcht cc'       -: main3 6
'catcht d'        -: main3 99
'10 not cut back' -: main3 10

main5=: 3 : 0   NB. catcht at multiple levels
 try. 
  if. y<0  do. type_jthrow_=: 'aaaa' throw. end.
  if. y<4  do. type_jthrow_=: 'bbb'  throw. end.
  if. y<8  do. type_jthrow_=: 'cc'   throw. end.
  if. y=99 do. type_jthrow_=: 'd'    throw. end.
  (":y),' not cut back'
 catcht.
  select. type_jthrow_
   case. 'aaaa' do. 'catcht aaaa'  
   case. 'bbb'  do. 'catcht bbb'   
   case. 'cc'   do. 'catcht cc' 
   case.        do. throw.   NB. handled by higher-level catcht. (if any)
  end.
 end.
)

'catcht aaaa'     -: main5 _1
'catcht bbb'      -: main5 2
'catcht cc'       -: main5 6
'catcht d'        -: main5 99
'10 not cut back' -: main5 10

18!:55 <'jthrow'


4!:55 ;:'f main main1 main2 main3 main4 main5 sub t'


