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
// requires l/h to contain at least 7 vectors
void PASTE(pivotq,PASTE(SORTQNAME,i))(T *zl,T *zh,T **zm,T pivot){
 T ps=PIVOT(zl,(zh-zl));V p=VBC(ps); //todo use PIVOTP
 enum { BUFZ = 448 / TZ }; // <=1x alignment vector from each of low/high, plus 2x buffer vectors, plus 1x to ensure even vector count
 T buf[BUFZ],*bl=buf,*bh=buf+BUFZ;
 T *rl=zl,*rh=zh,*wl=zl,*wh=zh;
 // align rl/rh
 {M ml,mh;V vl=ALIGNUP(&rl,&ml),vh=ALIGNDOWN(&rh,&mh);
  COMPCM(&bl,&bh,vl,p,ml);
  COMPCM(&bl,&bh,vh,p,mh);}
 // clear space for two vectors from each of rh/l into the buffer
 COMPC(&bl,&bh,VL(rl),p);rl+=VW;
 COMPC(&bl,&bh,VL(rl),p);rl+=VW;
 COMPC(&bl,&bh,VL(rh-VW),p);rh-=VW;
 COMPC(&bl,&bh,VL(rh-VW),p);rh-=VW;
 // ensure even #vectors left
 if(64&((I)rh-(I)rl)){ COMPC(&bl,&bh,VL(rl),p);rl+=VW; }
 I4 hs=0,ls=0; // track amount of free space left in each of the low and high parts.  Doesn't need to be exact, so just initialise to zero; we care about their relative positions
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
 assert(wh-wl==(bl-buf)+(buf+BUFZ-bh));
 memcpy(wl,buf,TZ*(bl-buf));
 wl+=bl-buf;
 memcpy(wl,bh,TZ*(buf+BUFZ-bh));
 *zm=wl;}

// ditto, but pick a pivot for me, and write it out to pivot
void PASTE(pivotq,PASTE(SORTQNAME,ip))(T *zl,T *zh,T **zm,T *pivot){
 PASTE(pivotq,PASTE(SORTQNAME,i))(zl,zh,zm,*pivot=PIVOT(zl,(zh-zl)));}

// pick a pivot for me, and also calculate bounds
void PASTE(pivotq,PASTE(SORTQNAME,ib))(T *zl,T *zh,T **zm,T *min,T *max,T *pivot){
 T ps=PIVOT(zl,(zh-zl));V p=VBC(ps); //todo use PIVOTP
 enum { BUFZ = 448 / TZ }; // <=1x alignment vector from each of low/high, plus 2x buffer vectors, plus 1x to ensure even vector count
 V vmin=VBC(TMAXV),vmax=VBC(TMINV);
 T buf[BUFZ],*bl=buf,*bh=buf+BUFZ;
 T *rl=zl,*rh=zh,*wl=zl,*wh=zh;
 // align rl/rh
 {M ml,mh;V vl=ALIGNUP(&rl,&ml),vh=ALIGNDOWN(&rh,&mh);
  vmin=VMINM(VMINM(vmin,vl,ml),vh,mh);
  vmax=VMAXM(VMAXM(vmax,vl,ml),vh,mh);
  COMPCM(&bl,&bh,vl,p,ml);
  COMPCM(&bl,&bh,vh,p,mh);}
 // clear space for two vectors from each of rh/l into the buffer
 {V tv;
  COMPC(&bl,&bh,tv=VL(rl),   p);rl+=VW;vmin=VMIN(vmin,tv);vmax=VMAX(vmax,tv);
  COMPC(&bl,&bh,tv=VL(rl),   p);rl+=VW;vmin=VMIN(vmin,tv);vmax=VMAX(vmax,tv);
  COMPC(&bl,&bh,tv=VL(rh-VW),p);rh-=VW;vmin=VMIN(vmin,tv);vmax=VMAX(vmax,tv);
  COMPC(&bl,&bh,tv=VL(rh-VW),p);rh-=VW;vmin=VMIN(vmin,tv);vmax=VMAX(vmax,tv);}
 // ensure even #vectors left
 if(64&((I)rh-(I)rl)){ COMPC(&bl,&bh,VL(rl),p);vmin=VMIN(vmin,VL(rl));vmax=VMAX(vmax,VL(rl));rl+=VW; }
 I4 hs=0,ls=0; // track amount of free space left in each of the low and high parts.  Doesn't need to be exact, so just initialise to zero; we care about their relative positions
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
 assert(wh-wl==(bl-buf)+(buf+BUFZ-bh));
 memcpy(wl,buf,TZ*(bl-buf));
 wl+=bl-buf;
 memcpy(wl,bh,TZ*(buf+BUFZ-bh));
 *zm=wl;
 *pivot=ps;
 *min=VMINR(vmin);
 *max=VMAXR(vmax);}

#undef VW
#undef PASTE
#undef PASTE2

#undef ALIGNDOWN
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
