/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Format: ": Dyad                                                         */

#include "j.h"
#include "vcomp.h"


static F2(jtth2box){A z;I n,p,q,*v,x,y;
 RZ(a=vi(a)); n=AN(a); v=AV(a);
 ASSERT(1>=AR(a),EVRANK);
 ASSERT(1==n||2==n,EVLENGTH);
 x=v[0]; y=2>n?0:v[1]; 
 ASSERT((UI)x<=2&&(UI)y<=2,EVDOMAIN);
 z=jtthorn1xy(jt,w,(x<<JTTHORNXX)+(y<<JTTHORNYX));  // override default box positioning 
 RETF(z);
}

// Convert a formatted numeric string from C format to J format
// The formatted string is pointed to by 'th2buf'
//  b true if field is in exponential form
//  m width of result area (0 if unknown)
//  zv->output area
// Result is the number of characters moved to the output area
static I jtc2j(J jt,B e,I m,C*zv,A*cellbuf){C c,*s,*t;I k,p;
 // If this field calls for exponential notation, and the result contains an exponent, convert the exponent
 //  by removing leading '0' or '+'
 if(e&&(t=strchr(CAV1(*cellbuf),'e'))){   // set t->the 'e'.  t will trace through the field
  // exponent is e+n[n...] or e-n[n...]  Step over the e, and - if present
  ++t; t+='-'==*t;
  // count k=the number of leading + or 0 characters in the exponent; leave c=stopper character
  k=0; NOUNROLL while(c=t[k],c=='0'||c=='+')++k;
  if(k){
   // There are +0 characters to delete.  If we ran off the end of the exponent, back up and use one 0 (which should be there already)
   if(!c||' '==c){*t++='0'; --k;}
   NOUNROLL while(t[0]=t[k])++t;  // close up the skipped characters, till end of string (including the\0)
   // t now points to trailing \0.  Set p=(field width)-(# characters copied) = #spare characters at end of field
   // if m is 0, p is negative.  Fill to end-of-field with spaces, and append \0 if there was any fill
   p=m-(t-CAV1(*cellbuf)); DQ(p,*t++=' ';); if(0<=p)CAV1(*cellbuf)[m]=0;
 }}
 // point t to start of formatted string, and k to the length.  k will be the return value
 t=CAV1(*cellbuf); k=strlen(t);
 if(!e&&(s=memchr(t,'-',k))){  /* turn -0 to 0 */
  // Nonexponential field containing '-' (s points to the -)
  s[0]=' ';   // blank out the sign
  // see if there is a significant digit.  If so, install _ sign.  This removes the sign from -0
  DO(k-(1+s-t), c=s[1+i]; if(c!='0'&&c!='.'){s[0]=CSIGN; break;});
  // If the field was -0 in a field of unknown width, skip over the now-empty sign field
  if(!m&&' '==s[0]){++t; --k;}
 }
 // If the field has fixed width and the formatted result doesn't fit, fill result area with ***
 if(m&&m<k)mvc(m,zv,1,iotavec-IOTAVECBEGIN+'*');
 else{
  // If the field is wider than the result, install spaces (to right or left depending on exponential flag)
  //  (I don't see why this is necessary); also advance zv to point to the result area if not exponential
  if(k<m){mvc(m-k,zv+e*k,1,iotavec-IOTAVECBEGIN+' '); if(!e)zv+=m-k;}
  // In all cases except ***-fill, copy the result to the output area
  DO(k, c=t[i]; *zv++='-'==c?CSIGN:c;);
 }
 // return the length of the string copied
 R k;
}    /* c format to j format */

static B jtfmtex(J jt,I m,I d,I n,I*xv,B b,I c,I q,I ex,A*cellbuf){B bm=b||m;C*u,*v=CAV1(*cellbuf);I k;
 if(AN(*cellbuf)<20+d){GATV0(*cellbuf,LIT,20+d,1); v=CAV1(*cellbuf);}
 if(b)*v++='_'; else if(m)*v++=' '; *v++=' '; sprintf(v,FMTI,c); v+=q;
 k=(XBASEN+d+1-q)/XBASEN; k=MIN(n-1,k);
 DQ(k, c=*--xv; sprintf(v,FMTI04,b?-c:c); v+=XBASEN;);
 k=v-CAV1(*cellbuf)-(2+bm);
 if(k<d){mvc(d-k,v,1,iotavec-IOTAVECBEGIN+'0'); v+=d-k;}
 else if(k>d&&(u=v=CAV1(*cellbuf)+d+2+bm,'5'<=*v)){
  NOUNROLL while('9'==*--u);
  if(' '!=*u)++*u; else{*++u='1'; ++ex;}
  mvc(v-u-1,u+1,1,iotavec-IOTAVECBEGIN+'0');
 }
 CAV1(*cellbuf)[bm]=CAV1(*cellbuf)[bm+1]; CAV1(*cellbuf)[bm+1]='.'; sprintf(v-!d,"e"FMTI"",ex);
 R 1;
}    /* format one extended integer in exponential form */

