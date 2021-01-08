// Inputs:
// GRADEFNNAME - name of the main entry point, normally jmsortxxx
// MERGEFNNAME - name of the merge entry point, normally jmergexxx
// COMPFN - function to call to compare, compn the data to call it with
// T - type of direct data (void if we are comparing using function call)
// PTRADD(p,ic) address of item at p[i]
// MVITEMS(dest,src,itemct)  move items from src to dest, incrementing pointers
// 

// merge the sorted lists of pointers lo[] and hi[] into a single list, and return the address of that list
// comp() is the comparison function and compn is a numeric parameter to it.  comp(op0,op1,compn) returns 1 if op0 comes before op1,
//  i. e. comp returns the number of the after argument
// wk[] is a workarea, big enough to hold the combined result.  It may overlap with hi[]
// We don't use RESTRICT on lo/hi/wkptr, even though the areas are separate, because VS will convert the loop
// to a memcpy, which is slower for the item-sizes we expect
static void* MERGEFNNAME(CMP comp, I compn, I bpi, T * lo, I lon, T * hi, I hin, T * RESTRICT wk){
 // First, look for in-order or almost-in-order merges.  If the first 3/4 of lo is ahead of all of hi, we
 // can start comparing there.  In fact, we keep looking further, ending up with a check for whether the entire
 // lo is ahead of hi, in which case we can return lo followed by hi unaltered.
 I orderedlen = 0;  // length of prefix that is ahead of hi
 I taillen = lon;  // length of tail, hoping that all before the tail is ordered
 T * wkptr=wk;  // running output pointer
 T * RESTRICT loend=PTRADD(lo,lon);  // address of end+1 of lo
 do{
  taillen>>=2;  // 1/4 of remaining length
  I offsettotry = lon - taillen;
  if(!COMPFN(compn,PTRADD(lo,offsettotry-1),hi))break; // if test value is not below the start of hi, stop looking
  orderedlen = offsettotry;  // remember the count of items before hi
 }while(taillen);
 if(orderedlen){
  // There is an ordered prefix.  See if the whole pair is presorted.
  if(orderedlen==lon){
   // Presorted list.  If the two lists are contiguous, just return their address.  Otherwise copy to be contiguous
   if(loend!=hi){MCL(loend,hi,hin*bpi);}
   R lo;  // return contiguous result
  }
  // Partial presort.  Because the presorted part is larger, leave it in place and copy the shorter remnant at the end
  // to the workarea; then change the pointers so we merge the moved remnant onto the end of the presorted fragment
  wk=lo; wkptr=PTRADD(wkptr,orderedlen); lo=PTRADD(lo,orderedlen); lon-=orderedlen;MCL(wkptr,lo,lon*bpi);   // move wkptr&lo to end of block; lon=length of remnant; move remnant to wk; point wk to start of original input
  T *tmp = wkptr; loend=PTRADD(wkptr,lon); wkptr=lo; lo=tmp;   // lo, loend point into workarea; wkptr-> input area
 }
 T *hiend=PTRADD(hi,hin);  // end+1 of hi
 // Perform the merge into wk[].
 do{
#if 0
  if(COMPFN(compn,lo,hi)){
   MVITEMS(wkptr,lo,1);
   if(lo==loend){
    if(wkptr!=hi)MCL(wkptr,hi,(C*)hiend-(C*)hi); break;
   }
  }else{
   MVITEMS(wkptr,hi,1);
   if(hi==hiend){
    MCL(wkptr,lo,(C*)loend-(C*)lo); break;
   }
  }
#else  // this version does not requires prediction of the test.  Faster, because we are mostly in cache and there is not a long delay for any fetch
  // We keep processing until one of the lists has been exhausted.  At that point, we copy the other list
  // to the output and return.
  if(lo!=loend){
   if(hi!=hiend){
    // Normal case. compare the leading keys, moving and incrementing the lower.  We do the address arithmetic
    // without branches to avoid misprediction
    I lomove = COMPFN(compn,lo,hi);  // 1 if we should move lo
    // Select the address to move from
    T *fromptr=lo; fromptr=(lomove-=1)?hi:fromptr;
    // Advance the selected address by one item.  We do this before the move loop so
    // that the fetches for the next compare can start while the copy is going on
    // lomove now is 0 if lo was selected, ~0 if hi was selected
    hi=(void *)((C*)hi+(lomove&=bpi)); lo=(void *)((C*)lo+(lomove^=bpi));
    // copy to the output
    MVITEMS(wkptr,fromptr,1);  // move, and advance wkptr
   }else{
    // hi[] was exhausted.  Copy the remnant of lo
    MCL(wkptr,lo,(C*)loend-(C*)lo); 
    break;  // all finished
   }
  }else{
   // lo[] was exhausted.  Copy the remnant of hi. But
   // if hi[] is occupying the last part of wk[], and lo[] is exhausted first, the remaining part of hi[] is
   // already in place and doesn't need to be copied.
   if(wkptr!=hi)MCL(wkptr,hi,(C*)hiend-(C*)hi); 
   break;  // all finished
  }
#endif
 }while(1);
 R wk;  // We have merged into the workarea
}

