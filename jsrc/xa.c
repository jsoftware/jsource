/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Xenos: Miscellaneous                                                    */

#include "j.h"
#include "x.h"

#include "cpuinfo.h"
extern uint64_t g_cpuFeatures;

#include <string.h>
#ifdef _WIN32
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif

F1(jtassertq){ASSERTMTV(w); R scb(JT(jt,assert));}

F1(jtasserts){B b; RE(b=b0(w)); JT(jt,assert)=b; R mtm;}

F1(jtboxq){ASSERTMTV(w); R str(sizeof(JT(jt,bx)),JT(jt,bx));}

F1(jtboxs){A x;
 RZ(w=vs(w));
 ASSERT(sizeof(JT(jt,bx))==AS(w)[0],EVLENGTH);
 MC(JT(jt,bx),CAV(w),sizeof(JT(jt,bx)));
 R mtv;
}  // box-display characters

// 9!:18
F1(jtctq){ASSERTMTV(w); R scf(1.0-jt->cct);}

// 9!:19
F1(jtcts){D d;
 ASSERT(!AR(w),EVRANK);
 RZ(w=cvt(FL,w)); d=DAV(w)[0];
 ASSERT(0<=d,EVDOMAIN); 
 ASSERT(d<=5.820766091e-11,EVDOMAIN);
 jt->cct=1.0-d;
 R mtv;
}

// 9!:4 and 9!:5 name caching
F1(jtnmcacheq){ASSERTMTV(w); R sc(jt->namecaching);}
F1(jtnmcaches){
 I arg=i0(w); RE(0); ASSERT(BETWEENC(arg,0,3),EVDOMAIN);  // arg must be 0, 1, or 2
 jt->namecaching|=(C)arg; if(arg==0)jt->namecaching=0; R mtv;  // save bits separately, clear if both 0, return empty vec
}

F1(jtdispq){A z; ASSERTMTV(w); GATV0(z,INT,*JT(jt,disp),1); ICPY(AV(z),1+JT(jt,disp),*JT(jt,disp)); R z;}

F1(jtdisps){UC n;
 RZ(w=vi(w));
 n=(UC)AN(w);
 ASSERT(1>=AR(w),EVRANK);
 ASSERT(all1(nubsieve(w)),EVDOMAIN);
 ASSERT(all1(eps(w,eval("1 2 4 5 6"))),EVINDEX);
 *JT(jt,disp)=n; DO(n, JT(jt,disp)[1+i]=(UC)IAV(w)[i];);
 R mtv;
}

// 9!:8 error messages
F1(jtevmq){ASSERTMTV(w); R behead(JT(jt,evm));}

// 9!:9 set error messages
F1(jtevms){A t,*tv,*wv;
 ARGCHK1(w);
 ASSERT(1==AR(w),EVRANK);
 ASSERT(NEVM==AN(w),EVLENGTH);
 ASSERT(BOX&AT(w),EVDOMAIN);
 GAT0(t,BOX,1+NEVM,1); tv=AAV(t); 
 *tv++=mtv;
 wv=AAV(w);
 DQ(NEVM, RZ(*tv=incorp(ca(vs(*wv)))); ACINITZAP(*tv) CAV(*tv)[AN(*tv)]=0; ++tv; ++wv;);  // NUL-terminate.  ca to make sure there's room.  ZAP since it's going into recursive box
 ACINITZAPRECUR(t,BOX); fa(JT(jt,evm)); JT(jt,evm)=t;  // ras to protect contents
 R mtv;
}

F1(jtfxx){
 ARGCHK1(w);
 ASSERT(AT(w)&LIT+BOX,EVDOMAIN);
 ASSERT(1>=AR(w),EVRANK);
 R fx(ope(w)); 
}

// 9!:28, immex flag
F1(jtiepdoq){ASSERTMTV(w); R scb(jt->iepdo);}

// 9!:29, immex flag
F1(jtiepdos){B b; RE(b=b0(w)); jt->iepdo=b; R mtm;}

// 9!:26, immex sentence
F1(jtiepq){
 ASSERTMTV(w); 
 ASSERT(1==AR(w),EVRANK);
 ASSERT(!AN(w),EVDOMAIN); 
 R JT(jt,iep)?JT(jt,iep):mtv;
}