static B jtfmtx(J jt,B e,I m,I d,C*s,I t,X*wv,A*cellbuf){B b;C*v=CAV1(*cellbuf);I c,n,p,q,*xv;X x;
 x=*wv; n=AN(x); xv=AV(x)+n-1; 
 c=*xv; b=0>c; if(b)c=-c;
 if(c==XPINF){if(b)*v++='_'; *v++='_'; *v=0; R 1;}
 q=c>999?4:c>99?3:c>9?2:1; p=q+XBASEN*(n-1);
 if(e)R fmtex(m,d,n,xv,b,c,q,p-1,cellbuf);
 else if(m&&m<b+p+d+!!d){mvc(m,v,1,iotavec-IOTAVECBEGIN+'*'); v[m]=0;}
 else{
  if(AN(*cellbuf)<4+p+d){GATV0(*cellbuf,LIT,4+p+d,1); v=CAV1(*cellbuf);}
  if(' '==s[0])*v++=' '; if(b)*v++='_'; 
  sprintf(v,FMTI,c); v+=q;
  DQ(n-1, c=*--xv; sprintf(v,FMTI04,b?-c:c); v+=XBASEN;); 
  if(d){*v++='.'; mvc(d,v,1,iotavec-IOTAVECBEGIN+'0'); v[d]=0;}
 }
 R 1;
}    /* format one extended integer */

static B jtfmtq(J jt,B e,I m,I d,C*s,I t,Q*wv,A*cellbuf){B b;C*v=CAV1(*cellbuf);I c,ex=0,k,n,p,q,*xv;Q y;X a,g,x;
 y=*wv; x=y.n; c=XDIG(x); b=0>c; if(b)x=negate(x);
 if(c==XPINF||c==XNINF){if(e)*v++=' '; if(e>b)*v++=' '; if(b)*v++='_'; *v++='_'; *v=0; R 1;}
 RZ(a=xpow(xc(10L),xc(1+d)));
 if(e&&c&&0>xcompare(x,y.d)){
  ex=XBASEN*(AN(y.n)-AN(y.d));
  g=xtymes(x,xpow(xc(10L),xc(1+d-ex)));
  RZ(x=xdiv(g,y.d,XMFLR));
  NOUNROLL while(1==xcompare(a,x)){--ex; g=xtymes(xc(10L),g); RZ(x=xdiv(g,y.d,XMFLR));}
  if(b)x=negate(x);
 }else x=xdiv(xtymes(y.n,a),y.d,XMFLR);
 RZ(x=xdiv(xplus(x,xc(5L)),xc(10L),XMFLR));
 n=AN(x); xv=AV(x)+n-1; c=*xv; b=0>c; if(b)c=-c;
 q=c>999?4:c>99?3:c>9?2:1; p=q+XBASEN*(n-1); if(c||!e)ex+=p-d-1;
 if(e)R fmtex(m,d,n,xv,b,c,q,ex,cellbuf);
 else if(m&&m<b+d+(I )!!d+(0>ex?1:1+ex)){mvc(m,v,1,iotavec-IOTAVECBEGIN+'*'); v[m]=0;}
 else{
  if(AN(*cellbuf)<4+p+d){GATV0(*cellbuf,LIT,4+p+d,1); v=CAV1(*cellbuf);}
  if(' '==s[0])*v++=' '; if(b)*v++='_';
  if(0>ex){k=-ex-1; DQ(1+MIN(d,k), *v++='0';);}
  sprintf(v,FMTI,c); v+=q;
  DQ(n-1, c=*--xv; sprintf(v,FMTI04,b?-c:c); v+=XBASEN;);
  if(d){v[1]=0; DQ(d, *v=v[-1]; --v;); *v='.';}
 }
 R 1;
}    /* format one rational number */

