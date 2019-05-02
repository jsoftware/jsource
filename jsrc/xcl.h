
F2(jtpartitiongrades){
#if C_AVX&&SY_64
 RZ(a&&w);
 ASSERT(AT(a)&B01,EVDOMAIN); ASSERT(AT(w)&(B01|INT),EVDOMAIN); if(!(AT(w)&INT))RZ(w=cvt(INT,w));
 ASSERT(AR(a)==1,EVRANK); ASSERT(AR(w),EVRANK);
 I ngrades; PROD(ngrades,AR(w)-1,AS(w));
 B *vb=BAV(a); I lr=bsum(AN(a),vb); I ll=AS(w)[AR(w)-1]-lr;
 A zl; GATV(zl,INT,ngrades*ll,AR(w),AS(w)); AS(zl)[AR(w)-1]=ll;
 A zr; GATV(zr,INT,ngrades*lr,AR(w),AS(w)); AS(zr)[AR(w)-1]=lr;
 I *vl=IAV(zl), *vr=IAV(zr), *vw=IAV(w); 
 DQ(AN(w), I g=*vw++; I *vz=vr; I r=(I)vb[g]<<LGSZI; vz=r?vz:vl; vr=(I*)((C*)vr+r); r^=SZI; vl=(I*)((C*)vl+r); *vz=g;)
 RETF(link(zl,zr));
#else
 ASSERT(0,EVDOMAIN);
#endif
}

