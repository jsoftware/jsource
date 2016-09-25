/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Xenos: Miscellaneous                                                    */

#include "j.h"
#include "x.h"


F1(jtassertq){ASSERTMTV(w); R scb(jt->assert);}

F1(jtasserts){B b; RE(b=b0(w)); jt->assert=b; R mtm;}

F1(jtboxq){ASSERTMTV(w); R ca(jt->bxa);}

F1(jtboxs){A x;
 RZ(w=vs(w));
 ASSERT(11==*AS(w),EVLENGTH);
 x=jt->bxa; ra(w); RZ(jt->bxa=w); jt->bx=CAV(jt->bxa); fa(x);
 R mtv;
}

F1(jtctq){ASSERTMTV(w); R scf(jt->ct);}

F1(jtcts){D d;
 ASSERT(!AR(w),EVRANK);
 RZ(w=cvt(FL,w)); d=*DAV(w);
 ASSERT(0<=d,EVDOMAIN); 
 ASSERT(d<=5.820766091e-11,EVDOMAIN);
 jt->ctdefault=jt->ct=d;
 R mtv;
}

F1(jtdispq){A z; ASSERTMTV(w); GATV(z,INT,*jt->disp,1,0); ICPY(AV(z),1+jt->disp,*jt->disp); R z;}

F1(jtdisps){I n;
 RZ(w=vi(w));
 n=AN(w);
 ASSERT(1>=AR(w),EVRANK);
 ASSERT(all1(nubsieve(w)),EVDOMAIN);
 ASSERT(all1(eps(w,eval("1 2 4 5 6"))),EVINDEX);
 *jt->disp=n; ICPY(1+jt->disp,AV(w),n);
 R mtv;
}

F1(jtdotnamesq){ASSERTMTV(w); R jt->dotnames?one:zero;}

F1(jtdotnamess){B b,c;
 RZ(w);
 ASSERT(!AR(w),EVRANK);
 if(!(B01&AT(w)))RZ(w=cvt(B01,w));
 c=jt->dotnames; jt->dotnames=b=*BAV(w);
 if(c&&!b)ds(CMDOT)=ds(CNDOT)=ds(CUDOT)=ds(CVDOT)=ds(CXDOT)=ds(CYDOT)=0;
 else if(!c&&b){
  ds(CMDOT)=mdot;
  ds(CNDOT)=ndot;
  ds(CUDOT)=udot;
  ds(CVDOT)=vdot;
  ds(CXDOT)=xdot;
  ds(CYDOT)=ydot;
 }
 R mtv;
}

F1(jtevmq){ASSERTMTV(w); R behead(jt->evm);}

F1(jtevms){A t,*tv,*wv;
 RZ(w);
 ASSERT(1==AR(w),EVRANK);
 ASSERT(NEVM==AN(w),EVLENGTH);
 ASSERT(BOX&AT(w),EVDOMAIN);
 GAT(t,BOX,1+NEVM,1,0); tv=AAV(t); 
 *tv++=mtv;
 if(ARELATIVE(w))RZ(w=car(w));
 wv=AAV(w);
 DO(NEVM, RZ(*tv++=vs(*wv++)););
 ra(t); fa(jt->evm); jt->evm=t;
 R mtv;
}

F1(jtfxx){
 RZ(w);
 ASSERT(AT(w)&LIT+BOX,EVDOMAIN);
 ASSERT(1>=AR(w),EVRANK);
 R fx(ope(w)); 
}

F1(jtiepdoq){ASSERTMTV(w); R scb(jt->iepdo);}

F1(jtiepdos){B b; RE(b=b0(w)); jt->iepdo=b; R mtm;}

F1(jtiepq){
 ASSERTMTV(w); 
 ASSERT(1==AR(w),EVRANK);
 ASSERT(!AN(w),EVDOMAIN); 
 R jt->iep?jt->iep:mtv;
}

F1(jtieps){
 RZ(w);
 ASSERT(1>=AR(w),EVRANK);
 ASSERT(!AN(w)||AT(w)&LIT,EVDOMAIN);
 fa(jt->iep);
 ra(w); RZ(jt->iep=w); 
 R mtm;
}

I prokey=1; /* enabled for 5.01 beta */

F1(jtoutparmq){A z;D*u,x;I*v;
 ASSERTMTV(w);
 if(IMAX==jt->outmaxlen||IMAX==jt->outmaxbefore||IMAX==jt->outmaxafter){
  GAT(z,FL, 4,1,0); u=DAV(z);
  u[0]=(D)jt->outeol;
  x=(D)jt->outmaxlen;    u[1]=x==IMAX?inf:x;
  x=(D)jt->outmaxbefore; u[2]=x==IMAX?inf:x;
  x=(D)jt->outmaxafter;  u[3]=x==IMAX?inf:x;
 }else{
  GAT(z,INT,4,1,0); v= AV(z);
  v[0]=jt->outeol;
  v[1]=jt->outmaxlen;
  v[2]=jt->outmaxbefore;
  v[3]=jt->outmaxafter;
 }
 R z;
}

