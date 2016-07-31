/* Copyright 1990-2003, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* For Symbol Tables aka Locales                                           */


#define LOCPATH(g)      ((*AV(g)+jt->sympv)->val )
#define LOCNAME(g)      ((*AV(g)+jt->sympv)->name)
#define NMHASH(p,s)     (*(s)+(p)+99991L*(UC)(s)[(p)-1])


/* macro to define a function that walks through a symbol table */
/* f - name of derived function                                 */
/* T       - for z, C datatype                                  */
/* TYPE    - for z, J datatype                                  */
/* COUNT   - for z, # of initial items for z                    */
/* COL     - for z, # columns                                   */
/* SELECT  - selection function on a name                       */
/* PROCESS - processing on a selected name                      */

#define SYMWALK(f,T,TYPE,COUNT,COL,SELECT,PROCESS)  \
 F1(f){A z;I*e,i,j,k,m=0,n;L*d;T*zv;                         \
  RZ(w);                                                     \
  n=AN(w); e=1+AV(w);                                  \
  GATVS(z,(TYPE),(COUNT)*(COL),(1<(COL))?2:1,0,TYPE##SIZE);                \
  if(1<(COL)){*AS(z)=(COUNT); *(1+AS(z))=(COL);}             \
  zv=(T*)AV(z);                                              \
if(jt->peekdata)printf("Definition has %lld chains\n",n);  /* crashdebug */ \
  for(i=1;i<n;++i){   /* crashdebug */  \
   k=*e++;  /* initial symbol index */  \
if(jt->peekdata)printf("Processing hashchain %lld\n",i);  /* crashdebug */ \
if(jt->peekdata)printf("Initial symbol index is %lld\n",k);  /* crashdebug */ \
   while(j=k){                         \
if(jt->peekdata)printf("Processing symbol index %llx\n",j);  /* crashdebug */ \
   d=j+jt->sympv;                                            \
if(jt->peekdata)printf("L entry address is %p\n",d);  /* crashdebug */ \
   k=d->next;                                                \
if(jt->peekdata)printf("d->name=%p, d->val=%p\n, SELECT=%d",d->name,d->val,(SELECT));  /* crashdebug */ \
   if((d->name)&&(d->val)&&(SELECT)){                        \
    if(m==*AS(z)){RZ(z=ext(0,z)); zv=(m*(COL))+(T*)AV(z);}   \
if(jt->peekdata)printf("Calling PROCESS\n");  /* crashdebug */ \
    {PROCESS;}                                               \
    ++m;                                                     \
  }}}                                                         \
if(jt->peekdata)printf("End of loop, m=%lld\n",m);  /* crashdebug */ \
  AN(z)=m*(COL);   \
if(jt->peekdata)printf("stored to AN\n");  /* crashdebug */ \
  *AS(z)=m;                                   \
if(jt->peekdata)printf("stored to AS\n");  /* crashdebug */ \
  R z;                                                       \
 }

