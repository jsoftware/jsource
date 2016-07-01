/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Words: Numeric Input Conversion                                         */

#include "j.h"

#if (SYS & SYS_LINUX)
#include <stdlib.h>
#endif

#define NUMH(f)  B f(J jt,I n,C*s,void*vv)

/* numd    floating point number (double)                      */
/* numj    complex number                                      */
/* numx    extended precision integer                          */
/* nume    extended precision floating point number (not used) */
/* numr    rational number                                     */
/* numq    nume or numr                                        */
/* numbpx  3b12 or 3p12 or 3x12 number                         */
/*                                                             */
/* numb    subfunction of numbpx                               */
/*                                                             */
/* converts a single number and assigns into result pointer    */
/* returns 0 if error, 1 if ok                                 */

static NUMH(jtnumd){C c,*t;D*v,x,y;
 RZ(n);
 v=(D*)vv;
 if('-'==*s&&3>n)
  if(1==n){*v=inf; R 1;}
  else{
   c=*(1+s);
   if('-'==c){*v=infm; R 1;}
   else if('.'==c){*v=jnan; R 1;}
  }
 x=strtod(s,&t);
 if(t<s+n-1&&'r'==*t){y=strtod(1+t,&t); x=y?x/y:0<x?inf:0>x?infm:0;}
 R t>=s+n?(*v=x,1):0;
}

static NUMH(jtnumj){C*t,*ta;D x,y;Z*v;
 v=(Z*)vv;
 if(t=memchr(s,'j',n))ta=0; else t=ta=memchr(s,'a',n);
 RZ(numd(t?t-s:n,s,&x));
 if(t){t+=ta?2:1; RZ(numd(n+s-t,t,&y));} else y=0;
 if(ta){C c;
  c=*(1+ta);
  RZ(0<=x&&(c=='d'||c=='r'));
  if(c=='d')y*=PI/180; if(y<=-P2||P2<=y)y-=P2*jfloor(y/P2); if(0>y)y+=P2;
  v->re=y==0.5*PI||y==1.5*PI?0:x*cos(y); v->im=y==PI?0:x*sin(y);
 }else{v->re=x; v->im=y;}
 R 1;
}

static NUMH(jtnumi){B neg;C*t;I j;static C*dig="0123456789";
 if(neg='-'==*s){++s; --n; RZ(n);}
 for(;*s=='0'&&n>1;--n,++s);  // skip leading zeros, as long as there is more than one character
 RZ(19>=n);   // 2^63 is 9223372036854775808.  So a 20-digit input must overflow, and the most a
  // 19-digit number can be is a little way into the negative; so testing for negative will be a valid test for overflow
 j=0; DO(n, RZ(t=memchr(dig,*s++,10L)); j=10*j+(t-dig););
 RZ(0<=j||neg&&j==IMIN);  // overflow if negative AND not the case of -2^63, whichs shows as IMIN with a negative flag
 *(I*)vv=0>j||!neg?j:-j;   // if j<0, it must be IMIN, keep it neg; otherwise change sign if neg
 R 1;
}     /* called only if SY_64 */

static NUMH(jtnumx){A y;B b,c;C d,*t;I j,k,m,*yv;X*v;static C*dig="0123456789";
 v=(X*)vv;
 d=*(s+n-1); b='-'==*s; c='x'==d||'r'==d; s+=b;
 if('-'==d){RZ(2>=n); RZ(*v=vci(1==n?XPINF:XNINF)); R 1;}
 n-=b+c; RZ(m=(n+XBASEN-1)/XBASEN); k=n-XBASEN*(m-1);
 GA(y,INT,m,1,0); yv=m+AV(y);
 DO(m, j=0; DO(k, RZ(t=memchr(dig,*s++,10L)); j=10*j+(t-dig);); *--yv=b?-j:j; k=XBASEN;);
 RZ(*v=yv[m-1]?y:xstd(y));
 R 1;
}