// 9!:27, immex sentence
F1(jtieps){
 ARGCHK1(w);
 ASSERT(1>=AR(w),EVRANK);
 ASSERT(!AN(w)||AT(w)&LIT,EVDOMAIN);
 RZ(ras(w)); fa(JT(jt,iep));
 RZ(JT(jt,iep)=w); 
 R mtm;
}

// 9!:38
F1(jtoutparmq){A z;D*u;I*v;
 ASSERTMTV(w);
 GAT0(z,INT,4,1); v= AV(z);
 v[0]=JT(jt,outeol);
 v[1]=JT(jt,outmaxlen);
 v[2]=JT(jt,outmaxbefore);
 v[3]=JT(jt,outmaxafter);
 RETF(z);
}

// 9!:37
F1(jtoutparms){I*v;
 RZ(w=vib(w));
 ASSERT(1==AR(w),EVRANK);
 ASSERT(4==AN(w),EVLENGTH);
 v=AV(w);
 ASSERT(0==v[0]||2==v[0],EVINDEX);
 ASSERT(0<=v[1],EVDOMAIN);
 ASSERT(0<=v[2],EVDOMAIN);
 ASSERT(0<=v[3],EVDOMAIN);
 JT(jt,outeol)      =(UC)v[0];
 JT(jt,outmaxlen)   =MIN(0x7fffffff,v[1]);
 JT(jt,outmaxbefore)=MIN(0x7fffffff,v[2]);
 JT(jt,outmaxafter) =MIN(0x7fffffff,v[3]);
 R mtv;
}

F1(jtposq){ASSERTMTV(w); R v2((jt->boxpos&JTTHORNX)>>JTTHORNXX,(jt->boxpos&JTTHORNY)>>JTTHORNYX);}

F1(jtposs){I n,p,q,*v;
 RZ(w=vi(w));
 n=AN(w); v=AV(w);
 ASSERT(1>=AR(w),EVRANK);
 ASSERT(1==n||2==n,EVLENGTH);
 if(1==n)p=q=*v; else{p=v[0]; q=v[1];} 
 ASSERT(BETWEENC(p,0,2)&&BETWEENC(q,0,2),EVDOMAIN);
 jt->boxpos=(p<<JTTHORNXX)+(q<<JTTHORNYX);
 R mtv;
}

F1(jtppq){C*end;I k;
 ASSERTMTV(w);
 k = strtoI(3+jt->pp, (char**)&end, 10);
 R sc(k);
}

F1(jtpps){I k;
 RE(sc(k=i0(w))); ASSERT(0<k,EVDOMAIN); ASSERT(k<=NPP,EVLIMIT);
 snprintf(3+jt->pp,sizeof(jt->pp)-3,FMTI"g", k);
 R mtv;
}

F1(jtretcommq){ASSERTMTV(w); R scb(JT(jt,retcomm));}

F1(jtretcomms){B b; RE(b=b0(w)); JT(jt,retcomm)=b; R mtm;}

F1(jtseclevq){ASSERTMTV(w); R sc(JT(jt,seclev));}

F1(jtseclevs){I k; 
 RE(k=i0(w)); 
 ASSERT(0==k||1==k,EVDOMAIN); 
 if(!JT(jt,seclev)&&1==k)JT(jt,seclev)=(UC)k;
 R mtm;
}

#if 0  // not implemented
F1(jtsysparmq){I k;
 RE(k=i0(w));
 switch(k){
  default: ASSERT(0,EVINDEX);
  case 0:  R sc(jt->fdepn);
  case 1:  R sc(jt->fdepi);
  case 2:  R sc(jt->fcalln);
  case 3:  R sc(jt->callstacknext);
}}

F1(jtsysparms){A*wv;I k,m;
 ARGCHK1(w);
 ASSERT(BOX&AT(w),EVDOMAIN);
 ASSERT(1==AR(w),EVRANK);
 ASSERT(2==AN(w),EVLENGTH);
 wv=AAV(w); 
 RE(k=i0(wv[0]));
 switch(k){
  default: ASSERT(0,EVINDEX);
  case 0:  RE(m=i0(wv[1])); jt->fdepn =(I4)m; break;
  case 1:  ASSERT(0,EVDOMAIN);  /* jt->fdepi  can not be set */
  case 2:  RE(m=i0(wv[1])); jt->fcalln=(I4)m; break;
  case 3:  ASSERT(0,EVDOMAIN);  /* jt->fcalli can not be set */
 }
 R mtm;
}
#endif

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

F1(jtasgzombq){ASSERTMTV(w); R sc(JT(jt,asgzomblevel));}

