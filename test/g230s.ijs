prolog './g230s.ijs'
NB. |/\. B -------------------------------------------------------------

(0 1 0 0,:0 1 0 1) -: |/\. 0 0 1 1 ,: 0 1 0 1

rem=: 4 : 'x|y'

f=: 3 : '(|/\. -: rem/\.) y ?@$ 2'
,f"1 x=.7 8 9,."0 1 [ _1 0 1+  255
,f"1 |."1 x
,f"1 x=.7 8 9,."0 1 [ _1 0 1+4*255
,f"1 |."1 x

'domain error' -: |/\. etx 'deipnosophist'
'domain error' -: |/\. etx ;:'professors in New England'
'domain error' -: |/\. etx u:'deipnosophist'
'domain error' -: |/\. etx u:&.> ;:'professors in New England'
'domain error' -: |/\. etx 10&u:'deipnosophist'
'domain error' -: |/\. etx 10&u:&.> ;:'professors in New England'
'domain error' -: |/\. etx s:@<"0 'deipnosophist'
'domain error' -: |/\. etx s:@<"0&.> ;:'professors in New England'
'domain error' -: |/\. etx <"0@s: ;:'professors in New England'




epilog''