static X jtx10(J jt,I e){A z;I c,m,r,*zv;
 m=1+e/XBASEN; r=e%XBASEN;
 GA(z,INT,m,1,0); zv=AV(z);
 DO(m-1, *zv++=0;);
 c=1; DO(r, c*=10;); *zv=c;
 R z;
}     /* 10^e as a rational number */

static NUMH(jtnume){C*t,*td,*te;I e,ne,nf,ni;Q f,i,*v,x,y;
 v=(Q*)vv;
 nf=0; i.d=iv1; f.d=iv1;
 if(te=memchr(s,'e',n)){ne=n-(te-s)-1; e=strtoI(1+te,&t,10);  RZ(!*t&&10>ne);}
 if(td=memchr(s,'.',n)){nf=te?(te-td)-1:n-(td-s)-1; if(nf)RZ(numx(nf,td+1,&f.n));}
 ni=td?td-s:te?te-s:n; RZ(numx(ni,s,&i.n));
 x=i;
 if(nf){y.n=iv1; y.d=x10(nf); RE(x='-'==*s?qminus(x,qtymes(f,y)):qplus(x,qtymes(f,y)));}
 if(te){if(0>e){y.n=iv1; y.d=x10(-e);}else{y.n=x10(e); y.d=iv1;} RE(x=qtymes(x,y));}
 *v=x;
 R 1;
}

static NUMH(jtnumr){C c,*t;I m,p,q;Q*v;
 v=(Q*)vv;
 m=(t=memchr(s,'r',n))?t-s:n; RZ(numx(m,s,&v->n)); v->d=iv1;
 if(t){
  c=s[n-1]; RZ('r'!=c&&'x'!=c);
  RZ(numx(n-m-1,s+m+1,&v->d));
  p=*AV(v->n); q=*AV(v->d); 
  RZ(p!=XPINF&&p!=XNINF||q!=XPINF&&q!=XNINF);
  RE(*v=qstd(*v));
 }
 R 1;
}

static NUMH(jtnumq){B b=0;C c,*t;
 t=s; DO(n, c=*t++; if(c=='e'||c=='.'){b=1; break;});
 R b?nume(n,s,vv):numr(n,s,vv);
}

static Z zpi={PI,0};

static B jtnumb(J jt,I n,C*s,Z*v,Z b){A c,d,y;I k;
  static C dig[]="0123456789abcdefghijklmnopqrstuvwxyz";I m=strlen(dig);
 if(!n){*v=zeroZ; R 1;}
 RZ(d=indexof(str(m,dig),str(n,s)));
 RZ(all0(eps(sc(m),d)));
 k=sizeof(Z);
 GA(c,CMPX,1,0,0); MC(AV(c),&b,k); RZ(y=base2(c,d)); MC(v,AV(y),k);
 R 1;
}

static NUMH(jtnumbpx){B ne,ze;C*t,*u;I k,m;Z b,p,q,*v,x,y;
 v=(Z*)vv;
 if(t=memchr(s,'b',n)){
  RZ(numbpx(t-s,s,&b));
  ++t; if(ne='-'==*t)++t;
  m=k=n+s-t; if(u=memchr(t,'.',m))k=u-t;
  RZ(ne||m>(1&&u));
  RZ(numb(k,t,&p,b));
  if(u){
   k=m-(1+k);
   if(ze=!(b.re||b.im))b.re=1;
   RZ(numb(k,1+u,&q,b));
   if(ze){if(q.re)p.re=inf;} else{DO(k,q=zdiv(q,b);); p=zplus(p,q);}
  }
  *v=p; if(ne){v->re=-v->re; v->im=-v->im;}
  R 1;
 }
 if(t=memchr(s,'p',n))u=0; else t=u=memchr(s,'x',n);
 if(!t)R numj(n,s,v);
 RZ(numj(t-s,s,&x)); ++t; RZ(numj(n+s-t,t,&y));
 if(u)*v=ztymes(x,zexp(y)); else *v=ztymes(x,zpow(zpi,y));
 R 1;
}