F1(jtasgzombs){I k; 
 RE(k=i0(w)); 
 ASSERT(BETWEENC(k,0,2),EVDOMAIN);
 JT(jt,asgzomblevel)=(C)k;
 R mtm;
}

// 9!:56  undocumented
// query/override cpu feature
F1(jtcpufeature){
 ARGCHK1(w);
 ASSERT(AT(w)&LIT,EVDOMAIN);
 ASSERT(AN(w),EVLENGTH);
 ASSERT(1>=AR(w),EVRANK);
 w=str0(w);
 if (!strcasecmp(CAV(w),"CPU")) {
#if defined(__aarch64__)||defined(_M_ARM64)
  R cstr("arm64");
#elif defined(__arm__)||defined(_M_ARM)
  R cstr("arm");
#elif defined(__x86_64__)||defined(_M_X64)
  R cstr("x86_64");
#elif defined(__i386__)||defined(_M_IX86)
  R cstr("x86");
#else
  R cstr("unknown");
#endif
 }
#if defined(__aarch64__)
 if     (!strcasecmp(CAV(w),"FP"      )) R sc(!!(getCpuFeatures()&ARM_HWCAP_FP ));
 else if(!strcasecmp(CAV(w),"ASIMD"   )) R sc(!!(getCpuFeatures()&ARM_HWCAP_ASIMD ));
 else if(!strcasecmp(CAV(w),"EVTSTRM" )) R sc(!!(getCpuFeatures()&ARM_HWCAP_EVTSTRM ));
 else if(!strcasecmp(CAV(w),"AES"     )) R sc(!!(getCpuFeatures()&ARM_HWCAP_AES ));
 else if(!strcasecmp(CAV(w),"PMULL"   )) R sc(!!(getCpuFeatures()&ARM_HWCAP_PMULL ));
 else if(!strcasecmp(CAV(w),"SHA1"    )) R sc(!!(getCpuFeatures()&ARM_HWCAP_SHA1 ));
 else if(!strcasecmp(CAV(w),"SHA2"    )) R sc(!!(getCpuFeatures()&ARM_HWCAP_SHA2 ));
 else if(!strcasecmp(CAV(w),"CRC32"   )) R sc(!!(getCpuFeatures()&ARM_HWCAP_CRC32 ));
 else if(!strcasecmp(CAV(w),"ATOMICS" )) R sc(!!(getCpuFeatures()&ARM_HWCAP_ATOMICS ));
 else if(!strcasecmp(CAV(w),"FPHP"    )) R sc(!!(getCpuFeatures()&ARM_HWCAP_FPHP ));
 else if(!strcasecmp(CAV(w),"ASIMDHP" )) R sc(!!(getCpuFeatures()&ARM_HWCAP_ASIMDHP ));
 else if(!strcasecmp(CAV(w),"CPUID"   )) R sc(!!(getCpuFeatures()&ARM_HWCAP_CPUID ));
 else if(!strcasecmp(CAV(w),"ASIMDRDM")) R sc(!!(getCpuFeatures()&ARM_HWCAP_ASIMDRDM ));
 else if(!strcasecmp(CAV(w),"JSCVT"   )) R sc(!!(getCpuFeatures()&ARM_HWCAP_JSCVT ));
 else if(!strcasecmp(CAV(w),"FCMA"    )) R sc(!!(getCpuFeatures()&ARM_HWCAP_FCMA ));
 else if(!strcasecmp(CAV(w),"LRCPC"   )) R sc(!!(getCpuFeatures()&ARM_HWCAP_LRCPC ));
 else if(!strcasecmp(CAV(w),"DCPOP"   )) R sc(!!(getCpuFeatures()&ARM_HWCAP_DCPOP ));
 else if(!strcasecmp(CAV(w),"SHA3"    )) R sc(!!(getCpuFeatures()&ARM_HWCAP_SHA3 ));
 else if(!strcasecmp(CAV(w),"SM3"     )) R sc(!!(getCpuFeatures()&ARM_HWCAP_SM3 ));
 else if(!strcasecmp(CAV(w),"SM4"     )) R sc(!!(getCpuFeatures()&ARM_HWCAP_SM4 ));
 else if(!strcasecmp(CAV(w),"ASIMDDP" )) R sc(!!(getCpuFeatures()&ARM_HWCAP_ASIMDDP ));
 else if(!strcasecmp(CAV(w),"SHA512"  )) R sc(!!(getCpuFeatures()&ARM_HWCAP_SHA512 ));
 else if(!strcasecmp(CAV(w),"SVE"     )) R sc(!!(getCpuFeatures()&ARM_HWCAP_SVE ));
 else if(!strcasecmp(CAV(w),"ASIMDFHM")) R sc(!!(getCpuFeatures()&ARM_HWCAP_ASIMDFHM ));
 else if(!strcasecmp(CAV(w),"DIT"     )) R sc(!!(getCpuFeatures()&ARM_HWCAP_DIT ));
 else if(!strcasecmp(CAV(w),"USCAT"   )) R sc(!!(getCpuFeatures()&ARM_HWCAP_USCAT ));
 else if(!strcasecmp(CAV(w),"ILRCPC"  )) R sc(!!(getCpuFeatures()&ARM_HWCAP_ILRCPC ));
 else if(!strcasecmp(CAV(w),"FLAGM"   )) R sc(!!(getCpuFeatures()&ARM_HWCAP_FLAGM ));
 else R sc(0);
#elif defined(__x86_64__)||defined(__i386__)||defined(_M_X64)||defined(_M_IX86)
 if     (!strcasecmp(CAV(w),"SSSE3"   )) R sc(!!(getCpuFeatures()&CPU_X86_FEATURE_SSSE3 ));
 else if(!strcasecmp(CAV(w),"POPCNT"  )) R sc(!!(getCpuFeatures()&CPU_X86_FEATURE_POPCNT ));
 else if(!strcasecmp(CAV(w),"MOVBE"   )) R sc(!!(getCpuFeatures()&CPU_X86_FEATURE_MOVBE ));
 else if(!strcasecmp(CAV(w),"SSE4_1"  )) R sc(!!(getCpuFeatures()&CPU_X86_FEATURE_SSE4_1 ));
 else if(!strcasecmp(CAV(w),"SSE4_2"  )) R sc(!!(getCpuFeatures()&CPU_X86_FEATURE_SSE4_2 ));
 else if(!strcasecmp(CAV(w),"AES_NI"  )) R sc(!!(getCpuFeatures()&CPU_X86_FEATURE_AES_NI ));
 else if(!strcasecmp(CAV(w),"AVX"     )) R sc(!!(getCpuFeatures()&CPU_X86_FEATURE_AVX ));
 else if(!strcasecmp(CAV(w),"RDRAND"  )) R sc(!!(getCpuFeatures()&CPU_X86_FEATURE_RDRAND ));
 else if(!strcasecmp(CAV(w),"AVX2"    )) R sc(!!(getCpuFeatures()&CPU_X86_FEATURE_AVX2 ));
 else if(!strcasecmp(CAV(w),"SHA_NI"  )) R sc(!!(getCpuFeatures()&CPU_X86_FEATURE_SHA_NI ));
 else if(!strcasecmp(CAV(w),"FMA"     )) R sc(!!(getCpuFeatures()&CPU_X86_FEATURE_FMA ));
 else if(!strcasecmp(CAV(w),"RDSEED"  )) R sc(!!(getCpuFeatures()&CPU_X86_FEATURE_RDSEED ));
 else if(!strcasecmp(CAV(w),"BMI1"  )) R sc(!!(getCpuFeatures()&CPU_X86_FEATURE_BMI1 ));
 else if(!strcasecmp(CAV(w),"BMI2"  )) R sc(!!(getCpuFeatures()&CPU_X86_FEATURE_BMI2 ));
 else R sc(0);
#else
 R sc(0);
#endif
}

