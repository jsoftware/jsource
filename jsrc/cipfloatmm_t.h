// cache-blocking code
#ifndef OPHEIGHT
#define OPHEIGHT 2  // height of outer-product block
#define OPWIDTH 4  // width of outer-product block
#define CACHEWIDTH 64  // width of resident cache block
#define CACHEHEIGHT 16  // height of resident cache block
#endif
// Floating-point matrix multiply, hived off to a subroutine to get fresh register allocation
// *zv=*av * *wv, with *cv being a cache-aligned region big enough to hold CACHEWIDTH*CACHEHEIGHT floats
// a is shape mxp, w is shape pxn
static void FUNCNAME (D* av,D* wv,D* zv,I m,I n,I p){D c[(CACHEHEIGHT+1)*CACHEWIDTH + (CACHEHEIGHT+1)*OPHEIGHT*OPWIDTH + 2*CACHELINESIZE/sizeof(D)];
 // m is # 1-cells of a
 // n is # atoms in an item of w (and result)
 // p is number of inner-product muladds (length of a row of a, and # items of w)
 // point to cache-aligned areas we will use for staging the inner-product info
 D *cvw = (D*)(((I)&c+(CACHELINESIZE-1))&-CACHELINESIZE);  // place where cache-blocks of w are staged
 D *cva = (D*)(((I)cvw+(CACHEHEIGHT+1)*CACHEWIDTH*sizeof(D)+(CACHELINESIZE-1))&-CACHELINESIZE);   // place where expanded rows of a are staged
 // zero the result area
 memset(zv,C0,m*n*sizeof(D));
 // process each 64-float vertical stripe of w, producing the corresponding columns of z
 D* w0base = wv; D* z0base = zv; I w0rem = n;
 for(;w0rem>0;w0rem-=CACHEWIDTH,w0base+=CACHEWIDTH,z0base+=CACHEWIDTH){
  // process each 16x64 section of w, adding each result to the columns of z
  D* a1base=av; D* w1base=w0base; D* z1base=z0base; I w1rem=p;
  for(;w1rem>0;w1rem-=CACHEHEIGHT,a1base+=CACHEHEIGHT,w1base+=CACHEHEIGHT*n){D* RESTRICT cvx;D* w1next=w1base;I i;
   // read the 16x64 section of w into the cache area (8KB, 2 ways of cache), with prefetch of rows
   for(i=MIN(CACHEHEIGHT,w1rem),cvx=cvw;i;--i){I j;
    D* RESTRICT w1x=w1next; w1next+=n;  // save start of current input row, point to next row...
    _mm_prefetch((C*)w1next,_MM_HINT_T0);   // ... and prefetch next row
    for(j=0;j<MIN(CACHEWIDTH,w0rem);++j)*cvx++=*w1x++; for(;j<CACHEWIDTH;++j)*cvx++=0.0;   // move current row during prefetch
   }
   // Because of loop unrolling, we fetch and multiply one extra value in each cache column.  We make sure those values are 0 to avoid NaN errors
   for(i=0;i<MIN(CACHEWIDTH,w0rem);++i)*cvx++=0.0; 
   // the nx16 vertical strip of a will be multiplied by the 16x64 section of w and accumulated into z
   // process each 2x16 section of a against the 16x64 cache block
   D *a2base0=a1base; D* w2base=w1base; I a2rem=m; D* z2base=z1base; D* c2base=cvw;
   for(;a2rem>0;a2rem-=OPHEIGHT,a2base0+=OPHEIGHT*p,z2base+=OPHEIGHT*n){
    static D missingrow[CACHEHEIGHT]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
    // Prepare for the 2x16 block of a
    // If the second row of a is off the end of the data, we mustn't fetch it - switch the pointer to a row of 1s so it won't give NaN error on multiplying by infinity
    D *a2base1 = (a2rem>1)?a2base0+p:missingrow;
#if ARCHAVX
    // Make 4 sequential copies of each float in a, to support the parallel multiply for the outer product.  Interleave the values
    // from the two rows, putting them in the correct order for the multiply.  We fetch each row in order, to make sure we get fast page mode
    // for the row
    {__m256d *cvx;D *a0x;I j;
     for(a0x=a2base0,i=0;i<OPHEIGHT;++i,a0x=a2base1){
      for(cvx=(__m256d*)cva+i,j=MIN(CACHEHEIGHT,w1rem);j;--j,++a0x,cvx+=OPHEIGHT){
       *cvx = _mm256_set_pd(*a0x,*a0x,*a0x,*a0x);
      }
      // Because of loop unrolling, we fetch and multiply one extra outer product.  Make sure it is harmless, to avoid NaN errors
      *cvx = _mm256_set_pd(0.0,0.0,0.0,0.0);
     }
    }
#endif
    // process each 16x4 section of cache, accumulating into z
    I a3rem=MIN(w0rem,CACHEWIDTH);
    D* RESTRICT z3base=z2base; D* c3base=c2base;
    for(;a3rem>0;a3rem-=OPWIDTH,c3base+=OPWIDTH,z3base+=OPWIDTH){
     // initialize accumulator with the z values accumulated so far.
#if ARCHAVX
     __m256d z00,z01,z10,z11,z20,z21; static I valmask[8]={0, 0,0,0,-1,-1,-1,-1};
     z21 = z20 = z11 = z10 = _mm256_set_pd(0.0,0.0,0.0,0.0);
     // We have to use masked load at the edges of the array, to make sure we don't fetch from undefined memory.  Fill anything not loaded with 0
     if(a3rem>3){z00 = _mm256_loadu_pd(z3base);if(a2rem>1)z01 = _mm256_loadu_pd(z3base+n); else z01=z21;   // In the main area, do normal (unaligned) loads
     }else{z01 = z00 = z20;
           z00 = _mm256_maskload_pd(z3base,_mm256_set_epi64x(valmask[a3rem],valmask[a3rem+1],valmask[a3rem+2],valmask[a3rem+3]));
           I vx= (a2rem>1)?a3rem:0; z01 = _mm256_maskload_pd(z3base+n,_mm256_set_epi64x(valmask[vx],valmask[vx+1],valmask[vx+2],valmask[vx+3]));
     }
#else
     D z00,z01,z02,z03,z10,z11,z12,z13;
     z00=z3base[0];
     if(a3rem>3){z01=z3base[1],z02=z3base[2],z03=z3base[3]; if(a2rem>1)z10=z3base[n],z11=z3base[n+1],z12=z3base[n+2],z13=z3base[n+3];
     }else{if(a3rem>1){z01=z3base[1];if(a3rem>2)z02=z3base[2];}; if(a2rem>1){z10=z3base[n];if(a3rem>1)z11=z3base[n+1];if(a3rem>2)z12=z3base[n+2];}}
#endif
     // process outer product of each 2x1 section on each 1x4 section of cache

     // Before starting the last set of 16 outer products, issue prefetches for the next lines of a and z
     // We hope that the hardware prefetcher will get the remainder of each 2x16 block before it is needed
     if(a3rem<=4){PREFETCH((C*)(z2base+(OPHEIGHT*n)),_MM_HINT_T0);PREFETCH((C*)(z2base+(OPHEIGHT*n)+n),_MM_HINT_T0);
                  PREFETCH((C*)(a2base0+(OPHEIGHT*p)),_MM_HINT_T0);PREFETCH((C*)(a2base0+(OPHEIGHT*p))+p,_MM_HINT_T0);}

     I a4rem=MIN(w1rem,CACHEHEIGHT);
     D* RESTRICT c4base=c3base;
#if ARCHAVX  // This version if AVX instruction set is available.
     __m256d *a4base0=(__m256d *)cva;   // Can't put RESTRICT on this - the loop to init *cva gets optimized away
      // read the 2x1 a values and the 1x4 cache values
      // form outer product, add to accumulator
// This loop is hand-unrolled because the compiler doesn't seem to do it.  Unroll 3 times - needed on dual ALUs
     __m256d cval0 = _mm256_load_pd(c4base);  // read from cache

     __m256d cval1 = _mm256_load_pd(c4base+CACHEWIDTH);
     __m256d aval00 = _mm256_mul_pd(cval0,a4base0[0]);  // multiply by a
     __m256d aval01 = _mm256_mul_pd(cval0,a4base0[1]);

     do{
      __m256d cval2 = _mm256_load_pd(c4base+2*CACHEWIDTH);
      __m256d aval10 = _mm256_mul_pd(cval1,a4base0[2]);
      __m256d aval11 = _mm256_mul_pd(cval1,a4base0[3]);
      z00 = _mm256_add_pd(z00 , aval00);    // accumulate into z
      z01 = _mm256_add_pd(z01 , aval01);
      if(--a4rem<=0)break;

      cval0 = _mm256_load_pd(c4base+3*CACHEWIDTH);
      __m256d aval20 = _mm256_mul_pd(cval2,a4base0[4]);
      __m256d aval21 = _mm256_mul_pd(cval2,a4base0[5]);
      z10 = _mm256_add_pd(z10 , aval10);
      z11 = _mm256_add_pd(z11 , aval11);
      if(--a4rem<=0)break;

      cval1 = _mm256_load_pd(c4base+4*CACHEWIDTH);
      aval00 = _mm256_mul_pd(cval0,a4base0[6]);
      aval01 = _mm256_mul_pd(cval0,a4base0[7]);
      z20 = _mm256_add_pd(z20 , aval20);
      z21 = _mm256_add_pd(z21 , aval21);
      if(--a4rem<=0)break;

      a4base0 += OPHEIGHT*3;  // OPWIDTH is implied by __m256d type
      c4base+=CACHEWIDTH*3;
     }while(1);

     // Collect the sums of products
     z10 = _mm256_add_pd(z10,z20);z11 = _mm256_add_pd(z11,z21); z00 = _mm256_add_pd(z00,z10); z01 = _mm256_add_pd(z01,z11);
     // Store accumulator into z.  Don't store outside the array
     if(a3rem>3){_mm256_storeu_pd(z3base,z00);if(a2rem>1)_mm256_storeu_pd(z3base+n,z01);
     }else{_mm256_maskstore_pd(z3base,_mm256_set_epi64x(valmask[a3rem],valmask[a3rem+1],valmask[a3rem+2],valmask[a3rem+3]),z00);
           if(a2rem>1)_mm256_maskstore_pd(z3base+n,_mm256_set_epi64x(valmask[a3rem],valmask[a3rem+1],valmask[a3rem+2],valmask[a3rem+3]),z01);
     }

#else   // If no AVX instructions
     D* RESTRICT a4base0=a2base0; D* RESTRICT a4base1=a2base1; 
     do{
      // read the 2x1 a values and the 1x4 cache values
      // form outer product, add to accumulator
      D t0,t1,a0,a1,c0,c1,c2,c3;
      a0=a4base0[0]; t0=c4base[0]; 
      a1=a4base1[0]; c0=c4base[0];
      t0 *= a0; c0 *= a1;
      t1=c4base[1]; c1=c4base[1]; t1 *= a0; c1 *= a1;
      z00 += t0; t0 = c4base[2]; c2 = c4base[2]; c3 = c4base[3];
      z10 += c0; z01 += t1; z11 += c1;
      t0 *= a0; c2 *= a1; a0 *= c3; c3 *= a1;
      z02 += t0; z12 += c2; z03 += a0; z13 += c3;
      a4base0++,a4base1++;
      c4base+=CACHEWIDTH;
     }while(--a4rem>0);

     // Store accumulator into z.  Don't store outside the array
     z3base[0]=z00;
     if(a3rem>3){z3base[1]=z01,z3base[2]=z02,z3base[3]=z03; if(a2rem>1)z3base[n]=z10,z3base[n+1]=z11,z3base[n+2]=z12,z3base[n+3]=z13;
     }else{if(a3rem>1){z3base[1]=z01;if(a3rem>2)z3base[2]=z02;}; if(a2rem>1){z3base[n]=z10;if(a3rem>1){z3base[n+1]=z11;if(a3rem>2)z3base[n+2]=z12;}}}
#endif
    }
   }
  }
 }
}