// indicate types found in the input string.
// If x is not set here, numbers ending in x are ill-formed
// Example: '1j1 1x' sets j but not x, so 1x is ill-formed
// Example:  '16b4 1x' similarly, and '4.0 1x' similarly
/* (n,s) string containing the vector constant                */
/* j:  1 iff contains 1j2 or 1ad2 or 1ar2                     */
/* b:  1 iff has 1b1a or 1p2 or 1x2 (note: must handle 1j3b4) */
/* x:  1 iff contains 123x                                    */
/* q:  1 iff contains 3r4                                     */
/* ii: 1 iff integer (but not x)                              */

static void jtnumcase(J jt,I n,C*s,B*b,B*j,B*x,B*q,B*ii){B e;C c;
 *x=*q=*ii=0;
 // if the string contains 'a' or 'j', it must be complex
 *j=memchr(s,'j',n)||memchr(s,'a',n);
 // if it contains 'b' or 'p', that becomes the type regardless of others
 // (types incompatible with that raise errors later)
 *b=memchr(s,'b',n)||memchr(s,'p',n);
 if(!*j&&!*b){
#if SY_64
  *ii=1;
#endif
  // if not j or b type, scan again. x indicates 1x2 or 23x.  Set both
  if(memchr(s,'x',n)){*b=*x=1; *ii=0;}
  // if string contains r, it's rational (since not ar)
  if(memchr(s,'r',n)){*q=1;    *ii=0;}
  // if no x or r found, exit as float/integer
  if(!*x&&!*q&&!*ii)R;
  // If any . or e found, or 'x' not at the end, treat as float/int, with all other modes cleared
  // Thus, 4. 1r3 produces float, while 4 1r3 produces rational
  DO(n, c=s[i]; e=!s[1+i]; if(c=='.'||c=='e'||c=='x'&&!e){*x=*q=*ii=0; R;});
}}

A jtconnum(J jt,I n,C*s){PROLOG;A y,z;B b,(*f)(),ii,j,p=1,q,x;C c,*v;I d=0,e,k,m,t,*yv;
 if(1==n)                {if(k=s[0]-'0',0<=k&&k<=9)R num[ k]; else R ainf;}
 else if(2==n&&CSIGN==*s){if(k=s[1]-'0',0<=k&&k<=9)R num[-k];}
 RZ(y=str(1+n,s)); s=v=CAV(y); s[n]=0;
 GA(y,INT,1+n,1,0); yv=AV(y);
 DO(n, c=*v; *v++=c=c==CSIGN?'-':c==CTAB||c==' '?C0:c; b=C0==c; if(p!=b)yv[d++]=i; p=b;);
 if(d%2)yv[d++]=n; m=d/2;
 numcase(n,s,&b,&j,&x,&q,&ii);
 f=q?jtnumq:x?jtnumx:b||j?jtnumbpx:ii?jtnumi:jtnumd; 
 t=q?RAT   :x?XNUM  :b||j?CMPX    :ii?INT   :FL;     k=bp(t);
 GA(z,t,m,1!=m,0); v=CAV(z);
 if(ii){
  DO(m, d=i+i; e=yv[d]; if(!numi(yv[1+d]-e,e+s,v)){ii=0; break;} v+=k;);
  if(!ii){t=FL; f=jtnumd; GA(z,t,m,1!=m,0); v=CAV(z);}
 }
 if(!ii)DO(m, d=i+i; e=yv[d]; ASSERT(f(jt,yv[1+d]-e,e+s,v),EVILNUM); v+=k;);
 if(t&FL+CMPX)RZ(z=cvt0(z));
 EPILOG(bcvt(0,z));
}


