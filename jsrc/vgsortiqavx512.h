// sort with avx512
// broad strategy:
// - perform an initial pass, during which we partition the input into the destination
//   this is solidly memory-bound for any interesting size, so we also have the opportunity to record the range
//   (todo and look for patterns)
// - if very small range, switch to counting sort
//   partitioning was mildly pessimal, but not completely pointless, since the individual cache footprint of the lower and upper halves is lesser than the combined footprint would be
//   there might also be the opportunity to use counting sort only for one of the partitions
// - if <=32-bit range, next pass will partition each partition, while also taking the opportunity to squish the elements (basecase will expand them later)
//   minutiae: first squish the upper partition downwards, sort everything, and then expand upwards; this will result in a downwards walk.  Then squish the lower partition upwards, and expand it downwards at the end
//   this pleases the speculative prefetcher

// Requires to be defined:
// SORTQNAME: name of sort fn
// T: type to be sorted
// TZ: sizeof(T)
// TMAXV, TMINV: bounds for type
// CMP(L|G)(M): compare for <|>= (with mask)
// COMP: compress
// COMPS: compress-store
// VMIN(M), VMAX(M): do the needful.  mask merges with lhs
// VMINR/VMAXR: min/max reduce
// V: vector of the correct type
// VL(M): vector load (with mask)
// VS: vector store
// VBC: vector broadcast
// M: mask of the correct width

#define PASTE2(x,y) x##y
#define PASTE(x,y) PASTE2(x,y)

#define VW (64/TZ) //vector width: #elements in a vector

// inplace pivot [zl zh) according to pivot, storing middle pointer to zm
// requires l/h to contain at least 6 vectors
void PASTE(pivotq,PASTE(SORTQNAME,i))(T *zl,T *zh,T **zm,T pivot){
 T ps=PIVOT(zl,(zh-zl));V p=VBC(ps); //todo use PIVOTP
 T *rl=zl,*rh=zh,*wl=zl,*wh=zh;
 // buffered vectors bv*, possibly partial with masks:
 // align rl/rh
 V bvl,bvh;M bvlm,bvhm;
 V bv0,bv1,bv2,bv3; // 4 buffer vectors, and possibly one vector of buffer
 bvl=ALIGNUP(&rl,&bvlm);
 bvh=ALIGNDN(&rh,&bvhm);
 bv0=VL(rl);rl+=VW;
 bv1=VL(rl);rl+=VW;
 bv2=VL(rh-=VW);
 bv3=VL(rh-=VW);
 I4 hs=0,ls=0; // track amount of free space left in each of the low and high parts.  Doesn't need to be exact, so just initialise to zero; we care about their relative positions
 // if odd #vectors, ensure even
 if(VW&(rh-rl)){
  COMPCA(&wl,&wh,VL(rl),p,&ls,&hs);
  ls+=VW;rl+=VW;}
 I niter=(UI)(rh-rl)/(UI)(2*VW);
 rh-=2*VW; // now implicitly offset by 2 vectors, so we can always read 2 vectors from either of rl and rh
 while(niter--){
  T *ptr=ls<hs?rl:rh;
  I lad=ls<hs; // did we adjust the low read head or the high one?
  ls+=lad*2*VW;hs+=(2*VW)^(lad*2*VW); // whichever we're adjusting, we made space for two vectors
  rl+=lad*2*VW;rh-=(2*VW)^(lad*2*VW);
  V v0=VL(ptr),v1=VL(ptr+VW);
  COMPCA(&wl,&wh,v0,p,&ls,&hs);
  COMPCA(&wl,&wh,v1,p,&ls,&hs);}
 COMPCM(&wl,&wh,bvl,p,bvlm);
 COMPCM(&wl,&wh,bvh,p,bvhm);
 COMPC(&wl,&wh,bv0,p);
 COMPC(&wl,&wh,bv1,p);
 COMPC(&wl,&wh,bv2,p);
 COMPC(&wl,&wh,bv3,p);
 //assert(wl==wh);
 *zm=wl;}

