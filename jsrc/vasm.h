/* Copyright 1990-2005, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Assembly Routines for Integer + * - with Overflow                */

/* fvv  zv=.xv+yv     0<n */
/* fv1  zv=.xv+y      0<n */
/* f1v  zv=.x +yv     0<n */
/* frv  zv=.xv+zv     0<n */
/* fr   z =.+/  xv    1<n */
/* fp   zv=.+/\ xv    1<n */
/* fs   zv=.+/\.xv    1<n */

#if 0 //! #if !C_NA
#if SY_WIN32 && !SY_64 && !SY_WINCE
#define OVF
#define PLUSVV(m,z,x,y)                 \
{                                       \
__asm        mov  ecx,m                 \
__asm        mov  edi,z                 \
__asm        mov  esi,x                 \
__asm        mov  edx,y                 \
__asm        sub  edi,4                 \
__asm        sub  esi,4                 \
__asm        sub  edx,4                 \
__asm pvv20: mov  eax,[esi+ecx*4]       \
__asm        add  eax,[edx+ecx*4]       \
__asm        jo   pvv30                 \
__asm        mov  [edi+ecx*4],eax       \
__asm        loop pvv20                 \
__asm        jmp  pvv40                 \
__asm pvv30: mov  er,EWOV               \
__asm pvv40:                            \
}   

#define PLUS1V(n,z,u,y)                 \
{                                       \
__asm        mov  ecx,n                 \
__asm        mov  edi,z                 \
__asm        mov  edx,u                 \
__asm        mov  esi,y                 \
__asm        sub  edi,4                 \
__asm        sub  esi,4                 \
__asm p1v20: mov  eax,[esi+ecx*4]       \
__asm        add  eax,edx               \
__asm        jo   p1v30                 \
__asm        mov  [edi+ecx*4],eax       \
__asm        loop p1v20                 \
__asm        jmp  p1v40                 \
__asm p1v30: mov  er,EWOV               \
__asm p1v40:                            \
}

#define PLUSV1(n,z,x,v)                 \
{                                       \
__asm        mov  ecx,n                 \
__asm        mov  edi,z                 \
__asm        mov  esi,x                 \
__asm        mov  edx,v                 \
__asm        mov  esi,x                 \
__asm        sub  edi,4                 \
__asm        sub  esi,4                 \
__asm pv120: mov  eax,[esi+ecx*4]       \
__asm        add  eax,edx               \
__asm        jo   pv130                 \
__asm        mov  [edi+ecx*4],eax       \
__asm        loop pv120                 \
__asm        jmp  pv140                 \
__asm pv130: mov  er,EWOV               \
__asm pv140:                            \
}

#define PLUSRV(d,z,x)                   \
{                                       \
__asm        mov  ecx,d                 \
__asm        mov  edi,z                 \
__asm        mov  esi,x                 \
__asm        sub  edi,4                 \
__asm        sub  esi,4                 \
__asm prv20: mov  eax,[esi+ecx*4]       \
__asm        add  eax,[edi+ecx*4]       \
__asm        jo   prv30                 \
__asm        mov  [edi+ecx*4],eax       \
__asm        loop prv20                 \
__asm        jmp  prv40                 \
__asm prv30: mov  er,EWOV               \
__asm prv40:                            \
}

#define PLUSR(n,z,x)                    \
{                                       \
__asm        mov  ecx,n                 \
__asm        mov  edi,z                 \
__asm        mov  esi,x                 \
__asm        sub  esi,4                 \
__asm        xor  eax,eax               \
__asm  pr20: add  eax,[esi+ecx*4]       \
__asm        jo   pr30                  \
__asm        loop pr20                  \
__asm        mov  [edi],eax             \
__asm        jmp  pr40                  \
__asm  pr30: mov  er,EWOV               \
__asm  pr40:                            \
}

#define PLUSP(n,z,x)                    \
{                                       \
__asm        mov  ecx,n                 \
__asm        mov  edi,z                 \
__asm        mov  esi,x                 \
__asm        xor  edx,edx               \
__asm        xor  eax,eax               \
__asm        mov  ebx,ecx               \
__asm        sar  ecx,1                 \
__asm  pp20: add  eax,[esi+edx*4]       \
__asm        jo   pp30                  \
__asm        mov  [edi+edx*4],eax       \
__asm        inc  edx                   \
__asm        add  eax,[esi+edx*4]       \
__asm        jo   pp30                  \
__asm        mov  [edi+edx*4],eax       \
__asm        inc  edx                   \
__asm        loop pp20                  \
__asm        and  ebx,1                 \
__asm        jz   pp40                  \
__asm        add  eax,[esi+edx*4]       \
__asm        jo   pp30                  \
__asm        mov  [edi+edx*4],eax       \
__asm        jmp  pp40                  \
__asm  pp30: mov  er,EWOV               \
__asm  pp40:                            \
}