#define EXEC2F(f,f1,t,T) \
 A f(J jt,A a,A w,I n,I m,I c){A z;B b;C d,*u,*uu,*x,*y;I i,j,k,mc,r;T a0,*zv;  \
  i=0; mc=m*c; u=CAV(w); y=u+n; j=c; uu=u+AN(w); if(mc)*(uu-1)=' ';         \
  r=AR(w)-(1==c); r=MAX(0,r);                                               \
  GA(z,t,mc,r,AS(w)); if(1<r&&1!=c)*(AS(z)+r-1)=c; zv=(T*)AV(z);            \
  RZ(a=cvt(t,a)); a0=*(T*)AV(a);                                            \
  while(i<mc){                                                              \
   while(u<uu&&C0==*u)++u;                                                  \
   while(u>=y){while(i<j)zv[i++]=a0; j+=c; y+=n; if(i==mc)R z;}             \
   x=strchr(u,C0); if(x<uu)k=x-u; else{*(uu-1)=C0; k=uu-1-u;}               \
   b=','==u[0]||','==u[k-1];                                                \
   x=u; DO(k, d=u[i]; if(','!=d)*x++=d==CSIGN?'-':d;); *x=C0;               \
   if(b||!f1(x-u,u,i+zv))zv[i]=a0;                                          \
   ++i; u+=1+k;                                                             \
  }                                                                         \
  R z;                                                                      \
 }

static EXEC2F(jtexec2x,numx,  XNUM,X)  /* note: modifies argument w */
static EXEC2F(jtexec2q,numq,  RAT, Q)
static EXEC2F(jtexec2z,numbpx,CMPX,Z)

// Try to convert a numeric field to integer.  Result is the integer, and *out is the
// output pointer after the integer - this will be either the \0 at the end of the field, or
// ==in to indicate failure.  Result is undefined on failure.
// This routine must recognize all valid ints,  We accept at most one sign, followed by any number
// of digits or commas, followed optionally by a decimal point, followed optionally by a
// string of 0s.  So, 123,456.00 is recognized as an integer.  If the input overflows an
// integer, failure.  For compatibility with non-integer code, we allow commas anywhere in
// the number except the beginning or end.
// This routine assumes 64-bit signed integers.
I strtoint(C* in, C** out) {
 I res = 0; // init result
 I neg, dig;  // negative flag, digit value
 *out = in;  // assume failure
 if(neg = '-'==*in){   // starting with - could be negative sign.  Remember the sign
  if('.'==*++in){if(in[1]==C0)R 0;}   // if not followed by . it is negative sign.  Followed by . is
    // _. if . is last character.  Step to next character, and fail if _.
 } else if('+'==*in)++in;  // skip + sign
 if(','==*in || C0==*in)R 0;  // if number begins with , or is empty (after sign), reject it.
   // could be just + (error) or just - (inf)
 for(;;++in) {   // Read integer part:
  dig = (I)*in - (I)'0';
  if(dig>= 0 && dig <= 9){  // numeric digit.  Accept it and check for overflow
   if(res==IMIN)R 0;  // if previous digit overflowed to IMIN, this must fail
   if(res >= 1+IMAX/10) R 0;  // fail if this will overflow for sure.
   res = res * 10 + dig; // accept the digit.  This may overflow, but that's not fatal yet if it overflows to IMIN
   if(res<0 && res!=IMIN)R 0;  // We allow IMIN to continue on, representing IMAX+1
   continue;
  }
  if(*in==C0 || *in=='.')break;  // end-of-field or end-of-integer part: exit
  if(','==*in)continue;  // skip comma
  R 0;  // unknown character encountered, fail
 }
 // We have read the integer part.  Check for .[0000]
 if('.'==*in) {   // is '.'?
  for(++in;*in!=C0;++in) {  // Step over '.', then loop till end-of-field or error
   if(*in=='0' || *in==',')continue;   // skip over trailing 0 or comma
   R 0;   // non-zero encountered in field; this can't be an int
  }
 }
 if(','==in[-1])R 0;  // fail if last character of the field is comma.  *in here must be \0
 // It passed as an int.  Return it, unless it overflows on sign
 // If the value is IMIN, it has already had the sign switched; otherwise apply the sign
 if(res==IMIN){if(!neg)R 0;}   // -2^63 is OK, but not +2^63
 else{if(neg)res=-res;}
 *out=in;   // Finally, signal success
 R res;  // Return the int value
}