// Format a single number
// e,m,d describe the field as given below
// s->printf format string
// t is the type of the data
// wv->the data
// CAV1(*cellbuf)->output area
static void jtfmt1(J jt,B e,I m,I d,C*s,I t,C*wv,A*cellbuf){D y;
 switch(CTTZNOFLAG(t)){
 case INTX:;
#if SY_64
  // If I is 64 bits, and the format string ends with '.0f', change the format to lld
  C *fldend; for(fldend=s;*fldend!='d'&&*fldend!='f'&&*fldend!='e';++fldend);  // find end-of-field
  if(fldend[0]=='f'&&fldend[-1]=='0'&&fldend[-2]=='.'){fldend[-2]='l'; fldend[-1]='l'; fldend[0]='d';}
  // If I is 64 bits and the format string ends with 'd', use integer; otherwise use float
  if(fldend[0]=='d'){sprintf(CAV1(*cellbuf),s,*(I*)wv); break;}
#endif
  sprintf(CAV1(*cellbuf),s,(D)*(I*)wv);
  break;
 case B01X:  sprintf(CAV1(*cellbuf),s,(D)*wv);     break;
 case XNUMX: fmtx(e,m,d,s,t,(X*)wv,cellbuf);          break;
 case RATX:  fmtq(e,m,d,s,t,(Q*)wv,cellbuf);          break;
 default:
  y=*(D*)wv; y=y?y:0.0;  /* -0 to 0 */
  if     (!memcmpne(wv,&inf, SZD))strcpy(CAV1(*cellbuf),e?"  _" :' '==s[0]?" _" :"_" );
  else if(!memcmpne(wv,&infm,SZD))strcpy(CAV1(*cellbuf),e?" __" :' '==s[0]?" __":"__");
  else if(_isnan(*(D*)wv)      )strcpy(CAV1(*cellbuf),e?"  _.":' '==s[0]?" _.":"_.");
  else sprintf(CAV1(*cellbuf),s,y);
  break;
 }
}   /* format one number */

// format a fixed-size column.  See th2a for description of most values
//  zk stride between result areas, in bytes
//  zv->place to put result
// No result: the output area is filled
static void jtth2c(J jt,B e,I m,I d,C*s,I n,I t,I wk,C*wv,I zk,C*zv,A*cellbuf){
 DQ(n, fmt1(e,m,d,s,t,wv,cellbuf); c2j(e,m,zv,cellbuf); zv+=zk; wv+=wk;);
}    /* format a column */

// Create a column of results (a table) for a single field
// Field descriptor:
//  e is 1 if exponential, 0 if decimal
//  m is defined width of field, or 0 to use as much as needed
//  d is number of decimal places requested
//  s->sprintf descriptor
// Input info:
//  n number of values to format
//  t type of input values
//  wk stride (in bytes) between values
//  wv->first value
// first set when we are processing the first field (to suppress the leading space between fields)
static A jtth2a(J jt,B e,I m,I d,C*s,I n,I t,I wk,C*wv,B first,A*cellbuf){PROLOG(0049);A y,z;C*u,*yv,*zv;I b,i,m0=m,k,p,q;
 // Set q=nominal length of field: the length given, if m!=0; otheriwse based on type.  p=length of allocated buffer
 q=m?m:t&B01?3:t&INT?12:17; p=n*q;
  // Allocate space for all results; set shape to (n,q); zv->result area
 GATV0(z,LIT,p,2); AS(z)[0]=n; AS(z)[1]=q; zv=CAV(z);
 // If field length is fixed, format the column to that width & return it
 if(m){th2c(e,m,d,s,n,t,wk,wv,m,zv,cellbuf); R z;}
 // Otherwise, field has variable width.  Format the values one by one.
 // q holds total length so far, p is length of allocated buffer.  ; m will hold the maximum length encountered
 // b will be set for exponential fields only, if no result is negative
 b = e;  // init no negative exponential values (if field is exponential).  0 if nonexponential field
 for(i=q=0;i<n;++i){
  fmt1(e,m0,d,s,t,wv,cellbuf);  // Create the (null-terminated) string in th2buf.  m0=0
  NOUNROLL while(p<q+(I)strlen(CAV1(*cellbuf))+1){RZ(z=over(z,z)); p+=p; zv=CAV(z);}  // If new string overflows output area, double the output-area size
   // u->place to put string; convert th2buf to j form in *u; k=length of string; update string pointer & null-terminate string; advance to next input value
   u=q+zv; q+=k=c2j(e,0L,u,cellbuf); zv[q++]=0; wv+=wk;
   // Exponential-field sign spacing:
   // If the result has a negative sign, remember that fact.  If the result DOES NOT have a negative sign, then
   // for add 1 to the field-length to account for the leading space that will be added.
   // In any case, set m to max string length
   if(e)if(CSIGN==*u){b=0;}else{++k;} m = MAX(m,k);
 }
 // If this is not the first field, prepend room for a space
 // If this is an exponential field, and none of the values were negative, retract the space we left for the negative sign
 // kludge - this does not conform to Ye Dic, but deployed code may rely on it
 m+=(!first) - b;
 // Allocate final result area, an nxm table of characters.  Install shape
 GATV0(y,LIT,n*m,2); AS(y)[0]=n; AS(y)[1]=m;
 // Clear result area to spaces.  Set yv-> first result string, u->first intermediate formatted string
 yv=CAV(y); mvc(AN(y),yv,1,iotavec-IOTAVECBEGIN+' ');  u=zv;
 // Copy the strings from the formatting area (u->) to the result area (yv->)
 // For exponential fields, start copying from the left, leaving one space if there is a negative sign somewhere else
 // in the column but not in this value; advance to next input & output
 if(e){yv+=first^1; DQ(n, q=strlen(u); MC(yv+(I )(!b&&CSIGN!=*u),u,q); yv+=m; u+=1+q;);}
 // For non-exponential, right-justify the data, step to next input & output
 else {yv+=m;      DQ(n, q=strlen(u); MC(yv-q,          u,q); yv+=m; u+=1+q;);}
 EPILOG(y);
}    /* like th2c, but allocates and returns array */

