NB. goto ----------------------------------------------------------------

fc =: 3 : 0
 if. y do. goto_true. else. goto_false. end.
 label_true.  'true'  return.
 label_false. 'false' return.
)

'true'  -: fc 1
'false' -: fc 0

fc=: 3 : 0
 if. y do. goto_true. else. goto_false. end.
 label_true.  'true'  return.
 label_false. 'false' return.
 label_truee. 'ok'
)

'true'  -: fc 1
'false' -: fc 0

fc=: 3 : 0
 if. y do. goto_true. else. goto_false. end.
 label_true.  'true'  return.
 label_false. 'false' return.
 label_tru. 'ok'
)

'true'  -: fc 1
'false' -: fc 0

xx=: 0 : 0
 if. y do. goto_true. else. goto_false. end.
 label_true.  'true'  return.
 label_false. 'false' return.
 label_true. 'bad'
)

'control error' -: ex '3 : xx'
 
xx=: 0 : 0
 if. y do. goto_true. else. goto_false. end.
 label_true.  label_true. 'true'  return.
 label_false. 'false' return.
)

'control error' -: ex '3 : xx'
 
def=: 3 : 0 " 0
 ('control error';'') i. < ex '3 : (yy=: (y{.xx),goto,y}.xx)'
)

goto=: <'goto_it.'
lab =: <'label_it.'

build=: 3 : 0
 (y{.bod),lab,y}.bod
)

bod=: (<;._2) 0 : 0
 select.      NB. 0
  if.         NB. 1
   y          NB. 2
  do.         NB. 3
   1          NB. 4
  else.       NB. 5
   0          NB. 6
  end.        NB. 7
 fcase.       NB. 8
   0          NB. 9
 do.          NB. 10
   'zero'     NB. 11
 case.        NB. 12
   1          NB. 13
 do.          NB. 14
  'one'       NB. 15
 end.         NB. 16
)

1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 -: b=: def i.1+#xx=: build 0
0 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 -: b=: def i.1+#xx=: build 1
0 0 1 1 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 -: b=: def i.1+#xx=: build 2
0 0 1 1 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 -: b=: def i.1+#xx=: build 3
0 0 0 0 1 1 1 0 0 0 0 0 0 0 0 0 0 0 0 -: b=: def i.1+#xx=: build 4
0 0 0 0 1 1 1 0 0 0 0 0 0 0 0 0 0 0 0 -: b=: def i.1+#xx=: build 5
0 0 0 0 0 0 1 1 1 0 0 0 0 0 0 0 0 0 0 -: b=: def i.1+#xx=: build 6
0 0 0 0 0 0 1 1 1 0 0 0 0 0 0 0 0 0 0 -: b=: def i.1+#xx=: build 7
0 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 -: b=: def i.1+#xx=: build 8
0 0 0 0 0 0 0 0 0 1 1 1 0 0 0 0 0 0 0 -: b=: def i.1+#xx=: build 9
0 0 0 0 0 0 0 0 0 1 1 1 0 0 0 0 0 0 0 -: b=: def i.1+#xx=: build 10
0 0 0 0 0 0 0 0 0 0 0 1 1 1 0 0 0 0 0 -: b=: def i.1+#xx=: build 11
0 0 0 0 0 0 0 0 0 0 0 1 1 1 0 0 0 0 0 -: b=: def i.1+#xx=: build 12
0 0 0 0 0 0 0 0 0 0 0 0 0 1 1 1 0 0 0 -: b=: def i.1+#xx=: build 13
0 0 0 0 0 0 0 0 0 0 0 0 0 1 1 1 0 0 0 -: b=: def i.1+#xx=: build 14
0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 1 1 0 -: b=: def i.1+#xx=: build 15
0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 1 1 0 -: b=: def i.1+#xx=: build 16
1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 -: b=: def i.1+#xx=: build 17

bod=: (<;._2) 0 : 0
 while.       NB. 0
  if.         NB. 1
   y          NB. 2
  do.         NB. 3
   1          NB. 4
  elseif.     NB. 5
   2          NB. 6
  do.         NB. 7
   3          NB. 8
  end.        NB. 9
 do.          NB. 10
  4           NB. 11
  try.        NB. 12
   5          NB. 13
  catch.      NB. 14
   6          NB. 15
  end.        NB. 16
 end.         NB. 17
)

