/* Copyright 1990-2009, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Template for ipbx (boolean inner products)                              */

/* requires F(x,y) *x++g=*y++ where g is one of | & ^ */

if(c==CPLUSDOT&&(c0==IPBX1||c1==IPBX1)||c==CSTARDOT&&(c0==IPBX0||c1==IPBX0)){
 e=c==CPLUSDOT?c1==IPBX1:c1==IPBX0;
 for(i=0;i<m;++i){
  b=*av; if(ac)++av; memcpy(zv,b?v1:v0,n); if(b==e){zv+=n; continue;}
  for(j=1;j<p;++j){
   b=*av; if(ac)++av; if(b==e){memset(zv,c==CPLUSDOT?C1:C0,n); break;}
   uu=(I*)zv; vv=(I*)(b?v1+j*wc:v0+j*wc); 
   DO(q, F(uu,vv););
   if(r){u=(B*)uu; v=(B*)vv; DO(r, F(u,v););}
  }
  zv+=n;
}}else if(c==CPLUSDOT&&(c0==IPBX0||c1==IPBX0)||c==CSTARDOT&&(c0==IPBX1||c1==IPBX1)||
    c==CNE&&(c0==IPBX0||c1==IPBX0)){
 e=c==CSTARDOT?c1==IPBX1:c1==IPBX0;
 for(i=0;i<m;++i){
  b=*av; if(ac)++av; memcpy(zv,b?v1:v0,n);
  for(j=1;j<p;++j){
   b=*av; if(ac)++av; if(b==e)continue;
   uu=(I*)zv; vv=(I*)(b?v1+j*wc:v0+j*wc);
   DO(q, F(uu,vv););
   if(r){u=(B*)uu; v=(B*)vv; DO(r, F(u,v););}
  }
  zv+=n;
}}else 
 for(i=0;i<m;++i){
  memcpy(zv,*av?v1:v0,n); if(ac)++av;
  for(j=1;j<p;++j){
   uu=(I*)zv; vv=(I*)(*av?v1+j*wc:v0+j*wc); if(ac)++av;
   DO(q, F(uu,vv););
   if(r){u=(B*)uu; v=(B*)vv; DO(r, F(u,v););}
  }
  zv+=n;
 }
#undef F
