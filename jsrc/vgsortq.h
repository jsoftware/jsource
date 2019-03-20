// define SORTQNAME to be the function name, SORTQTYPE to be the type of items sorted
// v->first value, n=# values
static void SORTQNAME(SORTQTYPE *v, I n){
 // loop till tail-recursion finished
 I l=0, r=n-1; I stackp=0; I stack[32][2];
 while(1){
  // the batch is short, sort it by fixed comparison; then pick up the next batch
  // we write this out in situ rather than calling a subroutine because there's not much register pressure
  // here and the compiler will not need to save/restore registers.  This doesn't matter for the floating-point version but it does for integer.
  while(r-l<=4){SORTQTYPE a,b,c,d,e;UI f;
   SORTQTYPE *vv = v+l;  // advance to the block to be reordered 
   switch(r-l){   // this is 1 LESS than the length of the block
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
   case 4:
    // read em in
    a=vv[0]; b=vv[1]; c=vv[2]; d=vv[3]; e=vv[4];
    // compare each value against each other value, and assemble the bits into f in order of 0-4
    // Then read the ordering value from the table.  This read may miss in cache but we don't care too much, since there is no dependency on the result of the read
    f=orderfromcomp5[((a>b)<<9) + ((a>c)<<8) + ((a>d)<<7) + ((a>e)<<6) + ((b>c)<<5) + ((b>d)<<4) + ((b>e)<<3) + ((c>d)<<2) + ((c>e)<<1) + (d>e)];
    // Store each value in its correct spot  Code to minimize the # instructions held waiting for the fetch to complete
    vv[f&7]=a; vv[(f>>3)&7]=b; vv[(f>>6)&7]=c; vv[(f>>9)&7]=d; vv[f>>12]=e;
    break;
   case -1: case 0:;
   }
   // that batch is sorted; move to the next
   if(--stackp<0)R;  // back up stack; if we're finishing the last call, we're through
   l=stack[stackp][0]; r=stack[stackp][1];  // resume the next 
  }
  // long batch: partition it and recur
  // choose the pivot as the median of first/last/middle, and exchange it with the end of the array
  SORTQTYPE pivot;
  {I pivotcomp = 0xc6>>(4*(v[l]>v[(l+r)>>1]) + 2*(v[l]>v[r]) + (v[(l+r)>>1]>v[r]));  // l>m, l>r, m>r 000 lmr  001 lrm  010 -  011 rlm  100 mlr  101 -  110 mrl  111 rml
      // encode l as 00 r as 11 m as 01/10, the sequence is mr-ll-rm 01 11 10 00 00 01 11 10  0 1 1 0 0 0 1 1 0 (LE) -> 011000110  0xc6
   I pivotx=((pivotcomp&1?r:l)+(pivotcomp&2?r:l))>>1; pivot=v[pivotx]; v[pivotx]=v[r];  // pick the median pivot, swap it (notionally) with the last 
  }
  // initialize comparison stacks to empty.  Bits go into the MSB of the comparison stack and are shifted down.  The in pointer is the index of the next value to be compared going in
  // lsb gives the bit # of the lowest valid value in the stack
  UI cstk0=0, cstk1=0; I cstklsb0=BW, cstklsb1=BW;
  // initialize comparison input pointers (input->next value to compare).  Prebias them by BW so that the output pointer is in0+cstklsb0  or  in1-cstklsb0
  I in0=l-BW; I in1=r-1+BW;  // the last position is notionally the swapped-out pivot
  I xchgx0=0; I xchgx1=r;  // remember the last successful exchange, so that we ignore any step past it
  // loop till partitioning completed
  // At this point we know that if cstk? is 0, cstklsb? has been set to BW.  Also, at least one cstk? is 0
  while(1){
   // add to each comparison stack.  Use as much as possible, but not much past halfway between comparison output pointers,
   // and never past the opposite output pointer.  This keeps us in the batch and avoids looking for swaps that have already been passed up.
   do{I midpoint=(in0+cstklsb0+in1-cstklsb1)>>1; UI newstkbit=1LL<<(BW-1);  // halfway between output pointers
    // If we were unable to add to either of the empty stacks, we are done.  The empty stack(s) will point past the end of the block, but the length of the partition will be 0
    I ncmp0=cstklsb0; ncmp0=ncmp0>in1-cstklsb1-(in0+BW)+1?in1-cstklsb1-(in0+BW)+1:ncmp0;  // Don't move input pointer0 beyond output pointer 1.
    ncmp0=ncmp0>midpoint-in0+8?midpoint-in0+8:ncmp0; ncmp0=ncmp0<0?0:ncmp0; if(!(cstk0|ncmp0))goto partdone;  // Don't move too far past midpoint (8 here); never less than 0; if no swaps at all, we're through
    I ncmp1=cstklsb1; ncmp1=ncmp1>(in1-BW)-(in0+cstklsb0)+1?(in1-BW)-(in0+cstklsb0)+1:ncmp1;
    ncmp1=ncmp1>in1-midpoint+8?in1-midpoint+8:ncmp1; ncmp1=ncmp1<0?0:ncmp1; if(!(cstk1|ncmp1))goto partdone;
    // look for swappable values and stack them.  At end, note the position of the first swappable.  If there are none, advance LSB to end of word to leave maximum space
    in0+=ncmp0; DP(ncmp0, UI newbit=v[i+in0+BW]>pivot?newstkbit:0; cstk0=(cstk0>>1)+newbit;) cstklsb0=CTTZI(cstk0);cstklsb0=(cstk0==0)?BW:cstklsb0;
    in1-=ncmp1; DQ(ncmp1, UI newbit=v[i+in1-BW+1]<pivot?newstkbit:0; cstk1=(cstk1>>1)+newbit;) cstklsb1=CTTZI(cstk1);cstklsb1=(cstk1==0)?BW:cstklsb1;
   }while((cstk0==0)||(cstk1==0));
// obsolete    if((cstklsb0|cstklsb1)&BW)goto partdone;
   // process the comparison stack until one of the stacks is empty.  Perform exchanges
   while(1){
    if(in0+cstklsb0>=in1-cstklsb1)goto partdone;  // if pointers have crossed, we're through
    xchgx0=in0+cstklsb0; xchgx1=in1-cstklsb1;  // remember the successful exchange, so that we don't move a pointer past it
    SORTQTYPE temp=v[xchgx0]; v[xchgx0]=v[xchgx1]; v[xchgx1]=temp;
// obsolete     cstk0&=cstk0-1; cstk1&=cstk1-1; // remove the bits that were processed
// obsolete     cstklsb0=CTTZI(cstk0); cstklsb1=CTTZI(cstk1);   // advance the pointer to the next swap; garbage if zero, but that's corrected immediately below
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
  // exchange the end of the array with the start of the right side
  v[r]=v[xchgx1]; v[xchgx1]=pivot;

  // push stack for the larger partition; modify batch pointers for the smaller
  // recursions are l..in1-(BW-cstklsb1) and in0-(BW-cstklsb0)+1..r    the +1 to step over the pivot from this pass
  if(xchgx0-l > r-xchgx1){stack[stackp][0]=l; stack[stackp][1]=xchgx0; l=xchgx1+1;
  }else{stack[stackp][0]=xchgx1+1; stack[stackp][1]=r; r=xchgx0;
  }
  ++stackp;
 }
}
#undef SORTQNAME
#undef SORTQTYPE
