prolog './g1x30.ijs'
NB. 1!:30 ---------------------------------------------------------------

sys=: 9!:12 ''

3 : 0 ''
NB. if. sys e. 0 1 2 5 6 7 do.
if. sys e. 0 1 2 6 7 do.

write  =: 1!:2
open   =: 1!:21
close  =: 1!:22
locks  =: 1!:30
lock   =: 1!:31
unlock =: 1!:32

f =: <jpath '~temp/foo.x'
(128$'a') write f
h =: open f

k =: locks ''
assert. lock h,0 8
assert. lock h,8 16
if. sys~:5 do.
assert. 0 -: lock h,0 16           NB. windows and *nix different behavior
end.
assert. ((h,0 8),:h,8 16)e.locks ''

assert. unlock h,0 8
assert. unlock h,8 16
assert. k -: locks ''

assert. lock h,0 8
assert. lock h,8 16
assert. lock h,64 1
close h
assert. k -: locks ''

assert. 'domain error'      -: lock   etx 'a'
assert. 'domain error'      -: lock   etx 'abc'
assert. 'domain error'      -: lock   etx 3.45
assert. 'domain error'      -: lock   etx 3j4
assert. 'domain error'      -: lock   etx 0;1;0
assert. 'domain error'      -: lock   etx 4;5;6

assert. 'file number error' -: lock   etx 0 0 1
assert. 'file number error' -: lock   etx 1 4 5
assert. 'file number error' -: lock   etx 2 4 5
assert. 'file number error' -: lock   etx 12345 12346,"0 1 [ 4 5

assert. 'domain error'      -: unlock etx 'abc'
assert. 'domain error'      -: unlock etx 3.45 5
assert. 'domain error'      -: unlock etx 3j4 
assert. 'domain error'      -: unlock etx 0;1;0
assert. 'domain error'      -: unlock etx 4;5;6
assert. 'domain error'      -: unlock etx 0 0 1

assert. 'index error'       -: unlock etx 3
assert. 'index error'       -: unlock etx 4
assert. 'index error'       -: unlock etx 1234 5 
assert. 'index error'       -: unlock etx i.2 4
assert. 'index error'       -: unlock etx 1 4 5
assert. 'index error'       -: unlock etx 2 4 5
assert. 'index error'       -: unlock etx 12345 12346,"0 1 [ 4 5
end.
1
)





epilog''

