/* Copyright 1990-2009, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* cr.c templates                                                          */

/* template 0 used by the rank operator general cases (monad and dyad)     */
/* requires:                                                               */
/*  VALENCE 1 or 2                                                         */

#if TEMPLATE==0
#if VALENCE==1
#define RCALL   CALL1(f1,yw,fs)
#define RDIRECT (wt&DIRECT)
#define RAC     (1==AC(yw))
#define RFLAG   (!(AFLAG(w)&AFNJA+AFSMM+AFREL))
#define RARG    {if(1<AC(yw))NEWYW; MOVEYW;}
#define RARGX   {if(1<AC(yw)){RZ(yw=ca(yw)); vv=CAV(yw);}}
#else
#define RCALL   CALL2(f2,ya,yw,fs)
#define RDIRECT (at&DIRECT&&wt&DIRECT)
#define RAC     (1==AC(ya)&&1==AC(yw))
#define RFLAG   (!(AFLAG(a)&AFNJA+AFSMM+AFREL)&&!(AFLAG(w)&AFNJA+AFSMM+AFREL))
#define RARG    {++jj; if(!b||jj==n){if(1<AC(ya))NEWYA; MOVEYA;}  \
                       if( b||jj==n){if(1<AC(yw))NEWYW; MOVEYW;} if(jj==n)jj=0;}
#define RARGX   {if(1<AC(ya)){RZ(ya=ca(ya)); uu=CAV(ya);}  \
                 if(1<AC(yw)){RZ(yw=ca(yw)); vv=CAV(yw);}}
#endif
{B cc=1;C*zv;I j=0,jj=0,old;
 if(mn){y0=y=RCALL; RZ(y);}
 else{I d;
  d=jt->db; jt->db=0; y=RCALL; jt->db=d;
  if(jt->jerr){if(jt->jerr==EVSTACK)RZ(y); y=zero; RESETERR;}
 } 
 yt=AT(y); yr=AR(y); ys=AS(y); yn=AN(y); k=yn*bp(yt);
 if(!mn||yt&DIRECT&&RFLAG){I zn;
  RARGX; RE(zn=mult(mn,yn));
  GA(z,yt,zn,p+yr,0L); ICPY(AS(z),s,p); ICPY(p+AS(z),ys,yr);
  if(mn){zv=CAV(z); MC(zv,AV(y),k);}
  old=jt->tbase+jt->ttop;
  for(j=1;j<mn;++j){
   RARG; 
   RZ(y=RCALL);
   if(yt!=AT(y)||yr!=AR(y)||yr&&ICMP(AS(y),ys,yr))break;
   MC(zv+=k,AV(y),k);
   if(cc&&RAC)tpop(old); else cc=0;
 }}
 if(j<mn){A q,*x,yz;
  jj=j%n;
  GA(yz,BOX,mn,p,s); x=AAV(yz);
  if(j){
   zv=CAV(z)-k;
   DO(j, GA(q,AT(y0),AN(y0),AR(y0),AS(y0)); MC(AV(q),zv+=k,k); *x++=q;);
  }
  *x++=y;
  DO(mn-j-1, RARG; RZ(y=RCALL); *x++=y;);
  z=ope(yz);
 }
 EPILOG(z);
}
#undef VALENCE
#undef RAC
#undef RARG
#undef RARGX
#undef RCALL
#undef RDIRECT
#undef RFLAG
#endif  /* TEMPLATE 0 */


#undef TEMPLATE