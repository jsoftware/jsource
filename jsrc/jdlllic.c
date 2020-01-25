/* Copyright (c) 1990-2000, Iverson Software Inc.  All rights reserved.    */
/* Licensed to Jsoftware Inc.                                              */
/*                                                                         */
/* Private Jsoftware encode/decode routines - license keys and ijl         */

#ifdef _WIN32
#include <windows.h>
#endif

#include <stdlib.h>
#include <math.h>
#include <ctype.h>

#if !SY_WINCE
#include <time.h>
#endif

#include "j.h"

#define max(a,b)    (((a) > (b)) ? (a) : (b))

typedef struct
{
	char sn[5];
	char type;
	char platform;
	char bb[2];
	char check[2];
} RN;

/* product release date - months into century */
int releasedate = 9+12*00;	/* 4.05 released September 2000 */

double PRIME = 99999999571.0;
double MULT = 261613;
double BASE = 1e6;
double BASEPOWER[4] = {4290,1000000,1000000,1};

static double mp();

static void xmp(x,y) double *x,*y;
{
	int i;

	for(i=0; i<4; ++i)
		y[i] = mp(x[i], BASEPOWER[i]);
}

static double mod(x) double x;
{
	return fmod(x, PRIME);
}

static double mp(x,y) double x,y;
{
	double r[4],q[4],x0,x1,y0,y1,t=0;
	int i;

	if(BASE >= x && BASE >= y) 
		return fmod(x*y, PRIME);
	x0 = floor(x / BASE);
	x1 = fmod(x, BASE);
	y0 = floor(y / BASE);
	y1 = fmod(y, BASE);
	r[0] = mod(x0*y0);
	r[1] = mod(x0*y1);
	r[2] = mod(x1*y0);
	r[3] = mod(x1*y1);
	xmp(r, q); 
	for(i=0; i<4; ++i)
		t += mod(q[i]);
	return mod(t);
}

#define XX(x) (x^0x55)
static char
shhh[]={7,XX('L'),XX('i'),XX('b'),XX('r'),XX('a'),XX('r'),XX('y')};

static int decode(q,code,check) char *q,*code,check;
{
	char rn[256];
	char *p;
	double f;
	int n;
	char cs[21];

	p = rn;
	while(*q)
	{
		if(isdigit(*q))
			*p++ = *q;
		q++;
	}
	*p = 0;
	if(strlen(rn) != 11) return 1;
	f = atof(rn);
	f = mp(f, MULT);
	sprintf (cs, "%f", f);
	strcat(cs,"."); /* in case no . */
	p=strchr(cs,'.');
	*p=0;
	n=(int)(p-cs);
	memset(code,'0',11);
	strcpy(code+11-n, cs); /* padded with leading 0's to 11 digits */
	return 0;
}

int snvalidate(char* sn)
{
    char *p,*q;
    char rn[256];
    char code[21];
    RN* prn;
    int i,bb,ck;
	
	if(!strcmp(sn, "beta")) return 1; //! enable for beta
    for(i=0,q=rn;i<shhh[0];i++) *q++=shhh[1+i]^0x55; *q='\0';
    if(!strcmp(sn, rn)) return 1;
	if(decode(sn, code, '0')) return 0;
	prn = (RN*)code;

	/* check checksum */
	p=code; ck=0;
	while(p<code+9)ck+=*p++-'0';
	if(ck!=10*(prn->check[0]-'0')+prn->check[1]-'0') return 0;
	if('9' != prn->type) return 0; /* unknown type */ 
	bb=(10*(prn->bb[0]-'0'))+prn->bb[1]-'0'; /* bestbefore months in 2000 */
	if(releasedate>bb) return 0; /* key is old */
	return '0'==prn->platform;
}

#define SK				133
#define LOCKEXTRA		5
#define SERIALNUMSIZE	32
#define lobyte(w)		((UC)(w))

#if !(SYS & SYS_LILENDIAN)
int swapint(int n){C* p,c;
 p=(C*)&n;
 c=p[3];p[3]=p[0];p[0]=c;
 c=p[2];p[2]=p[1];p[1]=c;
 R n;
}
#endif

F1(jtlock1){A z; C* p; C* src;
 UC c,c1,c2,k1[SK],k2[SK];    
 int r,len,xlen,maxc1,maxc2,i,j,k;

 RZ(w);
 ASSERT(1==AR(w),EVRANK);
 ASSERT(prokey&&AT(w)&LIT,EVDOMAIN);
 src=UAV(w);
 len=(int)AN(w);
 xlen=len;
#if SY_WINCE
 {SYSTEMTIME st;GetLocalTime(&st);srand((UC)(st.wMilliseconds)); }
#else
 srand((UC)time(NULL));
#endif
 c1=lobyte(rand());
 c2=lobyte(rand());
 maxc1=max(33,c1);
 maxc2=max(33,c2);
 r=4+4+SK+LOCKEXTRA+maxc1+len+SK+maxc2+2*SERIALNUMSIZE; /* result len */
 GATV0(z,LIT,r,1);
 p=CAV(z);
#if !(SYS & SYS_LILENDIAN)
 xlen=swapint(len);
#endif
 for(i=0;i<SK;++i) k1[i] = lobyte(rand());
 for(i=0;i<SK;++i) k2[i] = lobyte(rand());
 *p++=(UC)255; *p++=0; *p++=c1; *p++=c2;
 for(i=0;i<4;++i) *p++= k1[i] ^ *(i+(UC*)&xlen);
 MC(p,k1,SK);
 p+=SK;
 k=LOCKEXTRA+maxc1;
 for(i=0;i<k;++i) *p++= lobyte(rand());
 j=0;
 for(i=0;i<len;++i)
 {
  c=*src++;
  c= c^k1[j];
  if(++j==SK) j=0;
  *p++=c;
 }
 MC(p,k2,SK);
 p+=SK;
 for(i=0;i<maxc2;++i) *p++= lobyte(rand());
 for(i=0;i<2*SERIALNUMSIZE;++i) *p++= lobyte(rand());
 R z;
}

F2(jtlock2){ASSERT(0,EVDOMAIN);}

F1(jtunlock1){R unlock2(mtv,w);}

F2(jtunlock2){int i,j,len,tlen;UC c1,c2,k1[SK],*lp,*sp,*d;
 RZ(a&&w);  
 d=UAV(w);
 tlen=(int)AN(w);
 if(!tlen || 255!=d[0] || 0 != d[1] || tlen<8+SK) return w;	/* not jl */
 MC(k1, d+8, SK);
 for(i=0;i<(int)sizeof(int);++i)	*(i+(UC*)&len) = k1[i] ^ d[4+i];
#if !(SYS & SYS_LILENDIAN)
 len=swapint(len);
#endif
 c1 = max(33,d[2]);
 c2 = max(33,d[3]);
 if(!tlen == (8+LOCKEXTRA+c1+c2+len+ 2*SK + 2*SERIALNUMSIZE)) return w;	/* not jl */
 lp = d+8+SK+c1+LOCKEXTRA;
 sp = d;
 j=0;
 for(i=0;i<len;++i)
 {
  *sp++ = *lp++ ^ k1[j];
  if(++j==SK) j=0;
 }
 while(sp<d+tlen)*sp++=' ';
 R w;
}    /* ignores left argument */