#define PLUSS(n,z,x)                    \
{                                       \
__asm        mov  ecx,n                 \
__asm        mov  edi,z                 \
__asm        mov  esi,x                 \
__asm        sub  esi,4                 \
__asm        sub  edi,4                 \
__asm        xor  eax,eax               \
__asm  ps20: add  eax,[esi+ecx*4]       \
__asm        jo   ps30                  \
__asm        mov  [edi+ecx*4],eax       \
__asm        loop ps20                  \
__asm        jmp  ps40                  \
__asm  ps30: mov  er,EWOV               \
__asm  ps40:                            \
}


#define MINUSVV(m,z,x,y)                \
{                                       \
__asm        mov  ecx,m                 \
__asm        mov  edi,z                 \
__asm        mov  esi,x                 \
__asm        mov  edx,y                 \
__asm        sub  edi,4                 \
__asm        sub  esi,4                 \
__asm        sub  edx,4                 \
__asm mvv20: mov  eax,[esi+ecx*4]       \
__asm        sub  eax,[edx+ecx*4]       \
__asm        jo   mvv30                 \
__asm        mov  [edi+ecx*4],eax       \
__asm        loop mvv20                 \
__asm        jmp  mvv40                 \
__asm mvv30: mov  er,EWOV               \
__asm mvv40:                            \
}

#define MINUS1V(n,z,u,y)                \
{                                       \
__asm        mov  ecx,n                 \
__asm        mov  edi,z                 \
__asm        sub  edi,4                 \
__asm        mov  edx,u                 \
__asm        mov  esi,y                 \
__asm        sub  esi,4                 \
__asm m1v20: mov  eax,edx               \
__asm        sub  eax,[esi+ecx*4]       \
__asm        jo   m1v30                 \
__asm        mov  [edi+ecx*4],eax       \
__asm        loop m1v20                 \
__asm        jmp  m1v40                 \
__asm m1v30: mov  er,EWOV               \
__asm m1v40:                            \
}

#define MINUSV1(n,z,x,v)                \
{                                       \
__asm        mov  ecx,n                 \
__asm        mov  edi,z                 \
__asm        mov  esi,x                 \
__asm        mov  edx,v                 \
__asm        sub  edi,4                 \
__asm        sub  esi,4                 \
__asm mv120: mov  eax,[esi+ecx*4]       \
__asm        sub  eax,edx               \
__asm        jo   mv130                 \
__asm        mov  [edi+ecx*4],eax       \
__asm        loop mv120                 \
__asm        jmp  mv140                 \
__asm mv130: mov  er,EWOV               \
__asm mv140:                            \
}

#define MINUSRV(d,z,x)                  \
{                                       \
__asm        mov  ecx,d                 \
__asm        mov  edi,z                 \
__asm        mov  esi,x                 \
__asm        sub  edi,4                 \
__asm        sub  esi,4                 \
__asm mrv20: mov  eax,[esi+ecx*4]       \
__asm        sub  eax,[edi+ecx*4]       \
__asm        jo   mrv30                 \
__asm        mov  [edi+ecx*4],eax       \
__asm        loop mrv20                 \
__asm        jmp  mrv40                 \
__asm mrv30: mov  er,EWOV               \
__asm mrv40:                            \
}

#define MINUSR(n,z,x)                   \
{                                       \
__asm        mov  ecx,n                 \
__asm        mov  edi,z                 \
__asm        mov  esi,x                 \
__asm        sub  esi,4                 \
__asm        xor  eax,eax               \
__asm  mr20: mov  edx,[esi+ecx*4]       \
__asm        sub  edx,eax               \
__asm        jo   mr30                  \
__asm        mov  eax,edx               \
__asm        loop mr20                  \
__asm        mov  [edi],eax             \
__asm        jmp  mr40                  \
__asm  mr30: mov  er,EWOV               \
__asm  mr40:                            \
}

