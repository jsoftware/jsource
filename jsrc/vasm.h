/* Copyright 1990-2005, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Assembly Routines for Integer + * - with Overflow                */

/* C routines for platforms without asm support */
#if 0  // obsolete 

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
#define TYMESS(n,z,x)      {DPMULDDECLS I s=1; x+=n; z+=n; DQ(n, --x; --z; DPMULD(*x,s,s, R EWOV;) *z=s;)}

#endif
