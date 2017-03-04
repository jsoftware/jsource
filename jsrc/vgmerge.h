// Inputs:
// GRADEFNNAME - name of the main entry point, normally jmsortxxx
// MERGEFNNAME - name of the merge entry point, normally jmergexxx
// COMPFN - function to call to compare
// 



// merge the sorted lists of pointers lo[] and hi[] into a single list, and return the address of that list
// comp() is the comparison function and compn is a numeric parameter to it.  comp(op0,op1,compn) returns 1 if op0 comes before op1,
//  i. e. comp returns the number of the after argument
// wk[] is a workarea, big enough to hold the combined result.  It may overlap with hi[]
static void** MERGEFNNAME(CMP comp, I compn, void *(lo[]), I lon, void *(hi[]), I hin, void *(wk[])){
 // First, look for in-order or almost-in-order merges.  If the first 3/4 of lo is ahead of all of hi, we
 // can start comparing there.  In fact, we keep looking further, ending up with a check for whether the entire
 // lo is ahead of hi, in which case we can return lo followed by hi unaltered.
 I orderedlen = 0;  // length of prefix that is ahead of hi
 I taillen = lon;  // length of tail, hoping that all before the tail is ordered
 void**wkptr=wk;  // running output pointer
 void **loend=lo+lon;  // address of end+1 of lo
 do{
  taillen>>=2;  // 1/4 of remaining length
  I offsettotry = lon - taillen;
  if(!COMPFN(compn,lo[offsettotry-1],hi[0]))break; // if test value is not below the start of hi, stop looking
  orderedlen = offsettotry;  // remember the count of items before hi
 }while(taillen);
 if(orderedlen){
  // There is an ordered prefix.  See if the whole pair is presorted.
  if(orderedlen==lon){
   // Presorted list.  If the two lists are contiguous, just return their address.  Otherwise copy to be contiguous
   if(loend!=hi){DQ(hin, *loend++=*hi++;)}
   R lo;  // return contiguous result
  }
  // Partial presort.  Copy the ordered part to the output and set to compare the rest
  DQ(orderedlen, *wkptr++=*lo++;);
 }
 void **hiend=hi+hin;  // end+1 of hi
 // Perform the merge into wk[].
 void *loaddr=*lo, *hiaddr=*hi;
 do{
#if 1
 if(COMPFN(compn,loaddr,hiaddr)){++lo; *wkptr++=loaddr; if(lo!=loend)loaddr=*lo;else{if(wkptr!=hi)DQ(hiend-hi, *wkptr++=*hi++;); break;}}
 else{++hi; *wkptr++=hiaddr; if(hi!=hiend)hiaddr=*hi;else{DQ(loend-lo, *wkptr++=*lo++;); break;}}
#else  // this version does not requires prediction of the test but runs slower - odd, since it does just a fetch + test + branch extra
  // We keep processing until one of the lists has been exhausted.  At that point, we copy the other list
  // to the output and return.
  if(lo!=loend){
   if(hi!=hiend){
    // Normal case. compare the leading keys, moving and incrementing the lower.  We do the address arithmetic
    // without branches to avoid misprediction
    void *loaddr=*lo, *hiaddr=*hi;  // adresses of items to compare
    I lomove = COMPFN(compn,loaddr,hiaddr);  // 1 if we should move lo
    lo+=lomove;  // increment lo if we're moving it
    loaddr=(lomove^=1)?hiaddr:loaddr; hi+=lomove;  // increment hi if we're moving it, create store value in loaddr
    *wkptr++=(void *)loaddr;   // move the selected value
   }else{
    // hi[] was exhausted.  Copy the remnant of lo
    DQ(loend-lo, *wkptr++=*lo++;);
    break;  // all finished
   }
  }else{
   // lo[] was exhausted.  Copy the remnant of hi. But
   // if hi[] is occupying the last part of wk[], and lo[] is exhausted first, the remaining part of hi[] is
   // already in place and doesn't need to be copied.
   if(wkptr!=hi)DQ(hiend-hi, *wkptr++=*hi++;);
   break;  // all finished
  }
#endif
 }while(1);
 R wk;  // We have merged into the workarea
}
// sort the values in *in, using *wk as a work area of the same size.  The graded pointers will go into either
// in or wk, and the result will be the address of the graded data
static void** GRADEFNNAME(CMP comp, I compn, void *(in[]), I n, void *(wk[])){void *a,*b,*c,*d,*e,*f;
 switch(n){
 case 0: case 1: R in;
 case 2:  // happens only if original input is 2 long
  if(!COMPFN(compn,in[0],in[1])){void *tmp=in[0]; in[0]=in[1]; in[1]=tmp;} R in;
 case 3:
  a=in[0]; b=in[1];c=in[2];  // abc
  CXCHG(b,c,d);  // abd
  CXCHG(a,b,c);  // acd
  CXCHG(c,d,b);  // acb
  in[0]=a; in[1]=c; in[2]=b; R in;
 case 4:
  a=in[0]; b=in[1];c=in[2]; d=in[3]; // abcd
  CXCHG(a,b,e);   // aecd
  CXCHG(c,d,b);   // aecb
  CXCHG(e,b,d);   // aecd
  CXCHG(a,c,b);   // aebd
  CXCHG(e,b,c);   // aecd
  in[0]=a; in[1]=e; in[2]=c; in[3]=d; R in;
 case 5:
  a=in[0]; b=in[1];c=in[2]; d=in[3]; e=in[4]; // abcde
  CXCHG(b,c,f);   // abfde
  CXCHG(d,e,c);   // abfdc
  CXCHG(b,d,e);   // abfec
  CXCHG(a,f,d);   // abdec
  CXCHG(d,c,f);   // abdef
  CXCHG(a,e,c);   // abdcf
  CXCHG(a,b,e);   // aedcf
  CXCHG(d,c,b);   // aedbf
  CXCHG(e,d,c);   // aecbf
  in[0]=a; in[1]=e; in[2]=c; in[3]=(void *)b; in[4]=(void *)f; R in;
 default:
  // sort the low and high halves, and then merge the results, giving as workarea whatever buffer does not contain lo
  {I lohalf=n>>1; I hihalf=n-lohalf;
   void *lo=GRADEFNNAME(comp, compn, in, lohalf, wk); void *hi=GRADEFNNAME(comp, compn, in+lohalf, hihalf, wk+lohalf);
   R MERGEFNNAME(comp,compn,lo,lohalf,hi,hihalf,(void *)((I)in+(I)wk-(I)lo));
  }
 }
}

#undef COMPFN
#undef MERGEFNNAME
#undef GRADEFNNAME