// thread unsafe
F2(jtcpufeature2){I k;
 ARGCHK2(a,w);
 ASSERT(AT(w)&LIT,EVDOMAIN);
 ASSERT(AN(w),EVLENGTH);
 ASSERT(1>=AR(w),EVRANK);
 RE(k=i0(a));  // get arg
 ASSERT(k==0||k==1,EVDOMAIN);
 w=str0(w);
 if(k){
#if defined(__aarch64__)
 if     (!strcasecmp(CAV(w),"FP"      )) g_cpuFeatures |= ARM_HWCAP_FP ;
 else if(!strcasecmp(CAV(w),"ASIMD"   )) g_cpuFeatures |= ARM_HWCAP_ASIMD ;
 else if(!strcasecmp(CAV(w),"EVTSTRM" )) g_cpuFeatures |= ARM_HWCAP_EVTSTRM ;
 else if(!strcasecmp(CAV(w),"AES"     )) g_cpuFeatures |= ARM_HWCAP_AES ;
 else if(!strcasecmp(CAV(w),"PMULL"   )) g_cpuFeatures |= ARM_HWCAP_PMULL ;
 else if(!strcasecmp(CAV(w),"SHA1"    )) g_cpuFeatures |= ARM_HWCAP_SHA1 ;
 else if(!strcasecmp(CAV(w),"SHA2"    )) g_cpuFeatures |= ARM_HWCAP_SHA2 ;
 else if(!strcasecmp(CAV(w),"CRC32"   )) g_cpuFeatures |= ARM_HWCAP_CRC32 ;
 else if(!strcasecmp(CAV(w),"ATOMICS" )) g_cpuFeatures |= ARM_HWCAP_ATOMICS ;
 else if(!strcasecmp(CAV(w),"FPHP"    )) g_cpuFeatures |= ARM_HWCAP_FPHP ;
 else if(!strcasecmp(CAV(w),"ASIMDHP" )) g_cpuFeatures |= ARM_HWCAP_ASIMDHP ;
 else if(!strcasecmp(CAV(w),"CPUID"   )) g_cpuFeatures |= ARM_HWCAP_CPUID ;
 else if(!strcasecmp(CAV(w),"ASIMDRDM")) g_cpuFeatures |= ARM_HWCAP_ASIMDRDM ;
 else if(!strcasecmp(CAV(w),"JSCVT"   )) g_cpuFeatures |= ARM_HWCAP_JSCVT ;
 else if(!strcasecmp(CAV(w),"FCMA"    )) g_cpuFeatures |= ARM_HWCAP_FCMA ;
 else if(!strcasecmp(CAV(w),"LRCPC"   )) g_cpuFeatures |= ARM_HWCAP_LRCPC ;
 else if(!strcasecmp(CAV(w),"DCPOP"   )) g_cpuFeatures |= ARM_HWCAP_DCPOP ;
 else if(!strcasecmp(CAV(w),"SHA3"    )) g_cpuFeatures |= ARM_HWCAP_SHA3 ;
 else if(!strcasecmp(CAV(w),"SM3"     )) g_cpuFeatures |= ARM_HWCAP_SM3 ;
 else if(!strcasecmp(CAV(w),"SM4"     )) g_cpuFeatures |= ARM_HWCAP_SM4 ;
 else if(!strcasecmp(CAV(w),"ASIMDDP" )) g_cpuFeatures |= ARM_HWCAP_ASIMDDP ;
 else if(!strcasecmp(CAV(w),"SHA512"  )) g_cpuFeatures |= ARM_HWCAP_SHA512 ;
 else if(!strcasecmp(CAV(w),"SVE"     )) g_cpuFeatures |= ARM_HWCAP_SVE ;
 else if(!strcasecmp(CAV(w),"ASIMDFHM")) g_cpuFeatures |= ARM_HWCAP_ASIMDFHM ;
 else if(!strcasecmp(CAV(w),"DIT"     )) g_cpuFeatures |= ARM_HWCAP_DIT ;
 else if(!strcasecmp(CAV(w),"USCAT"   )) g_cpuFeatures |= ARM_HWCAP_USCAT ;
 else if(!strcasecmp(CAV(w),"ILRCPC"  )) g_cpuFeatures |= ARM_HWCAP_ILRCPC ;
 else if(!strcasecmp(CAV(w),"FLAGM"   )) g_cpuFeatures |= ARM_HWCAP_FLAGM ;
#elif defined(__x86_64__)||defined(__i386__)||defined(_M_X64)||defined(_M_IX86)
 if     (!strcasecmp(CAV(w),"SSSE3"   )) g_cpuFeatures |= CPU_X86_FEATURE_SSSE3 ;
 else if(!strcasecmp(CAV(w),"POPCNT"  )) g_cpuFeatures |= CPU_X86_FEATURE_POPCNT ;
 else if(!strcasecmp(CAV(w),"MOVBE"   )) g_cpuFeatures |= CPU_X86_FEATURE_MOVBE ;
 else if(!strcasecmp(CAV(w),"SSE4_1"  )) g_cpuFeatures |= CPU_X86_FEATURE_SSE4_1 ;
 else if(!strcasecmp(CAV(w),"SSE4_2"  )) g_cpuFeatures |= CPU_X86_FEATURE_SSE4_2 ;
 else if(!strcasecmp(CAV(w),"AES_NI"  )) g_cpuFeatures |= CPU_X86_FEATURE_AES_NI ;
 else if(!strcasecmp(CAV(w),"AVX"     )) g_cpuFeatures |= CPU_X86_FEATURE_AVX ;
 else if(!strcasecmp(CAV(w),"RDRAND"  )) g_cpuFeatures |= CPU_X86_FEATURE_RDRAND ;
 else if(!strcasecmp(CAV(w),"AVX2"    )) g_cpuFeatures |= CPU_X86_FEATURE_AVX2 ;
 else if(!strcasecmp(CAV(w),"SHA_NI"  )) g_cpuFeatures |= CPU_X86_FEATURE_SHA_NI ;
 else if(!strcasecmp(CAV(w),"FMA"     )) g_cpuFeatures |= CPU_X86_FEATURE_FMA ;
 else if(!strcasecmp(CAV(w),"RDSEED"  )) g_cpuFeatures |= CPU_X86_FEATURE_RDSEED ;
 else if(!strcasecmp(CAV(w),"BMI1"  )) g_cpuFeatures |= CPU_X86_FEATURE_BMI1 ;
 else if(!strcasecmp(CAV(w),"BMI2"  )) g_cpuFeatures |= CPU_X86_FEATURE_BMI2 ;
#endif
 } else {
#if defined(__aarch64__)
 if     (!strcasecmp(CAV(w),"FP"      )) g_cpuFeatures &= ~ARM_HWCAP_FP ;
 else if(!strcasecmp(CAV(w),"ASIMD"   )) g_cpuFeatures &= ~ARM_HWCAP_ASIMD ;
 else if(!strcasecmp(CAV(w),"EVTSTRM" )) g_cpuFeatures &= ~ARM_HWCAP_EVTSTRM ;
 else if(!strcasecmp(CAV(w),"AES"     )) g_cpuFeatures &= ~ARM_HWCAP_AES ;
 else if(!strcasecmp(CAV(w),"PMULL"   )) g_cpuFeatures &= ~ARM_HWCAP_PMULL ;
 else if(!strcasecmp(CAV(w),"SHA1"    )) g_cpuFeatures &= ~ARM_HWCAP_SHA1 ;
 else if(!strcasecmp(CAV(w),"SHA2"    )) g_cpuFeatures &= ~ARM_HWCAP_SHA2 ;
 else if(!strcasecmp(CAV(w),"CRC32"   )) g_cpuFeatures &= ~ARM_HWCAP_CRC32 ;
 else if(!strcasecmp(CAV(w),"ATOMICS" )) g_cpuFeatures &= ~ARM_HWCAP_ATOMICS ;
 else if(!strcasecmp(CAV(w),"FPHP"    )) g_cpuFeatures &= ~ARM_HWCAP_FPHP ;
 else if(!strcasecmp(CAV(w),"ASIMDHP" )) g_cpuFeatures &= ~ARM_HWCAP_ASIMDHP ;
 else if(!strcasecmp(CAV(w),"CPUID"   )) g_cpuFeatures &= ~ARM_HWCAP_CPUID ;
 else if(!strcasecmp(CAV(w),"ASIMDRDM")) g_cpuFeatures &= ~ARM_HWCAP_ASIMDRDM ;
 else if(!strcasecmp(CAV(w),"JSCVT"   )) g_cpuFeatures &= ~ARM_HWCAP_JSCVT ;
 else if(!strcasecmp(CAV(w),"FCMA"    )) g_cpuFeatures &= ~ARM_HWCAP_FCMA ;
 else if(!strcasecmp(CAV(w),"LRCPC"   )) g_cpuFeatures &= ~ARM_HWCAP_LRCPC ;
 else if(!strcasecmp(CAV(w),"DCPOP"   )) g_cpuFeatures &= ~ARM_HWCAP_DCPOP ;
 else if(!strcasecmp(CAV(w),"SHA3"    )) g_cpuFeatures &= ~ARM_HWCAP_SHA3 ;
 else if(!strcasecmp(CAV(w),"SM3"     )) g_cpuFeatures &= ~ARM_HWCAP_SM3 ;
 else if(!strcasecmp(CAV(w),"SM4"     )) g_cpuFeatures &= ~ARM_HWCAP_SM4 ;
 else if(!strcasecmp(CAV(w),"ASIMDDP" )) g_cpuFeatures &= ~ARM_HWCAP_ASIMDDP ;
 else if(!strcasecmp(CAV(w),"SHA512"  )) g_cpuFeatures &= ~ARM_HWCAP_SHA512 ;
 else if(!strcasecmp(CAV(w),"SVE"     )) g_cpuFeatures &= ~ARM_HWCAP_SVE ;
 else if(!strcasecmp(CAV(w),"ASIMDFHM")) g_cpuFeatures &= ~ARM_HWCAP_ASIMDFHM ;
 else if(!strcasecmp(CAV(w),"DIT"     )) g_cpuFeatures &= ~ARM_HWCAP_DIT ;
 else if(!strcasecmp(CAV(w),"USCAT"   )) g_cpuFeatures &= ~ARM_HWCAP_USCAT ;
 else if(!strcasecmp(CAV(w),"ILRCPC"  )) g_cpuFeatures &= ~ARM_HWCAP_ILRCPC ;
 else if(!strcasecmp(CAV(w),"FLAGM"   )) g_cpuFeatures &= ~ARM_HWCAP_FLAGM ;
#elif defined(__x86_64__)||defined(__i386__)||defined(_M_X64)||defined(_M_IX86)
 if     (!strcasecmp(CAV(w),"SSSE3"   )) g_cpuFeatures &= ~CPU_X86_FEATURE_SSSE3 ;
 else if(!strcasecmp(CAV(w),"POPCNT"  )) g_cpuFeatures &= ~CPU_X86_FEATURE_POPCNT ;
 else if(!strcasecmp(CAV(w),"MOVBE"   )) g_cpuFeatures &= ~CPU_X86_FEATURE_MOVBE ;
 else if(!strcasecmp(CAV(w),"SSE4_1"  )) g_cpuFeatures &= ~CPU_X86_FEATURE_SSE4_1 ;
 else if(!strcasecmp(CAV(w),"SSE4_2"  )) g_cpuFeatures &= ~CPU_X86_FEATURE_SSE4_2 ;
 else if(!strcasecmp(CAV(w),"AES_NI"  )) g_cpuFeatures &= ~CPU_X86_FEATURE_AES_NI ;
 else if(!strcasecmp(CAV(w),"AVX"     )) g_cpuFeatures &= ~CPU_X86_FEATURE_AVX ;
 else if(!strcasecmp(CAV(w),"RDRAND"  )) g_cpuFeatures &= ~CPU_X86_FEATURE_RDRAND ;
 else if(!strcasecmp(CAV(w),"AVX2"    )) g_cpuFeatures &= ~CPU_X86_FEATURE_AVX2 ;
 else if(!strcasecmp(CAV(w),"SHA_NI"  )) g_cpuFeatures &= ~CPU_X86_FEATURE_SHA_NI ;
 else if(!strcasecmp(CAV(w),"FMA"     )) g_cpuFeatures &= ~CPU_X86_FEATURE_FMA ;
 else if(!strcasecmp(CAV(w),"RDSEED"  )) g_cpuFeatures &= ~CPU_X86_FEATURE_RDSEED ;
 else if(!strcasecmp(CAV(w),"BMI1"  )) g_cpuFeatures &= ~CPU_X86_FEATURE_BMI1 ;
 else if(!strcasecmp(CAV(w),"BMI2"  )) g_cpuFeatures &= ~CPU_X86_FEATURE_BMI2 ;
#endif
}
OPENSSL_setcap();
#if C_AVX && !defined(ANDROID)
 hwfma=(getCpuFeatures()&CPU_X86_FEATURE_FMA)?1:0;
#endif
R mtm;
}