// return (table of score,maxx);best row
#define NPARS 8
#define LGNPARS 3
F1(jtfindsplit){PROLOG(300); // lambda,gh,grades
#if C_AVX&&SY_64
 ASSERT(AT(w)&BOX,EVDOMAIN); ASSERT(AR(w)==1,EVRANK); ASSERT(AN(w)==3,EVLENGTH);
 A alambda=AAV(w)[0]; ASSERT(AR(alambda)==0,EVRANK); if(!(AT(alambda)&FL))RZ(alambda=cvt(FL,alambda));
 A agh=AAV(w)[1]; ASSERT(AR(agh)==2,EVRANK); ASSERT(AS(agh)[0]==2,EVLENGTH); if(!(AT(agh)&FL))RZ(agh=cvt(FL,agh));
 A agrades=AAV(w)[2]; ASSERT(AR(agrades)==2,EVRANK); if(!(AT(agrades)&INT))RZ(agrades=cvt(INT,agrades)); I *grades=IAV(agrades); DO(AS(agrades)[1], ASSERT((UI)grades[i]<(UI)AS(agh)[1],EVINDEX);)
 if(AN(agrades)==0)RETF(link(mtm,mtv));
 A aghs; GATV0(aghs,FL,AS(agh)[1],1);
 A aghsum; GATV0(aghsum,FL,(AS(agrades)[1]+(NPARS-1))&(-NPARS),1);
 A az; GATV0(az,FL, AS(agrades)[0]*2, 2); AS(az)[0]= AS(agrades)[0]; AS(az)[1]=2;
 A az2; GAT0(az2,FL,2,1);
 _mm256_zeroupper(VOIDARG);
 NAN0;
 {
  __m256i spreadmask = _mm256_set_epi32(3,3,2,2,1,1,0,0);
  DS *ghs=(DS*)voidAV(aghs);
  D *g=DAV(agh); D *h=g+AS(agh)[1];
  __m256d gd; __m256d hd; __m128 gs; __m128 hs;
  DQ((AS(agh)[1]-1)>>LGNPAR,
   gd=_mm256_loadu_pd(g); g+=NPAR; gs=_mm256_cvtpd_ps(gd);
   hd=_mm256_loadu_pd(h); h+=NPAR; hs=_mm256_cvtpd_ps(hd);
   _mm256_storeu_ps(ghs,_mm256_blend_ps(
     _mm256_permutevar8x32_ps(_mm256_castps128_ps256(gs),spreadmask),
     _mm256_permutevar8x32_ps(_mm256_castps128_ps256(hs),spreadmask),
     0xaa)); ghs+=2*NPAR;
  )
 __m256i endmask = _mm256_loadu_si256((__m256i*)(jt->validitymask+((-AS(agh)[1])&(NPAR-1))));
  gd=_mm256_maskload_pd(g,endmask); g+=NPAR; gs=_mm256_cvtpd_ps(gd);
  hd=_mm256_maskload_pd(h,endmask); h+=NPAR; hs=_mm256_cvtpd_ps(hd);
  _mm256_maskstore_ps(ghs,endmask,_mm256_blend_ps(
    _mm256_permutevar8x32_ps(_mm256_castps128_ps256(gs),spreadmask),
    _mm256_permutevar8x32_ps(_mm256_castps128_ps256(hs),spreadmask),
    0xaa));
 }
 __m256 bestfombesti1ng=_mm256_set_ps(0.0,0.0,0.0,0.0,0.0,1.0,0.0,-1e20f);
 __m256 lambda;
 {
  DS minlambda=MAX((DS)DAV(alambda)[0],0.000001f);
  lambda=_mm256_set_ps(minlambda,minlambda,minlambda,minlambda,minlambda,minlambda,minlambda,minlambda);
 }
 I ng=0; for(ng=0;ng<AS(agrades)[0];++ng){
  __m256 totalg, totalh;
  // +/\ 
  {
   __m256 acc0, acc1, gh0246, gh1357; acc0=_mm256_set_ps(0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0); acc1=acc0;
   D *gh=DAV(aghs);  DS *ghsum=(DS*)DAV(aghsum);
   DQ((AS(agrades)[1]-1)>>LGNPARS,
    acc0=_mm256_add_ps(acc0,_mm256_castpd_ps(_mm256_broadcast_sd(&gh[*grades++]))); gh0246=_mm256_add_ps(acc0,acc1);
    acc1=_mm256_add_ps(acc1,_mm256_castpd_ps(_mm256_broadcast_sd(&gh[*grades++]))); gh1357=_mm256_add_ps(acc0,acc1);
    acc0=_mm256_add_ps(acc0,_mm256_castpd_ps(_mm256_broadcast_sd(&gh[*grades++]))); gh0246=_mm256_blend_ps(gh0246,_mm256_add_ps(acc0,acc1),0xc);
    acc1=_mm256_add_ps(acc1,_mm256_castpd_ps(_mm256_broadcast_sd(&gh[*grades++]))); gh1357=_mm256_blend_ps(gh1357,_mm256_add_ps(acc0,acc1),0xc);
    acc0=_mm256_add_ps(acc0,_mm256_castpd_ps(_mm256_broadcast_sd(&gh[*grades++]))); gh0246=_mm256_blend_ps(gh0246,_mm256_add_ps(acc0,acc1),0x30);
    acc1=_mm256_add_ps(acc1,_mm256_castpd_ps(_mm256_broadcast_sd(&gh[*grades++]))); gh1357=_mm256_blend_ps(gh1357,_mm256_add_ps(acc0,acc1),0x30);
    acc0=_mm256_add_ps(acc0,_mm256_castpd_ps(_mm256_broadcast_sd(&gh[*grades++]))); gh0246=_mm256_blend_ps(gh0246,_mm256_add_ps(acc0,acc1),0xc0);
    acc1=_mm256_add_ps(acc1,_mm256_castpd_ps(_mm256_broadcast_sd(&gh[*grades++]))); gh1357=_mm256_blend_ps(gh1357,_mm256_add_ps(acc0,acc1),0xc0);
    _mm256_storeu_ps(ghsum, _mm256_blend_ps(gh0246,_mm256_permute_ps(gh1357,0xa0),0xaa)); ghsum+=NPARS;
    _mm256_storeu_ps(ghsum, _mm256_blend_ps(gh1357,_mm256_permute_ps(gh0246,0xf5),0x55)); ghsum+=NPARS;
   )
   I n=(AS(agrades)[1]-1)&(NPARS-1);
   acc0=_mm256_add_ps(acc0,_mm256_castpd_ps(_mm256_broadcast_sd(&gh[*grades++]))); gh0246=_mm256_add_ps(acc0,acc1);
   if(n>0){acc1=_mm256_add_ps(acc1,_mm256_castpd_ps(_mm256_broadcast_sd(&gh[*grades++]))); gh1357=_mm256_add_ps(acc0,acc1);
   if(n>1){acc0=_mm256_add_ps(acc0,_mm256_castpd_ps(_mm256_broadcast_sd(&gh[*grades++]))); gh0246=_mm256_blend_ps(gh0246,_mm256_add_ps(acc0,acc1),0xfc);
   if(n>2){acc1=_mm256_add_ps(acc1,_mm256_castpd_ps(_mm256_broadcast_sd(&gh[*grades++]))); gh1357=_mm256_blend_ps(gh1357,_mm256_add_ps(acc0,acc1),0xfc);
   if(n>3){acc0=_mm256_add_ps(acc0,_mm256_castpd_ps(_mm256_broadcast_sd(&gh[*grades++]))); gh0246=_mm256_blend_ps(gh0246,_mm256_add_ps(acc0,acc1),0xf0);
   if(n>4){acc1=_mm256_add_ps(acc1,_mm256_castpd_ps(_mm256_broadcast_sd(&gh[*grades++]))); gh1357=_mm256_blend_ps(gh1357,_mm256_add_ps(acc0,acc1),0xf0);
   if(n>5){acc0=_mm256_add_ps(acc0,_mm256_castpd_ps(_mm256_broadcast_sd(&gh[*grades++]))); gh0246=_mm256_blend_ps(gh0246,_mm256_add_ps(acc0,acc1),0xc0);
   if(n>6){acc1=_mm256_add_ps(acc1,_mm256_castpd_ps(_mm256_broadcast_sd(&gh[*grades++]))); gh1357=_mm256_blend_ps(gh1357,_mm256_add_ps(acc0,acc1),0xc0);
   }else gh1357=_mm256_blend_ps(gh1357,gh0246,0xc0);}}else gh1357=_mm256_blend_ps(gh1357,gh0246,0xc0);}}else gh1357=_mm256_blend_ps(gh1357,gh0246,0xc0);}}else gh1357=_mm256_blend_ps(gh1357,gh0246,0xc0);
   _mm256_storeu_ps(ghsum, _mm256_blend_ps(gh0246,_mm256_permute_ps(gh1357,0xa0),0xaa)); ghsum+=NPARS;
   _mm256_storeu_ps(ghsum, _mm256_blend_ps(gh1357,_mm256_permute_ps(gh0246,0xf5),0x55));

   gh0246=_mm256_add_ps(acc0,acc1);
   totalg=_mm256_blend_ps(gh0246,_mm256_permute_ps(gh0246,0xa0),0xaa);
   totalh=_mm256_blend_ps(gh0246,_mm256_permute_ps(gh0246,0xf5),0x55);
  }
  // the rest
  {
   __m256 maxfom=_mm256_set_ps(-1e20f,-1e20f,-1e20f,-1e20f,-1e20f,-1e20f,-1e20f,-1e20f);
   __m256 maxx=_mm256_set_ps(-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0);
   __m256 loopx=_mm256_set_ps(7.0,6.0,5.0,4.0,3.0,2.0,1.0,0.0);
   __m256 loopinc=_mm256_set_ps(8.0,8.0,8.0,8.0,8.0,8.0,8.0,8.0);
   __m256 fom,fomge;
   DS *ghsum=(DS*)DAV(aghsum);
   I nd=AN(aghsum)>>LGNPARS; while(1){
    __m256 gl, gr, hl, hr;
    gl=_mm256_loadu_ps(ghsum); ghsum+=NPARS;
    gr=_mm256_sub_ps(totalg,gl);
    gl=_mm256_mul_ps(gl,gl); gr=_mm256_mul_ps(gr,gr);
    hl=_mm256_loadu_ps(ghsum); ghsum+=NPARS;
    hr=_mm256_add_ps(_mm256_sub_ps(totalh,hl),lambda); hl=_mm256_add_ps(hl,lambda);
    fom=_mm256_add_ps(_mm256_div_ps(gl,hl),_mm256_div_ps(gr,hr));
    fomge=_mm256_cmp_ps(fom,maxfom,_CMP_GE_OQ);
    if(--nd<=0)break;
    maxfom=_mm256_blendv_ps(maxfom,fom,fomge);
    maxx=_mm256_blendv_ps(maxx,loopx,fomge);
    loopx=_mm256_add_ps(loopx,loopinc);
   }
   fomge=_mm256_and_ps(fomge,_mm256_loadu_ps(&((DS*)(jt->validitymask))[8-((AS(agrades)[1]-1)&(NPARS-1))]));
   maxfom=_mm256_blendv_ps(maxfom,fom,fomge);
   maxx=_mm256_blendv_ps(maxx,loopx,fomge);

   __m256 fomfromupper=_mm256_permute_ps(maxfom,0xf5);
   __m256 xfromupper=_mm256_permute_ps(maxx,0xf5);
   __m256 upperok=_mm256_or_ps(_mm256_cmp_ps(fomfromupper,maxfom,_CMP_GT_OQ),_mm256_and_ps(_mm256_cmp_ps(fomfromupper,maxfom,_CMP_EQ_OQ),_mm256_cmp_ps(xfromupper,maxx,_CMP_GT_OQ)));
   maxfom=_mm256_blendv_ps(maxfom,fomfromupper,upperok);
   maxx=_mm256_blendv_ps(maxx,xfromupper,upperok);
   fomfromupper=_mm256_permute_ps(maxfom,0xaa);
   xfromupper=_mm256_permute_ps(maxx,0xaa);
   upperok=_mm256_or_ps(_mm256_cmp_ps(fomfromupper,maxfom,_CMP_GT_OQ),_mm256_and_ps(_mm256_cmp_ps(fomfromupper,maxfom,_CMP_EQ_OQ),_mm256_cmp_ps(xfromupper,maxx,_CMP_GT_OQ)));
   maxfom=_mm256_blendv_ps(maxfom,fomfromupper,upperok);
   maxx=_mm256_blendv_ps(maxx,xfromupper,upperok);
   fomfromupper=_mm256_permute2f128_ps(maxfom,maxfom,0x33);
   xfromupper=_mm256_permute2f128_ps(maxx,maxx,0x33);
   upperok=_mm256_or_ps(_mm256_cmp_ps(fomfromupper,maxfom,_CMP_GT_OQ),_mm256_and_ps(_mm256_cmp_ps(fomfromupper,maxfom,_CMP_EQ_OQ),_mm256_cmp_ps(xfromupper,maxx,_CMP_GT_OQ)));
   maxfom=_mm256_blendv_ps(maxfom,fomfromupper,upperok);
   maxx=_mm256_blendv_ps(maxx,xfromupper,upperok);
   maxfom=_mm256_sub_ps(maxfom,_mm256_permute2f128_ps(fom,_mm256_permute_ps(fom,0xff),0x33));
   _mm_storeu_pd(&DAV(az)[2*ng],_mm_cvtps_pd(_mm256_castps256_ps128(_mm256_blend_ps(maxfom,_mm256_permute_ps(maxx,0x00),0x2))));

   maxfom=_mm256_blend_ps(maxfom,_mm256_permute_ps(bestfombesti1ng,0xff),0x2);
   bestfombesti1ng=_mm256_blendv_ps(bestfombesti1ng,maxfom,(_mm256_blend_ps(_mm256_xor_ps(maxfom,maxfom),_mm256_permute_ps(_mm256_cmp_ps(maxfom,bestfombesti1ng,_CMP_GE_OQ),0x00),3)));
   bestfombesti1ng=_mm256_blend_ps(bestfombesti1ng,_mm256_add_ps(bestfombesti1ng,_mm256_permute_ps(bestfombesti1ng,0xaa)),0x8);

  }
  _mm_storeu_pd(&DAV(az2)[0],_mm_cvtps_pd(_mm256_castps256_ps128(bestfombesti1ng)));
 }
 NAN1;
 az=link(az,az2);
 EPILOG(az);
#else
 ASSERT(0,EVDOMAIN);
#endif
}