// Normal numeric-to-character conversion.
// a is the default, w is the character buffer to convert, m is the number of rows of characters,
// n is the length of each row, c is the number of values in each row
static A jtexec2r(J jt,A a,A w,I n,I m,I c){A z;B b,e;C d,*u,*uu,*v,*x,*y;D a0,a1,*zv;I k,j,mc,r;
 B tryingint; // set if we have to attempt to convert to int before float, if ints can hold
   // higher precision than float
B valueisint; // set if the value we are processing is really an int
  // Calculate total # result values; set input scan pointer u; set &next row of input y; set end-of-input pointer uu
 // set end-of-result-row counter j
 k=0; mc=m*c; u=CAV(w); y=u+n; j=c; uu=u+AN(w);
 // Rank of result is rank of w, unless the rows have only 1 character; make rows atoms then, removing them from rank
 r=AR(w)-(1==c); r=MAX(0,r); 
 // Allocate the result array, as floats.  If the last atom of shape was not removed, replace it with c, the output length per list
 GA(z,FL,mc,r,AS(w)); if(1<r&&1!=c)*(AS(z)+r-1)=c; zv=DAV(z);
 if(!mc)R z;  // If no fields at all, exit with empty result (avoids infinite loop below)
 // Convert the default to float, unless we are trying big integers.  We try ints if the default is int,
 // but only on 64-bit systems where int and float have the same size
 if(!(tryingint = sizeof(D)==sizeof(I) && AT(a)&B01+INT)){RZ(a=cvt(FL,a));}
 else if(AT(a)==B01)RZ(a=cvt(INT,a));  // If we are trying ints, we must promote Bool to int
 // Get the default value; supposedly a (D) but if we are trying ints it might be really an (I)
 a0=DAV(a)[0];
 // loop till all results have been produced.  Some values require a restart, so we control this field-by-field
 // rather than row-by-row
 while(1){   // Loop for each field - we know there's at least one, thus n>0
  // Skip over any \0 characters we are pointing at
  while(u<uu&&C0==*u)++u;
  // If we have consumed all the input for the current row, fill the rest of the
  // row with defaults, then advance input & output to next row; return if all done
  while(u>=y){while(k<j)zv[k++]=a0; j+=c; y+=n;}
  if(k>=mc)break;   // exit loop if all inputs processed
  // Read a number from the input, leaving v pointing to the character that stopped the conversion
  // If we are trying ints first to avoid floating-point truncation, do so
  if(valueisint = tryingint) {
    ((I *)zv)[k] = strtoint(u,&v);  // returns an I, which we store into the nominally-D result array
    if(u==v){valueisint = 0;}
      // The conversion to int failed, but that's not enough for us to write off ints.  Maybe the
      // value was invalid, and we will use the default, which is known to be int.
  }
  if(!valueisint)zv[k]=strtod(u,&v);
  // We have read a number, either as an int or a float.  Analyze the stopper character
  switch(*v){
   case ',':
    // comma.  We will remove commas from the number and then rescan it.
    b=u==v; x=v;   // b='first character was comma'; x is output pointer for the copy
    while(d=*++v)if(','!=d)*x++=d;   // copy to end-of-field, discarding commas
    if(b||','==*(v-1)){zv[k++]=a0; u=v;}else while(v>x)*x++=C0;  // if first or last character is comma, use default and continue, skipping the field;
       // otherwise put \0 over the characters after the last copied one, and go back to rescan the number
    continue;
   case '-':
    // hyphen (remember, we converted _ to hyphen to begin with).  It wasn't interpreted as a sign, so it must
    // be an infinity, if it is valid.
    e=u==v; v++; d=*v++; b=e&&C0==*v;  // e means 'first character was -'; d is character after the hyphen;
       // b is set if the - was the first character and the third character is end-of-field
    if     (e&& C0==d){zv[k]=inf;}   // -<end> infinity: take it
    else if(b&&'-'==d){zv[k]=infm;}   // --<end> neginfinity
    else if(b&&'.'==d){zv[k]=jnan;}  // -.<end> NaN
    else{zv[k++]=a0; --v; while(C0!=*v++); u=v;continue;}   // NOTA; invalid including - in the middle, use default; advance to end-of-field
    // the non-error cases fall through to process the input value...
   case C0:
    // \0 (normal end-of-field), or fallthrough from '-' non-error.  Either way it's a number.  Accept the number and continue.  But if this is the
    // first float that forces us to switch to floats, we have to do that
    if (valueisint != tryingint) {
     // We hit a float.  Forget about ints
     tryingint = 0;
     // Convert the default value to float
     a1 = a0; a0 = (D)*(I *)&a1;
     // Convert all previously-read values to float.  Also converted the default value above
     // We have to use pointer aliasing to read the value in zv as an int
     DO(k, zv[i] = (D)((I *)zv)[i];)
    }
    k++; u=v; continue;   // move to the next input number
   case 'a':  case 'b':  case 'j':  case 'p':  case 'r':  case 'x':
    // case requiring analysis for complex numbers - switch over to that code, abandoning our work here
    if(u!=v)R exec2z(a,w,n,m,c);
    // but if special character at beginning of field, that's not a valid complex number, fall through to...
   default:
    // Other stopper character, that's invalid, use default, skip the field
    zv[k++]=a0; while(C0!=*++v); u=v;
 }}
 // All done.  If we ended still looking for ints, the whole result must be int, so flag it as such
 if(tryingint)AT(z) = INT;
 R z;
}