F1(jtoutparms){I*v;
 RZ(w=vib(w));
 ASSERT(1==AR(w),EVRANK);
 ASSERT(4==AN(w),EVLENGTH);
 v=AV(w);
 ASSERT(0==v[0]||2==v[0],EVINDEX);
 ASSERT(0<=v[1],EVDOMAIN);
 ASSERT(0<=v[2],EVDOMAIN);
 ASSERT(0<=v[3],EVDOMAIN);
 jt->outeol      =v[0];
 jt->outmaxlen   =v[1];
 jt->outmaxbefore=v[2];
 jt->outmaxafter =v[3];
 R mtv;
}

F1(jtposq){ASSERTMTV(w); R v2(jt->pos[0],jt->pos[1]);}

F1(jtposs){I n,p,q,*v;
 RZ(w=vi(w));
 n=AN(w); v=AV(w);
 ASSERT(1>=AR(w),EVRANK);
 ASSERT(1==n||2==n,EVLENGTH);
 if(1==n)p=q=*v; else{p=v[0]; q=v[1];} 
 ASSERT(0<=p&&p<=2&&0<=q&&q<=2,EVDOMAIN);
 jt->pos[0]=p; jt->pos[1]=q;   
 R mtv;
}

F1(jtppq){C*end;I k;
 ASSERTMTV(w);
 k = strtoI(3+jt->pp, (char**)&end, 10);
 R sc(k);
}

F1(jtpps){I k;
 RE(sc(k=i0(w))); ASSERT(0<k,EVDOMAIN); ASSERT(k<=NPP,EVLIMIT);
 sprintf(3+jt->pp,FMTI"g", k);
 R mtv;
}

F1(jtretcommq){ASSERTMTV(w); R scb(jt->retcomm);}

F1(jtretcomms){B b; RE(b=b0(w)); jt->retcomm=b; R mtm;}

F1(jtseclevq){ASSERTMTV(w); R sc(jt->seclev);}

F1(jtseclevs){I k; 
 RE(k=i0(w)); 
 ASSERT(0==k||1==k,EVDOMAIN); 
 if(!jt->seclev&&1==k)jt->seclev=k;
 R mtm;
}

F1(jtsysparmq){I k;
 RE(k=i0(w));
 switch(k){
  default: ASSERT(0,EVINDEX);
  case 0:  R sc(jt->fdepn);
  case 1:  R sc(jt->fdepi);
  case 2:  R sc(jt->fcalln);
  case 3:  R sc(jt->fcalli);
}}

F1(jtsysparms){A*wv;I k,m,wd;
 RZ(w);
 ASSERT(BOX&AT(w),EVDOMAIN);
 ASSERT(1==AR(w),EVRANK);
 ASSERT(2==AN(w),EVLENGTH);
 wv=AAV(w); wd=(I)w*ARELATIVE(w);
 RE(k=i0(WVR(0)));
 switch(k){
  default: ASSERT(0,EVINDEX);
  case 0:  RE(m=i0(WVR(1))); jt->fdepn =m; break;
  case 1:  ASSERT(0,EVDOMAIN);  /* jt->fdepi  can not be set */
  case 2:  RE(m=i0(WVR(1))); jt->fcalln=m; break;
  case 3:  ASSERT(0,EVDOMAIN);  /* jt->fcalli can not be set */
 }
 R mtm;
}

F1(jtsysq){I j;
 ASSERTMTV(w);
 switch(SYS){
  case SYS_PC:        j=0;                break;
  case SYS_PC386:     j=1;                break;
  case SYS_PCWIN:     j=SY_WIN32 ? (SY_WINCE ? 7 : 6) : 2; break;
  case SYS_MACINTOSH: j=3;                break;
  case SYS_OS2:       j=4;                break;
  default:            j=SYS&SYS_UNIX ? 5 : -1;
 }
 R sc(j);
}

F1(jtxepq){
 ASSERTMTV(w); 
 R jt->xep?jt->xep:mtv;
}

F1(jtxeps){
 RZ(w);
 ASSERT(1>=AR(w),EVRANK);
 ASSERT(!AN(w)||AT(w)&LIT,EVDOMAIN);
 fa(jt->xep);
 ra(w); RZ(jt->xep=w); 
 R mtm;
}

F1(jtasgzombq){ASSERTMTV(w); R sc(jt->asgzomblevel);}

F1(jtasgzombs){I k; 
 RE(k=i0(w)); 
 ASSERT(0<=k&&k<=2,EVDOMAIN);
 jt->asgzomblevel=(C)k;
 R mtm;
}