1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 -: b=: def i.1+#xx=: build 0
0 1 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 -: b=: def i.1+#xx=: build 1
0 0 1 1 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 -: b=: def i.1+#xx=: build 2
0 0 1 1 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 -: b=: def i.1+#xx=: build 3
0 0 0 0 1 1 1 0 0 0 0 0 0 0 0 0 0 0 0 0 -: b=: def i.1+#xx=: build 4
0 0 0 0 1 1 1 0 0 0 0 0 0 0 0 0 0 0 0 0 -: b=: def i.1+#xx=: build 5
0 0 0 0 0 0 1 1 1 0 0 0 0 0 0 0 0 0 0 0 -: b=: def i.1+#xx=: build 6
0 0 0 0 0 0 1 1 1 0 0 0 0 0 0 0 0 0 0 0 -: b=: def i.1+#xx=: build 7
0 0 0 0 0 0 0 0 1 1 1 0 0 0 0 0 0 0 0 0 -: b=: def i.1+#xx=: build 8
0 0 0 0 0 0 0 0 1 1 1 0 0 0 0 0 0 0 0 0 -: b=: def i.1+#xx=: build 9
0 1 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 -: b=: def i.1+#xx=: build 10
0 0 0 0 0 0 0 0 0 0 0 1 1 1 1 1 1 1 1 0 -: b=: def i.1+#xx=: build 11
0 0 0 0 0 0 0 0 0 0 0 1 1 1 1 1 1 1 1 0 -: b=: def i.1+#xx=: build 12
0 0 0 0 0 0 0 0 0 0 0 0 0 1 1 1 0 0 0 0 -: b=: def i.1+#xx=: build 13
0 0 0 0 0 0 0 0 0 0 0 0 0 1 1 1 0 0 0 0 -: b=: def i.1+#xx=: build 14
0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 1 1 0 0 -: b=: def i.1+#xx=: build 15
0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 1 1 0 0 -: b=: def i.1+#xx=: build 16
0 0 0 0 0 0 0 0 0 0 0 1 1 1 1 1 1 1 1 0 -: b=: def i.1+#xx=: build 17
1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 -: b=: def i.1+#xx=: build 18

bod=: (<;._2) 0 : 0
 0           NB. 0
 if.         NB. 1
  y          NB. 2
 do.         NB. 3
  for.       NB. 4 
   1         NB. 5
  do.        NB. 6
   2         NB. 7
  end.       NB. 8
 else.       NB. 9
  whilst.    NB. 10
   3         NB. 11
  do.        NB. 12
   4         NB. 13
  end.       NB. 14
 end.        NB. 15
)

1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 -: b=: def i.1+#xx=: build 0
1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 -: b=: def i.1+#xx=: build 1
0 0 1 1 1 0 0 0 0 0 0 0 0 0 0 0 0 0 -: b=: def i.1+#xx=: build 2
0 0 1 1 1 0 0 0 0 0 0 0 0 0 0 0 0 0 -: b=: def i.1+#xx=: build 3
0 0 0 0 1 1 1 1 1 1 1 0 0 0 0 0 0 0 -: b=: def i.1+#xx=: build 4
0 0 0 0 0 1 1 1 0 0 0 0 0 0 0 0 0 0 -: b=: def i.1+#xx=: build 5
0 0 0 0 0 1 1 1 0 0 0 0 0 0 0 0 0 0 -: b=: def i.1+#xx=: build 6
0 0 0 0 0 0 0 1 1 1 0 0 0 0 0 0 0 0 -: b=: def i.1+#xx=: build 7
0 0 0 0 0 0 0 1 1 1 0 0 0 0 0 0 0 0 -: b=: def i.1+#xx=: build 8
0 0 0 0 1 1 1 1 1 1 1 0 0 0 0 0 0 0 -: b=: def i.1+#xx=: build 9
0 0 0 0 0 0 0 0 0 0 1 1 1 1 1 1 1 0 -: b=: def i.1+#xx=: build 10
0 0 0 0 0 0 0 0 0 0 0 1 1 1 0 0 0 0 -: b=: def i.1+#xx=: build 11
0 0 0 0 0 0 0 0 0 0 0 1 1 1 0 0 0 0 -: b=: def i.1+#xx=: build 12
0 0 0 0 0 0 0 0 0 0 0 0 0 1 1 1 0 0 -: b=: def i.1+#xx=: build 13
0 0 0 0 0 0 0 0 0 0 0 0 0 1 1 1 0 0 -: b=: def i.1+#xx=: build 14
0 0 0 0 0 0 0 0 0 0 1 1 1 1 1 1 1 0 -: b=: def i.1+#xx=: build 15
1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 -: b=: def i.1+#xx=: build 16


4!:55 ;:'b bod build def fc goto i lab xx yy'