// ditto, but pick a pivot for me, and write it out to pivot
void PASTE(pivotq,PASTE(SORTQNAME,ip))(T *zl,T *zh,T **zm,T *pivot){
 PASTE(pivotq,PASTE(SORTQNAME,i))(zl,zh,zm,*pivot=PIVOT(zl,(zh-zl)));}

// pick a pivot for me, and also calculate bounds
void PASTE(pivotq,PASTE(SORTQNAME,ib))(T *zl,T *zh,T **zm,T *min,T *max,T *pivot){
 T ps=PIVOT(zl,(zh-zl));V p=VBC(ps); //todo use PIVOTP
 V vmin=VBC(TMAXV),vmax=VBC(TMINV);
 T *rl=zl,*rh=zh,*wl=zl,*wh=zh;
 V bvl,bvh;M bvlm,bvhm;
 V bv0,bv1,bv2,bv3;
 // align rl/rh
 bvl=ALIGNUP(&rl,&bvlm);vmin=VMINM(vmin,bvl,bvlm);vmax=VMAXM(vmax,bvl,bvlm);
 bvh=ALIGNDN(&rh,&bvhm);vmin=VMINM(vmin,bvh,bvhm);vmax=VMAXM(vmax,bvh,bvhm);
 // clear space for two vectors from each of rh/l into the buffer
 bv0=VL(rl);rl+=VW;vmin=VMIN(vmin,bv0);vmax=VMAX(vmax,bv0);
 bv1=VL(rl);rl+=VW;vmin=VMIN(vmin,bv1);vmax=VMAX(vmax,bv1);
 bv2=VL(rh-=VW);   vmin=VMIN(vmin,bv2);vmax=VMAX(vmax,bv2);
 bv3=VL(rh-=VW);   vmin=VMIN(vmin,bv3);vmax=VMAX(vmax,bv3);
 I4 hs=0,ls=0; // track amount of free space left in each of the low and high parts.  Doesn't need to be exact, so just initialise to zero; we care about their relative positions
 // ensure even #vectors left
 if(VW&(rh-rl)){
  V t=VL(rl); COMPCA(&wl,&wh,t,p,&ls,&hs);vmin=VMIN(vmin,t);vmax=VMAX(vmax,t);
  ls+=VW;rl+=VW;}
 I niter=(UI)(rh-rl)/(UI)(2*VW);
 rh-=2*VW; // now implicitly offset by 2 vectors, so we can always read 2 vectors from either of rl and rh
 while(niter--){
  T *ptr=ls<hs?rl:rh;
  I lad=ls<hs; // did we adjust the low read head or the high one?
  ls+=lad*2*VW;hs+=(2*VW)^(lad*2*VW); // whichever we're adjusting, we made space for two vectors
  rl+=lad*2*VW;rh-=(2*VW)^(lad*2*VW);
  V v0=VL(ptr),v1=VL(ptr+VW);
  vmin=VMIN(vmin,VMIN(v0,v1));
  vmax=VMAX(vmax,VMAX(v0,v1));
  COMPCA(&wl,&wh,v0,p,&ls,&hs);
  COMPCA(&wl,&wh,v1,p,&ls,&hs);}
 COMPCM(&wl,&wh,bvl,p,bvlm);
 COMPCM(&wl,&wh,bvh,p,bvhm);
 COMPC(&wl,&wh,bv0,p);
 COMPC(&wl,&wh,bv1,p);
 COMPC(&wl,&wh,bv2,p);
 COMPC(&wl,&wh,bv3,p);
 //assert(wl==wh);
 *zm=wl;
 *pivot=ps;
 *min=VMINR(vmin);
 *max=VMAXR(vmax);}

#undef VW
#undef PASTE
#undef PASTE2

#undef ALIGNDN
#undef ALIGNUP
#undef COMPCM
#undef COMPC
#undef COMPCA
#undef VBC

#undef VMAXR
#undef VMINR
#undef VMINM
#undef VMAXM
#undef VMIN
#undef VMAX
#undef M
#undef TMINV
#undef TMAXV
#undef TZ
#undef T
#undef SORTQNAME