// Analyze the x arg in x ": y
// Result is 0 if x is well-formed, 1 if not
// Output is 4 arrays and 1 number.  Arrays have 1 entry for each atom of a:
// ep 1 if the result is in exponential form, 0 if decimal
// mp total minimum field width
// dp number of decimal places
// sp sprintf format string to use for the field
// zkp has the total field width (if known), or 0 if there were fields of unknown width
// Also we allocate & return a conversion buffer area whose length
// is long enough to hold 1 1-cell of the result.  We calculate the needed
// size here, by starting with a length of 500 and incrementing as required
// We may get it wrong, & have to reallocate later
static A jtth2ctrl(J jt,A a,A*ep,A*mp,A*dp,A*sp,I*zkp){A da,ea,ma,s;B b=1,*ev,r,x;
  C*sv;D y;I an,*av,d,*dv,i,lenreqd,m,*mv,zk=0;Z*au;const I sk=15;
 // b means 'all fields have a defined width'
 // zk holds the total of the field sizes
 // r='non-complex a', init length of conversion area to 500 bytes, convert a to int if it's not complex
 r=!(CMPX&AT(a)); lenreqd=500;
 if(r)RZ(a=cvt(INT,a));  // this detects invalid type for a
 an=AN(a); au=ZAV(a); av=AV(a);  // an=#atoms of a, au->a data (if complex), av->a data (if real)
 // Allocate output arrays, set return value, set ?v->first value of output area
 GATV0(ea,B01,an,   1); *ep=ea; ev=BAV(ea);  // exponential flag, Boolean list
 GATV0(ma,INT,an,   1); *mp=ma; mv= AV(ma);  // field width, integer list
 GATV0(da,INT,an,   1); *dp=da; dv= AV(da);  // #decimal places, integer list
 GATV0(s, LIT,an*sk,2); *sp=s;  sv=CAV(s); AS(s)[0]=an; AS(s)[1]=sk;  // spritf string, set as nx(sk) table
 // Look at each atom of a
 for(i=0;i<an;++i){
  // Split a into field-width m and #decimal places d, and x as a flag, negative to indicate exponential form
  if(r){m=av[i]; x=m<0; d=0;}  // real a: m= value of a, d=0 (no decimal places), 
  else{
   y=au[i].re; m=(I)tfloor(y); ASSERT(TEQ(y,(D)m),EVDOMAIN); x=m<0;  // real is field size, audit is integer
   y=au[i].im; d=(I)tfloor(y); ASSERT(TEQ(y,(D)d),EVDOMAIN); if(0>y)x=1;  // imag is decimal places
  }
  // Take abs of field sizes
  if(0>m)m=-m; if(0>d)d=-d; ASSERT(0<=(m|d),EVLIMIT);  // verify no overflow
  // Create sprintf format string for the field, depending on decimal/exponential form
  if(!x)sprintf(sv, "%%"FMTI"."FMTI"f",  m,d);  // %m.df
  else  sprintf(sv, m?"%%- "FMTI"."FMTI"e" :"%%-"FMTI"."FMTI"e", m?m-1:0,d+!!(SYS&SYS_PC));  // %- m.de (m=0)  or %-m.de (m!=0)
  // store results in output areas; advance sprintf pointer; count # bytes in fields; see if there are any unknown widths
  sv+=sk; ev[i]=x; mv[i]=m; dv[i]=d; zk+=m; b=b&&m; 
  // keep the size of the conversion buffer to a minimum of the given field width or 500+the number of decimal places (in case the values overflows the field)
  if(lenreqd<m)lenreqd=m; if(lenreqd<500+d)lenreqd=500+d;
 }
 // Now that we know the conversion buffer size, allocate it
 GATV0(s,LIT,lenreqd,1);
 // Output total line width if it is valid, 0 if not
 *zkp=b?zk:0; R s;
}    /* parse format control (left argument of ":) */