// 9!:58  undocumented
// query/set gemm threshold
// 0 igemm_thres  integer threshold
// 1 dgemm_thres  real threshold
// 2 zgemm_thres  complex threshold

F1(jtgemmtune){I k;
 ARGCHK1(w);
 ASSERT(AT(w)&(B01+INT),EVDOMAIN);
 ASSERT(1==AN(w),EVLENGTH);
 ASSERT(1>=AR(w),EVRANK);
 RE(k=i0(w));  // get arg
 ASSERT(k==0||k==1||k==2,EVDOMAIN);
 R sc((0==k)?JT(jt,igemm_thres):(1==k)?JT(jt,dgemm_thres):JT(jt,zgemm_thres));
}

F2(jtgemmtune2){I j,k;
 ARGCHK2(a,w);
 ASSERT(AT(a)&(B01+INT),EVDOMAIN);
 ASSERT(1==AN(a),EVLENGTH);
 ASSERT(1>=AR(a),EVRANK);
 ASSERT(AT(w)&(B01+INT),EVDOMAIN);
 ASSERT(1==AN(w),EVLENGTH);
 ASSERT(1>=AR(w),EVRANK);
 RE(j=i0(a));  // get arg
 RE(k=i0(w));  // get arg
 ASSERT(j>=-1,EVDOMAIN);
 ASSERT(k==0||k==1||k==2,EVDOMAIN);
 if(k==0) JT(jt,igemm_thres)=j;
 else if(k==1) JT(jt,dgemm_thres)=j;
 else JT(jt,zgemm_thres)=j;
 R sc(1);
}