#define MINUSP(n,z,x)                   \
{                                       \
__asm        mov  ecx,n                 \
__asm        mov  edi,z                 \
__asm        mov  esi,x                 \
__asm        xor  edx,edx               \
__asm        xor  eax,eax               \
__asm        mov  ebx,ecx               \
__asm        sar  ecx,1                 \
__asm  mp20: add  eax,[esi+edx*4]       \
__asm        jo   mp30                  \
__asm        mov  [edi+edx*4],eax       \
__asm        inc  edx                   \
__asm        sub  eax,[esi+edx*4]       \
__asm        jo   mp30                  \
__asm        mov  [edi+edx*4],eax       \
__asm        inc  edx                   \
__asm        loop mp20                  \
__asm        and  ebx,1                 \
__asm        jz   mp40                  \
__asm        add  eax,[esi+edx*4]       \
__asm        jo   mp30                  \
__asm        mov  [edi+edx*4],eax       \
__asm        jmp  mp40                  \
__asm  mp30: mov  er,EWOV               \
__asm  mp40:                            \
}

#define MINUSS(n,z,x)                   \
{                                       \
__asm        mov  ecx,n                 \
__asm        mov  edi,z                 \
__asm        mov  esi,x                 \
__asm        sub  esi,4                 \
__asm        sub  edi,4                 \
__asm        xor  eax,eax               \
__asm  ms20: mov  edx,[esi+ecx*4]       \
__asm        sub  edx,eax               \
__asm        jo   ms30                  \
__asm        mov  eax,edx               \
__asm        mov  [edi+ecx*4],eax       \
__asm        loop ms20                  \
__asm        jmp  ms40                  \
__asm  ms30: mov  er,EWOV               \
__asm  ms40:                            \
}


#define TYMESVV(m,z,x,y)                \
{                                       \
__asm        mov  ecx,m                 \
__asm        mov  edi,z                 \
__asm        mov  esi,x                 \
__asm        mov  edx,y                 \
__asm        sub  edi,4                 \
__asm        sub  esi,4                 \
__asm        sub  edx,4                 \
__asm tvv20: mov  eax,[esi+ecx*4]       \
__asm        imul eax,[edx+ecx*4]       \
__asm        jo   tvv30                 \
__asm        mov  [edi+ecx*4],eax       \
__asm        loop tvv20                 \
__asm        jmp  tvv40                 \
__asm tvv30: mov  er,EWOV               \
__asm tvv40:                            \
}

#define TYMES1V(n,z,u,y)                \
{                                       \
__asm        mov  ecx,n                 \
__asm        mov  edi,z                 \
__asm        mov  edx,u                 \
__asm        mov  esi,y                 \
__asm        sub  edi,4                 \
__asm        sub  esi,4                 \
__asm t1v20: mov  eax,[esi+ecx*4]       \
__asm        imul eax,edx               \
__asm        jo   t1v30                 \
__asm        mov  [edi+ecx*4],eax       \
__asm        loop t1v20                 \
__asm        jmp  t1v40                 \
__asm t1v30: mov  er,EWOV               \
__asm t1v40:                            \
}

#define TYMESV1(n,z,x,v)                \
{                                       \
__asm        mov  ecx,n                 \
__asm        mov  edi,z                 \
__asm        mov  esi,x                 \
__asm        mov  edx,v                 \
__asm        sub  edi,4                 \
__asm        sub  esi,4                 \
__asm tv120: mov  eax,[esi+ecx*4]       \
__asm        imul eax,edx               \
__asm        jo   tv130                 \
__asm        mov  [edi+ecx*4],eax       \
__asm        loop tv120                 \
__asm        jmp  tv140                 \
__asm tv130: mov  er,EWOV               \
__asm tv140:                            \
}

#define TYMESRV(d,z,x)                  \
{                                       \
__asm        mov  ecx,d                 \
__asm        mov  edi,z                 \
__asm        mov  esi,x                 \
__asm        sub  edi,4                 \
__asm        sub  esi,4                 \
__asm trv20: mov  eax,[esi+ecx*4]       \
__asm        imul eax,[edi+ecx*4]       \
__asm        jo   trv30                 \
__asm        mov  [edi+ecx*4],eax       \
__asm        loop trv20                 \
__asm        jmp  trv40                 \
__asm trv30: mov  er,EWOV               \
__asm trv40:                            \
}

