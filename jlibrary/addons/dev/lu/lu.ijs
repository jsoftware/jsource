NB. LU decomposition with permutation
NB. y is square matrix
NB. result is (permutation);(LU in Doolittle form)
NB. L and U are mashed together with the principal diagonal of L (which is all 1) omitted
Lu_j_=: 3 : 0
d =. _1 x: #y  NB. dimension of array, as integer
pv =. i. d NB. initialize permutation array to index vector
remvec =. i. d  NB. vector to fetch from i to end of vector
for_i. i. d do.  NB. for each ring
  col =. (<remvec;i) { y NB. fetch current column
  pivot =. col {~ pivotx =. (i. >./) | col  NB. find index of largest absolute value; save the value as pivot
  NB. swap the permutation array, and swap the rows of the matrix, and the values read for current column
  col =. ((0,pivotx){col) (0,~pivotx)} col
  y =. ((i,i+pivotx){y) (i,~i+pivotx)} y
  pv =. ((i,i+pivotx){pv) (i,~i+pivotx)} pv
  col =. (}. col) * %pivot  NB. divide the current column by the pivot
  remvec =. }. remvec  NB.  shorten the fetch vectors by 1 from the front
  y =. col (<remvec;i)} y  NB. store the current column into the array as L; leave U unchanged
  y =. (((<remvec;remvec) { y) - col */ (<i;remvec) { y) (<remvec;remvec)} y  NB. subtract (current column */ (}.first row)) from lower-right block
end.
pv;y
)