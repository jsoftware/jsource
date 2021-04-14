// define SORTQNAME to be the function name, SORTQTYPE to be the type of items sorted
// v->first value, n=# values
SORTQSCOPE void SORTQNAME(SORTQTYPE *v, I n){
 // loop till tail-recursion finished
 I l=0, r=n-1; I stackp=0; I stack[32][2];
 while(1){
  // the batch is short, sort it by fixed comparison; then pick up the next batch
  // we write this out in situ rather than calling a subroutine because there's not much register pressure
  // here and the compiler will not need to save/restore registers.  This doesn't matter for the floating-point version but it does for integer.
  while(r-l<=4){SORTQTYPE a,b,c,d,e;UI f;
   SORTQTYPE *vv = v+l;  // advance to the block to be reordered 
   switch(r-l){   // this is 1 LESS than the length of the block
   case 4:
    // read em in
    a=vv[0]; b=vv[1]; c=vv[2]; d=vv[3]; e=vv[4];
    // compare each value against each other value, and assemble the bits into f in order of 0-4
    // Then read the ordering value from the table.  This read may miss in cache but we don't care too much, since there is no dependency on the result of the read
    f=orderfromcomp5[((a>b)<<9) + ((a>c)<<8) + ((a>d)<<7) + ((a>e)<<6) + ((b>c)<<5) + ((b>d)<<4) + ((b>e)<<3) + ((c>d)<<2) + ((c>e)<<1) + (d>e)];
    // Store each value in its correct spot  Code to minimize the # instructions held waiting for the fetch to complete
    vv[f&7]=a; vv[(f>>3)&7]=b; vv[(f>>6)&7]=c; vv[(f>>9)&7]=d; vv[f>>12]=e;
    break;
   case 1:
    a=vv[0]; b=vv[1]; f=a>b; vv[f]=a; vv[f^1]=b;
    break;
   case 2:
    a=vv[0]; b=vv[1]; c=vv[2];
    f=orderfromcomp3[((a>b)<<2) + ((a>c)<<1) + (b>c)];
    vv[f&3]=a; vv[(f>>2)&3]=b; vv[f>>4]=c;
    break;
   case 3:
    a=vv[0]; b=vv[1]; c=vv[2]; d=vv[3];
    f=orderfromcomp4[((a>b)<<5) + ((a>c)<<4) + ((a>d)<<3) + ((b>c)<<2) + ((b>d)<<1) + (c>d)];
    vv[f&3]=a; vv[(f>>2)&3]=b; vv[(f>>4)&3]=c; vv[f>>6]=d;
    break;
   case -1: case 0:;
   }
   // that batch is sorted; move to the next
   if(--stackp<0)R;  // back up stack; if we're finishing the last call, we're through
   l=stack[stackp][0]; r=stack[stackp][1];  // resume the next 
  }
  // medium or long batch (6 or more): partition it and recur
  // choose the pivot as the median of first/last/middle, and exchange it with the end of the array
  SORTQTYPE pivot;
  {
   // actually we advance the first/last to quartile positions, to avoid a worst-case when an almost-sorted list has some very small numbers
   // added to the end.  In that case a small value is chosen as the pivot and replaced at the end with another small value
   I ll=l+((r-l)>>2), rr=r-((r-l)>>2); 
   I pivotcomp = 0xc6>>(4*(v[ll]>v[(ll+rr)>>1]) + 2*(v[ll]>v[rr]) + (v[(ll+rr)>>1]>v[rr]));  // l>m, l>r, m>r 000 lmr  001 lrm  010 -  011 rlm  100 mlr  101 -  110 mrl  111 rml
      // encode l as 00 r as 11 m as 01/10, the sequence is mr-ll-rm 01 11 10 00 00 01 11 10  0 1 1 0 0 0 1 1 0 (LE) -> 011000110  0xc6
   I pivotx=((pivotcomp&1?rr:ll)+(pivotcomp&2?rr:ll))>>1; pivot=v[pivotx]; v[pivotx]=v[r];  // pick the median pivot, swap it (notionally) with the last 
  }
#if SORTQCOND
   SORTQTYPE256 pivot256=SORTQSET256(pivot,pivot,pivot,pivot);  // copy pivot across regs
#endif

  I xchgx0, xchgx1;  // partition running pointers.  At the end xchgx0 is the end of the left side, xchgx1 the beginning of the right
  if(r-l<=BW){
   // medium-size partition.  Use a single comparison stack, processed in both directions.  We update the comparisons as we make them, so this looks like
   // classic quicksort: when the pointers cross, they are pointing to the end of the opposite partition

   // Create the comparison list, which is in shape to use for the right partition, i. e. 1 means value is < pivot.  Set upper bits to 0.  The bits match memory addresses, i. e.
   // bit 0 is the lowest bit & corresponds to the beginning of the partition
   SORTQTYPE *v0=v+l;  // base of the partitioned region
   UI cstk=0;  // will hold comparison results.  Init to 0 for MSBs so as not to interfere with finding highest 1
#if SORTQCOND
   // go back to front, shifting bits up from the bottom
   {
   __m256i endmask;
   endmask = _mm256_loadu_si256((__m256i*)(validitymask+((-(r-l))&(NPAR-1))));  /* mask for 00=1111, 01=1000, 10=1100, 11=1110 */
   SORTQTYPE *vv=v0+(((r-l)-1)&(-NPAR));  // pointer to current batch, starting at end
   cstk=_mm256_movemask_pd(_mm256_and_pd(_mm256_castsi256_pd(endmask),SORTQCASTTOPD(SORTQCMP256(pivot256,SORTQMASKLOAD(vv,endmask) SORTQCMPTYPE))));
   do{vv-=NPAR; cstk=16*cstk+_mm256_movemask_pd(SORTQCASTTOPD(SORTQCMP256(pivot256,SORTQULOAD((SORTQULOADTYPE)vv) SORTQCMPTYPE)));}while(vv!=v0);  // >5 items, so safe to use while
   }
#else
   DQ(r-l, cstk=2*cstk+(v0[i]<pivot);)  // back to front, to leave cstk in normal order.  The last slot is empty, so don't compare.  We can have up to 64 compares.
#endif

   if(!(cstk&(cstk+1))) {
    // Here there is no 0-bit with a higher 1-bit, i. e. no exchange to be  performed.  We have to check this separately because CT[LT]ZI is unpredictable on all-0 input.
    // The cases are: 00000 (lower partition empty), 11111 (upper partition empty), 00111 (no partition required)
    if(cstk==0){
     // The case 00000 is ominous: we might be handling a partition with many repeated values (equal to the pivot).  If we don't take action, all those repeated values will be mapped to
     // the upper partition and we will enter a worst-case where we partition only one item each pass.  To prevent that, we will repartition using the same pivot, but
     // this time moving the equal values to the lower partition.  If this produces a better partition, we will use it.  If not, we must be processing a block of
     // ALL equal values, and we stop partitioning.  We thus process a block with many equals as follows: scan & partition to strip elements lower than the equals; scan, rescan, & partition to
     // strip elements higher than the equals; scan & rescan to detect the all-equals case & stop partitioning
     DQ(r-l, cstk=2*cstk+(v0[i]<=pivot);)  // this time equality moves to the lower side (cstk is already 0 to start)
     // There MUST have been at least 1 equal value, because the pivot was the median of three and yet nothing compared low the first time; one value at least must compare equal.
     if(!(cstk&(cstk+1))) {
      // There are still no exchanges.  Find the partition sizes.
      UI4 xchgx04; CTLZI(cstk,xchgx04); xchgx0=xchgx04; xchgx1=xchgx0+1;  // low partition always ends right below the high - no middle partition
      if(xchgx1!=r-l)goto finmedxchg;  // there are no exchanges, but we have to process both partitions (one of which might be empty)
      r=l; continue;  // all equal -- abort this partition and go to the next one
     }
     // ...falling through if there are exchanges after a rescan
    } else {
     // The lower partition is not empty.  Set the partition pointer above the lowest 1-bit
     UI4 xchgx04; CTLZI(cstk,xchgx04); xchgx0=xchgx04; xchgx1=xchgx0+1;  // low partition always ends right below the high - no middle partition
     goto finmedxchg;
    }
    // ...falling through if there are exchanges after a rescan
   }

   // There is at least 1 exchange.  Do them.  We swap the highest 1-bit with the lowest 0-bit, and complement both bits.  When the pointers cross, they are left after the cross pointing to the
   // end of the opposite partition.  So, we use xchgx1 for the left side, xchgx0 for the right side.
   while(1){
    // find the exchange points.  This is the dependency loop (actually the CTTZI is not part of the dependency).  The cstk^= could be replaced with cstk&=(1<<xchgx0)-1 if that would
    // generate a BZHI instruction.  After we get the exchange points we complement the bits of the exchange.  This guarantees that the number of set bits is invariant.
    // Therefore, when we terminate xchgx0 will always be the end of the low side and xchgx1 that of the high side
    UI4 xchgx04; CTLZI(cstk,xchgx04); xchgx0=xchgx04; xchgx1=CTTZI(~cstk); cstk|=cstk+1; cstk^=(I)1<<xchgx0;  // get indexes of the swaps
    if(xchgx0<xchgx1)break;  // terminate when the swap would be retrograde
    SORTQTYPE temp=v0[xchgx0]; v0[xchgx0]=v0[xchgx1]; v0[xchgx1]=temp;  // do the exchange
   }

finmedxchg:  // exchanges if any are done, and xchgx0/xchgx1 are set
   // After all the exchanges the exchange pointers are set to finish the partitions
   // bias the exchange points back to the range in the original input
   xchgx0+=l; xchgx1+=l;
  }else{
   // large partition.  Use two comparison stacks, refreshing them when they go empty
   // initialize comparison stacks to empty.  Bits go into the MSB of the comparison stack and are shifted down.  The in pointer is the index of the next value to be compared going in
   // lsb gives the bit # of the lowest valid value in the stack
   UI cstk0=0, cstk1=0; I cstklsb0=BW, cstklsb1=BW;
   // initialize comparison input pointers (input->next value to compare).  Prebias them by BW so that the output pointer is in0+cstklsb0  or  in1-cstklsb0
   I in0=l-BW; I in1=r-1+BW;  // the last position is notionally the swapped-out pivot
   xchgx0=0; xchgx1=r;  // remember the last successful exchange, so that we ignore any step past it
   // loop till partitioning completed
   // At this point we know that if cstk? is 0, cstklsb? has been set to BW.  Also, at least one cstk? is 0
   while(1){
    // add to each comparison stack.  Use as much as possible, but not much past halfway between comparison output pointers,
    // and never past the opposite output pointer.  This keeps us in the batch and avoids looking for swaps that have already been passed up.
    do{I midpoint=(in0+cstklsb0+in1-cstklsb1)>>1;  // halfway between output pointers
     // If we were unable to add to either of the empty stacks, we are done.  The empty stack(s) will point past the end of the block, but the length of the partition will be 0
     I ncmp0=cstklsb0; ncmp0=ncmp0>in1-cstklsb1-(in0+BW)+1?in1-cstklsb1-(in0+BW)+1:ncmp0;  // Don't move input pointer0 beyond output pointer 1.
     ncmp0=ncmp0>midpoint-in0+8?midpoint-in0+8:ncmp0; ncmp0=ncmp0<0?0:ncmp0; if(!(cstk0|ncmp0))goto partdone;  // Don't move too far past midpoint (8 here); never less than 0; if no swaps at all, we're through
     I ncmp1=cstklsb1; ncmp1=ncmp1>(in1-BW)-(in0+cstklsb0)+1?(in1-BW)-(in0+cstklsb0)+1:ncmp1;
     ncmp1=ncmp1>in1-midpoint+8?in1-midpoint+8:ncmp1; ncmp1=ncmp1<0?0:ncmp1; if(!(cstk1|ncmp1))goto partdone;
     // look for swappable values and stack them.  At end, note the position of the first swappable.  If there are none, advance LSB to end of word to leave maximum space
#if SORTQCOND
     // the LSB is the earliest bit in the compare order.  For side 0 it is the lowest address, for side 1 the highest.
     // Therefore chunks of side 1 must be reversed
     if(ncmp0){  // it is possible that there is nothing to do (if one side didn't move much).  In that case, leave everything set
      __m256i endmask;
      endmask = _mm256_loadu_si256((__m256i*)(validitymask+((-ncmp0)&(NPAR-1))));  /* mask for 00=1111, 01=1000, 10=1100, 11=1110 */
      cstk0>>=ncmp0;  // put the preexisting bits below the space where new bits will go.  If this shifts too much, OK because reg is 0 then
      I bittofill=BW-ncmp0;  // get the running index of where we will put new bits
      SORTQTYPE *vv=v+in0+BW;  // pointer to current batch, starting at 
      // fill 4-bit sections up to the last, which will be 1-4 bits
      while(bittofill<BW-NPAR){cstk0=cstk0+((I)_mm256_movemask_pd(SORTQCASTTOPD(SORTQCMP256(SORTQULOAD((SORTQULOADTYPE)vv),pivot256 SORTQCMPTYPE)))<<bittofill); vv+=NPAR; bittofill+=NPAR;}
      cstk0=cstk0+((I)_mm256_movemask_pd(SORTQCASTTOPD(SORTQCMP256(SORTQMASKLOAD(vv,endmask),pivot256 SORTQCMPTYPE)))<<bittofill);

      // advance input pointer over the comparisons we made; get index of lowest 1-bit (handling the case of none specially)
      in0+=ncmp0; cstklsb0=CTTZI(cstk0);cstklsb0=(cstk0==0)?BW:cstklsb0;
     }
     if(ncmp1){  // it is possible that there is nothing to do (if one side didn't move much).  In that case, leave everything set
      __m256i endmask;
      endmask = _mm256_loadu_si256((__m256i*)(validitymask+5+((ncmp1-1)&(NPAR-1))));  /* mask for 00=1111, 01=0001, 10=0011, 11=0111 */
      cstk1>>=ncmp1;  // put the preexisting bits below the space where new bits will go.  If this shifts too much, OK because reg is 0 then
      I bittofill=BW-ncmp1;  // get the running index of where we will put new bits
      SORTQTYPE *vv=v+in1-BW-(NPAR-1);  // pointer to beginning of the 4-word section ending at in1 
      // fill 4-bit sections up to the last, which will be 1-4 bits
      // if AVX2, use permute4x rather than the 2 insts here
      while(bittofill<BW-NPAR){cstk1=cstk1+((I)_mm256_movemask_pd(_mm256_permute_pd(_mm256_permute2f128_pd(SORTQCASTTOPD(SORTQCMP256(pivot256,SORTQULOAD((SORTQULOADTYPE)vv) SORTQCMPTYPE)),SORTQCASTTOPD(pivot256),0x01),0x5))<<bittofill); vv-=NPAR; bittofill+=NPAR;}
      cstk1=cstk1+((I)_mm256_movemask_pd(_mm256_permute_pd(_mm256_permute2f128_pd(SORTQCASTTOPD(SORTQCMP256(pivot256,SORTQMASKLOAD(vv,endmask) SORTQCMPTYPE)),SORTQCASTTOPD(pivot256),0x01),0x5))<<bittofill);

      // advance input pointer over the comparisons we made; get index of lowest 1-bit (handling the case of none specially)
      in1-=ncmp1; cstklsb1=CTTZI(cstk1);cstklsb1=(cstk1==0)?BW:cstklsb1;
     }
#else
     UI newstkbit=1LL<<(BW-1);
     in0+=ncmp0; DP(ncmp0, UI newbit=v[i+in0+BW]>pivot?newstkbit:0; cstk0=(cstk0>>1)+newbit;) cstklsb0=CTTZI(cstk0);cstklsb0=(cstk0==0)?BW:cstklsb0;
     in1-=ncmp1; DQ(ncmp1, UI newbit=v[i+in1-BW+1]<pivot?newstkbit:0; cstk1=(cstk1>>1)+newbit;) cstklsb1=CTTZI(cstk1);cstklsb1=(cstk1==0)?BW:cstklsb1;
#endif
    }while((cstk0==0)||(cstk1==0));
    // process the comparison stack until one of the stacks is empty.  Perform exchanges
    while(1){
     if(in0+cstklsb0>=in1-cstklsb1)goto partdone;  // if pointers have crossed, we're through
     xchgx0=in0+cstklsb0; xchgx1=in1-cstklsb1;  // remember the successful exchange, so that we don't move a pointer past it
     SORTQTYPE temp=v[xchgx0]; v[xchgx0]=v[xchgx1]; v[xchgx1]=temp;
     // Remove the bit we processed, and update the lsb for the next swap.  If there is no swap here, advance LSB to end of word to free up stack space
     if(cstk0&=cstk0-1)cstklsb0=CTTZI(cstk0); else{cstklsb0=BW; break;}  // We hope the processor predicts these branches to fail always, so that we will get one misbranch to exit the loop
     if(cstk1&=cstk1-1)cstklsb1=CTTZI(cstk1); else{cstklsb1=BW; goto testcstk1;}
    }
    // Here when cstk0 ended: we have not updated cstk1 yet
    cstk1&=cstk1-1; cstklsb1=CTTZI(cstk1); cstklsb1=cstk1==0?BW:cstklsb1;
 testcstk1:;
   }
   // At this point we know that indexes BELOW= in0+BW-cstklsb0 are <= the pivot, and those ABOVE= in1-(BW-cstklsb1)  are >= the pivot.  In other words, stklsb gives the location of the exchange that
   // was not made because those values were already in position.  Recursion will be to points including those values.  Any points between those values (exclusive) must be equal to the pivot
 partdone:
   // back up the pointers to the last successful exchange if any, since that exchange is known to have stored a value for that side
   xchgx0=MAX(xchgx0,in1-cstklsb1); xchgx1=MIN(xchgx1,in0+cstklsb0);  // Now xchgx0 is the end of the left side, and xchgx1 of the right
  }
  // partitioning is complete

  // exchange the end of the array with the start of the right side
  v[r]=v[xchgx1]; v[xchgx1]=pivot;

  // push stack for the larger partition; modify batch pointers for the smaller
  // recursions are l..xchgx0 and xchgx1+1..r    the +1 to step over the pivot from this pass
  ++xchgx1;  // the upper partition starts AFTER the pivot
  I lenl=xchgx0-l, lenr=r-xchgx1, l0=l, r0=r;
  // make l,r the smaller partition and l0,r0 the larger;  then stack l0,r0
  l0=lenl>lenr?l0:xchgx1; l=lenl>lenr?xchgx1:l; r0=lenl>lenr?xchgx0:r0; r=lenl>lenr?r:xchgx0; 
  stack[stackp][0]=l0; stack[stackp][1]=r0; ++stackp;
 }
}
#undef SORTQCOND
#undef SORTQSCOPE
#undef SORTQNAME
#undef SORTQTYPE
#undef SORTQSET256
#undef SORTQTYPE256
#undef SORTQCASTTOPD
#undef SORTQAND256
#undef SORTQCMP256
#undef SORTQCMPTYPE 
#undef SORTQMASKLOAD
#undef SORTQULOAD
#undef SORTQULOADTYPE