// sort the values in *wv, using *wk as a work area of the same size.  in and wk are the main output and the workarea to use.
// On the first pass, where we order small blocks of the input, we draw from *wv and create sorted strings in *in.  Thereafter
// we draw from *in.  We do this rather than copying the input into *wv to begin with, because that copy might run out of cache.
// By doing the sort we have more work to do between copied blocks & can take more time to prefetch into cache.
static T* GRADEFNNAME(CMP comp, I compn, I n, I bpi, T * RESTRICT in, T * RESTRICT wk,T * RESTRICT wv){T *a,*b,*c,*d,*e,*iin=in;
 switch(n){
 case 5:
  a=wv; b=PTRADD(a,1); c=PTRADD(b,1); d=PTRADD(c,1); e=PTRADD(d,1);
  CXCHG2(b,c);
  CXCHG2(d,e);
  CXCHG2(b,d);
  CXCHG2(a,c);
  CXCHG2(a,d);
  CXCHG2(c,e);
  CXCHG2(a,b);
  CXCHG2(c,d);
  CXCHG2(b,c);
  MVITEMS(in,a,1);
  goto exitbcde;
 case 1: e=wv; goto exite; 
 case 2:  // happens only if original input is 2 long
  d=wv; e=PTRADD(d,1);
  CXCHG2(d,e);
  goto exitde;
 case 3:
  c=wv; d=PTRADD(c,1); e=PTRADD(d,1);
  CXCHG2(d,e);
  CXCHG2(c,d);
  CXCHG2(d,e);
  goto exitcde;
 case 4:
  b=wv; c=PTRADD(b,1); d=PTRADD(c,1); e=PTRADD(d,1);
  CXCHG2(b,c);
  CXCHG2(d,e);
  CXCHG2(b,d);
  CXCHG2(c,e);
  CXCHG2(c,d);
exitbcde:
  MVITEMS(in,b,1);
exitcde:
  MVITEMS(in,c,1);
exitde:
  MVITEMS(in,d,1);
exite:
  MVITEMS(in,e,1);
case 0:
  R iin;

 default:
  // sort the low and high halves, and then merge the results, giving as workarea whatever buffer does not contain lo
  {I lohalf=n>>1; I hihalf=n-lohalf;
   T *lo=GRADEFNNAME(comp, compn, lohalf, bpi, in, wk, wv); T *hi=GRADEFNNAME(comp, compn, hihalf, bpi, PTRADD(in,lohalf), PTRADD(wk,lohalf), PTRADD(wv,lohalf));
   R MERGEFNNAME(comp,compn,bpi,lo,lohalf,hi,hihalf,(T *)((I)in+(I)wk-(I)lo));
  }
 }

}

#undef COMPFN
#undef MERGEFNNAME
#undef GRADEFNNAME
#undef T
#undef PTRADD
#undef MVITEMS