// x ". y
F2(jtexec2){A z;B b,ii,j,p,q,x;C d,*v;I at,c,i,k,m,n,r,*s;
 RZ(a&&w);
 ASSERT(!AR(a),EVRANK);  // x must be an atom
 at=AT(a);
 ASSERT(at&NUMERIC,EVDOMAIN);  // x must be numeric
 if(!(LIT&AT(w)))RZ(w=toc1(0,w));  // convert y to ASCII if it isn't already; error if there are non-ASCII characters; error if not character type
 m=n=c=0; r=AR(w);   // get rank of y argument

 // process each list of the input to see how many numbers it contains.  We will
 // use this to set the shape of the result area
 if(!r||*(AS(w)+r-1)){    // skip the count if y is atom, or the last axis of y has dimension 0.   Nothing to count.
  // Calculate w ,"1 0 ' '   to end each (or only) line with delimiter
  RZ(w=irs2(w,chr[' '],0L,1L,0L,jtover));  // New w will be created
  v=CAV(w); r=AR(w); s=AS(w); n=s[r-1]; m=prod(r-1,s);  // v->data, m = #lists, n = length of each list
  for(i=0;i<m;++i){I j;
   // b is set when the current character is a space; p when the previous character was a space
   // k counts the number of words on this line
   // c is the max # words found on a line
   b=1; k=0; 
   for(j=0;j<n;++j){
    p=b; d=*v; b=' '==d;
    switch(d){
     case ' ':   *v=C0;  break;
     case CSIGN: *v='-'; 
    }
    ++v; if(p>b)++k;
   } 
   if(k>c)c=k;
 }}
 // c is length of each output list; the list has had _ replaced by - and space replaced by \0
 // Classify the input y according the types it contains
 numcase(m*n,CAV(w),&b,&j,&x,&q,&ii);

 // Select the conversion routine.  If the return is float/complex, make sure -0 is replaced by 0
 if(at&CMPX)                z=cvt0(exec2z(a,w,n,m,c));  // If x argument is complex, force that mode
 else if(q)                 z=     exec2q(a,w,n,m,c);  // Otherwise, if data contains rationals, use that mode
 else if(x&&at&B01+INT+XNUM)z=     exec2x(a,w,n,m,c);   // Otherwise if data contains extended integers, use that mode as long as x is compatible
 else                       z=cvt0(exec2r(a,w,n,m,c));  // otherwise do normal int/float conversion
 // Select the precision to use: the smallest that can hold the data
 R bcvt(0,z);
}
