/* Copyright 1990-2005, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/* J console */
/* #define READLINE for Unix readline support */

#include "jsockos.h" 
#include "j.h"
#include "jsock.h"

#ifdef _WIN32
#include <io.h> 
#else
#define _stdcall
#endif

static char **adadbreak;
static void sigint(int k){**adadbreak+=1;signal(SIGINT,sigint);}
static char input[1000];

/* J socket callbacks */

/* J calls for output */
void _stdcall JoutputS(J jt, int type, char* s)
{
	if(MTYOEXIT==type){putdata(JCMDEXIT,(I)s,0,0,0,0); quit(); exit(0);}
	else putdata(JCMDOUT,type,strlen(s),s,0,0);
}

/* J calls for wd commands - 3 is EVDOMAIN
x    - 11!:x
w    - A* argument
p1   - result (if not 0)
p2   - result (if p1==0)

p2==0 - result is mtm

p2 -> int type, int element count, integer or char data

type==0 - char result
type==1 - char result (to be cut and boxed on 0)
type==2 - int result

return error code (3 EVDOMAIN)
*/
int _stdcall JwdS(J jt, int x, A w, A* p1, int** p2)
{
	int type;
	SOCKBUF* pb;
	/* binrep of user arg has 4 byte ints */
	int* pi=(int*)CAV(w);
	/* test for wd'q' - depends on 3!:1 format */
	if(x==0 && LIT&pi[1] && 1==pi[2] && 0==pi[3] && 'q'==*(16+(C*)pi))
	{
		int* pi=geteventdata();
		if(0==pi) return EVDOMAIN;
		*p2=pi;
		return 0;	/* wd'q' result is ready */
	}
	if(x==0)
	{
		char* pl=JGetLocale(jt);
		putdata(JCMDWD,x,AN(w),CAV(w),strlen(pl),pl);
	}
	else
		putdata(JCMDWD,x,AN(w),CAV(w),0,0);
	if(!(pb=getdata())) errorm("jwds getdata failed");
	if(pb->cmd!=JCMDWDZ) errorm("jwds not wdz");
	type = pb->type;
	if(type>CTERR) return type-CTERR;
	if(type==CTMTM) return 0;
	*p2=(int*)&pb->type;
	return 0;
}

int _stdcall JmsgsS(J jt)
{
	if(!*jt->adbreak) R 0;
	*jt->adbreak=0;
	R 1;
}

char* _stdcall JinputS(J jt, char* prompt)
{
	I n,len;SOCKBUF* pb;
	
	putdata(JCMDIN,0,strlen(prompt),prompt,0,0);
	if(!(pb=getdata())) errorm("jinputs getdata failed");
	if(pb->cmd!=JCMDINZ) errorm("jinputs not inz");
	if(pb->type==1)
	{
		seteventdata();
		len=strlen(pb->d); /* input followed by event data */
	}
	else
		len = pb->len;
	n=min(len,sizeof(input)-1);
	memcpy(input, &pb->d, n);
	input[n]=0;
	return input;
}

/* end J socket callbacks */

/* process socket messages to the end */

#define nsz 1023

void run(void* jt)
{
 I r,n,len;A a;SOCKBUF* pb;static C setname[nsz+1]="";
 while(1)
 {
  pb=getdata();
  if(!pb) errorm("run getdata failed");
  len = pb->len;
  switch(pb->cmd)
  {
	case JCMDDO:
		if(pb->type==1) /* input and event data */
		{
			seteventdata();
			len=strlen(pb->d);
		}
		if(len<sizeof(input)-1)
		{
			memcpy(input, pb->d, len);
			input[len]=0;
			r=JDo(jt,input);
		}
		else
			r=EVLENGTH;
		putdata(JCMDDOZ,r,0,0,0,0);
		break;
	case JCMDSETN:
		n=len<nsz?len:nsz;
		memcpy(setname, pb->d, n);
		setname[n]=0;
		break;			
	case JCMDSET:
		r=JSetA(jt,strlen(setname),setname,len,pb->d);
		putdata(JCMDSETZ,r,0,0,0,0);
		break;
	case JCMDGET:
		a=JGetA(jt,len,pb->d);
		if(a==0)
			putdata(JCMDGETZ,EVVALUE,0,0,0,0);
		else
			putdata(JCMDGETZ,0,AN(a),CAV(a),0,0);
		break;
	default:
		errorm("unknown command");
  }
 }
}

void addargv(int argc, char* argv[], C* d)
{
 C *p,*q; I i;
 p=d+strlen(d);
 for(i=0;i<argc;++i)
 {
  if(1==argc){*p++=',';*p++='<';}
  if(i)*p++=';';	
  *p++='\'';
  q=argv[i];
  while(*q)
  {
   *p++=*q++;
   if('\''==*(p-1))*p++='\'';
  }
  *p++='\'';
 } 
 *p=0;
}

int main(int argc, char* argv[])
{
 void *jt;
 void* callbacksS[] = {JoutputS, JwdS, JinputS, 0, (void*)SMSOCKET};
 int sockflag=0,logflag=0;

 sockflag = serverinit(argc, argv);
 if(!sockflag){fputs("Missing connection parameters.\n", stderr); return 0;}
 jt = JInit();
 if(!jt){fputs("Init j.dll failed.\n", stderr); return 0;}
 JSM(jt, callbacksS);
 adadbreak=(char**)jt; // first address in jt is address of breakdata
 signal(SIGINT,sigint);
 if(sockflag) run(jt); /* process socket messages to the end */
 return 0;
}