// enable/disable tstack auditing, since some testcases run too long with it enabled
// bit 0 is set to disable, bit 1 is a one-shot to ask for an audit
// result is old value
F1(jtaudittdisab){
#if MEMAUDIT&2
 I k,oldval;
 oldval = JT(jt,audittstackdisabled);  // return value
 RE(k=i0(w));  // get arg
 if(k&2){JT(jt,audittstackdisabled)=0; audittstack(jt);}  // run once if so requested
 JT(jt,audittstackdisabled)=k;
 R sc(oldval);
#else
 R sc(0);
#endif
}

// 9!:66 check compiler genberated code for feature level
F1(jtcheckcompfeatures){UI i;I v1,v2,temp;
 RZ(w=vib(w));  // inputs must be integer
 I ttype=IAV(w)[0];  // test type
 I featon=IAV(w)[1];  // 0=run code that does not use the feature, 1=use feature.  The code using the feature will be faster is the feature is present, otherwise slower
 ASSERT((featon&-2)==0,EVDOMAIN);
 switch(2*ttype+featon){
 default: ASSERT(0,EVDOMAIN);  // invalid feature
 case 2*0+0: ;  // andn, not used
  v1=(I)w, v2=AN(w);  // two unpredictable values
  NOUNROLL for(i=100000000; i; --i){
   v1&=v2; v2&=(v1+SGNTO0(v1));
  }
  break;
 case 2*0+1: ;  // andn, used
  v1=(I)w, v2=AN(w);  // two unpredictable values
  NOUNROLL for(i=100000000; i; --i){
   v1&=~v2; v2&=~(v1+SGNTO0(v1));
  }
  break;
 case 2*1+0: ;  // blsr, not used
  v1=(I)w, v2=AN(w);  // two unpredictable values
  NOUNROLL for(i=100000000; i; --i){
   v1+=SGNTO0(v2); v2+=SGNTO0(v1);
  }
  break;
 case 2*1+1: ;  // blsr, used
  v1=(I)w, v2=AN(w);  // two unpredictable values
  NOUNROLL for(i=100000000; i; --i){
   v1+=LOWESTBIT(v2); v2+=LOWESTBIT(v1);
  }
  break;
 case 2*2+0: ;  // shlx, used
  v1=(I)w, v2=AN(w);  // two unpredictable values
  NOUNROLL for(i=100000000; i; --i){
   I v3=v1+v2; v2=v3+v1; v1=v2+v3;
  }
  break;
 case 2*2+1: ;  // shlx, used
  v1=(I)w, v2=AN(w);  // two unpredictable values
  NOUNROLL for(i=100000000; i; --i){
   I v3=v1<<v2; v2=v3<<v1; v1=v2<<v3;
  }
  break;
 case 2*3+0: ;  // tzcnt, not used
  v1=(I)w, v2=AN(w);  // two unpredictable values
  NOUNROLL for(i=100000000; i; --i){
   v1+=v1*v1;
  }
  break;
 case 2*3+1: ;  // tzcnt, used
  v1=(I)w, v2=AN(w);  // two unpredictable values
  NOUNROLL for(i=100000000; i; --i){
   v1+=CTTZI(v1);
  }
  break;
 case 2*4+0: ;  // lzcnt, not used
  v1=(I)w, v2=AN(w);  // two unpredictable values
  NOUNROLL for(i=100000000; i; --i){
   v1+=v1*v1;
  }
  break;
 case 2*4+1: ;  // lzcnt, used
  v1=(I)w, v2=AN(w);  // two unpredictable values
  NOUNROLL for(i=100000000; i; --i){
   I temp; CTLZI(v1,temp); v1=v1+(63^temp);
  }
  break;
 }
 temp=v2-v1; forcetomemory(&temp);  // make sure code executes
 R mtv;
}