#define TYMESR(n,z,x)                   \
{                                       \
__asm        mov  ecx,n                 \
__asm        mov  edi,z                 \
__asm        mov  esi,x                 \
__asm        sub  esi,4                 \
__asm        mov  eax,1                 \
__asm  tr20: imul eax,[esi+ecx*4]       \
__asm        jo   tr30                  \
__asm        loop tr20                  \
__asm        mov  [edi],eax             \
__asm        jmp  tr40                  \
__asm  tr30: mov  er,EWOV               \
__asm  tr40:                            \
}

#define TYMESP(n,z,x)                   \
{                                       \
__asm        mov  ecx,n                 \
__asm        mov  edi,z                 \
__asm        mov  esi,x                 \
__asm        xor  edx,edx               \
__asm        mov  eax,1                 \
__asm  tp20: imul eax,[esi+edx*4]       \
__asm        jo   tp30                  \
__asm        mov  [edi+edx*4],eax       \
__asm        inc  edx                   \
__asm        loop tp20                  \
__asm        jmp  tp40                  \
__asm  tp30: mov  er,EWOV               \
__asm  tp40:                            \
}

#define TYMESS(n,z,x)                   \
{                                       \
__asm        mov  ecx,n                 \
__asm        mov  edi,z                 \
__asm        mov  esi,x                 \
__asm        sub  esi,4                 \
__asm        sub  edi,4                 \
__asm        mov  eax,1                 \
__asm  ts20: imul eax,[esi+ecx*4]       \
__asm        jo   ts30                  \
__asm        mov  [edi+ecx*4],eax       \
__asm        loop ts20                  \
__asm        jmp  ts40                  \
__asm  ts30: mov  er,EWOV               \
__asm  ts40:                            \
}

#endif

#if SY_64 /* win64 and linux64 asm routines */
#define OVF

C asmplusvv(I,I*,I*,I*);
C asmminusvv(I,I*,I*,I*);
C asmtymesvv(I,I*,I*,I*);

C asmplus1v(I,I*,I,I*);
C asmminus1v(I,I*,I,I*);
C asmtymes1v(I,I*,I,I*);

C asmminusv1(I,I*,I*,I);

C asmplusr(I,I*,I*);
C asmminusr(I,I*,I*);
C asmtymesr(I,I*,I*);

C asmplusrv(I,I*,I*);
C asmminusrv(I,I*,I*);
C asmtymesrv(I,I*,I*);
 
C asmpluss(I,I*,I*);
C asmminuss(I,I*,I*);
C asmtymess(I,I*,I*);

C asmplusp(I,I*,I*);
C asmminusp(I,I*,I*);
C asmtymesp(I,I*,I*);

C asminnerprod(I,I*,I*,I*,I);
C asminnerprodx(I,I*,I,I*);

#define  PLUSVV(m,z,x,y) er=asmplusvv(m,z,x,y)
#define MINUSVV(m,z,x,y) er=asmminusvv(m,z,x,y)
#define TYMESVV(m,z,x,y) er=asmtymesvv(m,z,x,y)

#define  PLUSRV(d,z,x)   er=asmplusrv(d,z,x)
#define MINUSRV(d,z,x)   er=asmminusrv(d,z,x)
#define TYMESRV(d,z,x)   er=asmtymesrv(d,z,x)

#define  PLUS1V(n,z,u,y) er=asmplus1v(n,z,u,y)
#define MINUS1V(n,z,u,y) er=asmminus1v(n,z,u,y)
#define TYMES1V(n,z,u,y) er=asmtymes1v(n,z,u,y)

#define  PLUSV1(n,z,x,v) PLUS1V(n,z,v,x)
#define MINUSV1(n,z,x,v) er=asmminusv1(n,z,x,v)
#define TYMESV1(n,z,x,v) TYMES1V(n,z,v,x)

#define  PLUSR(n,z,x)    er=asmplusr(n,z,x) 
#define MINUSR(n,z,x)    er=asmminusr(n,z,x) 
#define TYMESR(n,z,x)    er=asmtymesr(n,z,x) 

#define  PLUSS(n,z,x)    er=asmpluss(n,z,x) 
#define MINUSS(n,z,x)    er=asmminuss(n,z,x) 
#define TYMESS(n,z,x)    er=asmtymess(n,z,x) 

#define  PLUSP(n,z,x)    er=asmplusp(n,z,x) 
#define MINUSP(n,z,x)    er=asmminusp(n,z,x) 
#define TYMESP(n,z,x)    er=asmtymesp(n,z,x) 

#endif

#endif

/* C routines for platforms without asm support */
#ifndef OVF

#if SY_64
#define DI LD
#else
#define DI D
#endif