// x ": y
F2(jtthorn2){PROLOG(0050);A da,ea,h,ma,s,cellbuf,y,*yv,z;B e,*ev;C*sv,*wv,*zv;I an,c,d,*dv,k,m,*mv,n,r,sk,t,wk,*ws,zk;
 F2RANK(1,RMAX,jtthorn2,DUMMYSELF);  // apply rank 1 _
 // From here on the a arg is rank 0 or 1
 an=AN(a); t=AT(w);  // an=#atoms of a, t=type of w
 if(t&BOX)R th2box(a,w);  // If boxed w, go handle as special case
 ASSERT(t&NUMERIC&&!ISSPARSE(t)&&!ISSPARSE(AT(a)),EVDOMAIN);  // w must be numeric and dense; a must be dense
 // r=rank of w; ws->shape of w; c=#atoms in 1-cell of w; n = #1-cells of w
 r=AR(w); ws=AS(w); SHAPEN(w,r-1,c);  PRODX(n,r-1,ws,1);
 ASSERT(!AR(a)||c==an,EVLENGTH);  // if a is not an atom, it must have the same length as a 1-cell of w
 // k=#bytes in an atom of w, wk=*bytes in a cell of w; wv->first atom of w (prebiased)
 k=bpnoun(t); wk=c*k; wv=CAV(w)-k;
 // Analyze a to get info for each format
 RZ(cellbuf=th2ctrl(a,&ea,&ma,&da,&s,&zk));
 // ev->expformat flags, mv->field width, dv->decimal places, sk=length of each sprintf string, sv->sprintf string (prebiased)
 ev=BAV(ea); mv=AV(ma); dv=AV(da); sk=1<an?AS(s)[1]:0; sv=CAV(s)-sk;
 if(zk||!AN(w)){
  // We know the width, or there is nothing to format.  Create the lines one by one
  if(1==an)zk*=c;   // If only one atom in a, replicate it to match a line of w
  GATV(z,LIT,n*zk,r?r:1,ws); AS(z)[AR(z)-1]=zk; zv=CAV(z);  // Allocate table for result; init shape to shape of w; replace 1-cell length with length of line; zv->result area
  // Format the fields one by one, appending the new string to the accumulated old.  We process each field specifier for the entire
  // w before moving to the next field.
  DO(c, if(i<an){e=ev[i]; m=mv[i]; d=dv[i];} th2c(e,m,d,sv+=sk,n,t,wk,wv+=k,zk,zv,&cellbuf); zv+=m;);  // Set e,m,d (if atomic a, keep the same values each time
 }else{
  // The width is unknown.  Build up the result one field at a time.  Each field becomes a box in this intermediate result
  GATV0(y,BOX,c,1); yv=AAV(y);  // Allocate boxed array, one box per field
  // Format each field into its own box
  DO(c, if(i<an){e=ev[i]; m=mv[i]; d=dv[i];} RZ(yv[i]=th2a(e,m,d,sv+=sk,n,t,wk,wv+=k,(B)!i,&cellbuf)););
  // Join the fields of each line to produce an nxc table of characters, one row per 1-cell of w
  RZ(z=razeh(y));
  // If w has rank > 2, we need to rearrange the rows into an array.  Or, if there is a single 1-cell and
  // r was an atom or a list, we need to change the result from a table to a single list.
  if(2<r||1==n&&2!=r){
   if(!r)r=1;   // change atomic r to a list
   RZ(h=vec(INT,r,ws)); AV(h)[r-1]=AS(z)[1];  // Create a vector out of the shape of w; replace length of 1-cell with length of a row of result 
   RZ(z=reshape(h,z));   // reshape the result table to that shape
 }}
 EPILOG(z);
}