#define PLUSVV(n,z,x,y)   {I u,v,w; DQ(n, u=*x; v=*y; w=~u; u+=v; *z=u; ++x; ++y; ++z; w^=v; v^=u; if(XANDY(w,v)<0)R EWOV;)}
#define MINUSVV(n,z,x,y)   {I u,v,w; DQ(n, u=*x; v=*y; w=u-v; *z=w; ++x; ++y; ++z; v^=u; u^=w; if(XANDY(u,v)<0)R EWOV;)}
#define TYMESVV(n,z,x,y)   {DPMULDECLS  DQ(n, DPMUL(*x,*y,z, R EWOV;); z++; x++; y++;)}


// overflow if (~ssign^vsign)&&(resultsign^vsign), i. e. input signs equal and result changes sign
#define PLUSP(n,z,x)      {I s=0,u,v;   DQ(n, u=~s; v=*x; s+=v; u^=v; v^=s; x++; *z++=s; if(XANDY(u,v)<0)R EWOV;);}
#define MINUSP(n,z,x)      {I s=0,u,v,p=0;   DQ(n, u=~s; v=*x^p; s+=v-p; p = ~p; u^=v; v^=s; *z++=s; x++; if(XANDY(u,v)<0)R EWOV;);}
#define TYMESP(n,z,x)      {DPMULDDECLS I s=1; DQ(n, DPMULD(*x,s,s,R EWOV;) *z++=s; x++;)}

// accumulate the total in double integer precision; at the end decide which to use.  vs accumulates the number of negative numbers added: each one
// should be sign-extended and added to the high part, but we wait till the end to combine them, to save pipeline stalls
#define PLUSR(n,z,x)      {UI s=0,v,vs=0,h=0;   DQ(n, v=*x; vs += REPSGN((I)v); SPDPADD(v, s, h) x++;); *z=(I)s; if(h+vs+REPSGN((UI)s)){if(m==1){er=EWOV1; *(D*)z=(I)(h+vs)*(-2*(D)IMIN)+s;}else er=EWOV;}}
// accumulate in double integer precision; p alternates between 0 (add) and _1 (subtract); subtract by adding one's-complement + 1.  As with addition,
// we accumulate high-order sign-extensions to add and add them at the end.  Also, we don't add the 1 in the complement+1 each time: we just
// calculate the number of subtracts there will be, and add them all at once (at initialization of z)
#define MINUSR(n,z,x)      {UI s=n>>1,v,vs=0,h=0,p=0;  DQ(n, v=*x^p; vs += REPSGN((I)v); SPDPADD(v, s, h) x++; p=~p;); *z=(I)s; if(h+vs+REPSGN((UI)s)){if(m==1){er=EWOV1; *(D*)z=(I)(h+vs)*(-2*(D)IMIN)+s;}else er=EWOV;}}
#define TYMESR(n,z,x)      {I l=1; DPMULDDECLS DQ(n, DPMULD(l,*x,l,R EWOV;); x++;) *z=l;}

#define PLUSRV(n,z,x)     {I u,v,w; DQ(n, u=*x; v=*z; w=~u; u+=v; *z=u; ++x; ++z; w^=v; v^=u; if(XANDY(w,v)<0)R EWOV;)}
#define MINUSRV(n,z,x)     {I u,v,w; DQ(n, u=*x; v=*z; w=u-v; *z=w; ++x; ++z; v^=u; u^=w; if(XANDY(u,v)<0)R EWOV;)}
#define TYMESRV(n,z,x)     {DPMULDECLS DQ(n, DPMUL(*x,*z,z,R EWOV;); x++; z++;);}

#define PLUSS(n,z,x)      {I s=0,u,v; x+=n; z+=n; DQ(n, u=~s; v=*--x; s+=v; u^=v; v^=s; *--z=s; if(XANDY(u,v)<0)R EWOV;);}
// overflow if (~ssign^vsign)&&(resultsign^vsign), i. e. input signs differ and result has different sign from minuend
#define MINUSS(n,z,x)     {I s=0,u,v; x+=n; z+=n; DQ(n, u=s; v=*--x; s=v-u; u^=v; v^=s; *--z=s; if(XANDY(u,v)<0)R EWOV;);}
#define TYMESS(n,z,x)      {DPMULDDECLS I s=1; x+=n; z+=n; DQ(n, --x; --z; DPMULD(*x,s,s, R EWOV; *z=s;))}

#endif
