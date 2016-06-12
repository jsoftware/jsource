/* Copyright 1990-2003, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* For Symbol Tables aka Locales                                           */


#define LOCPATH(g)      ((*AV(g)+jt->sympv)->val )
#define LOCNAME(g)      ((*AV(g)+jt->sympv)->name)


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
  n=AN(w); e=1+AV(w); k=*e;                                  \
  GA(z,(TYPE),(COUNT)*(COL),(1<(COL))?2:1,0);                \
  if(1<(COL)){*AS(z)=(COUNT); *(1+AS(z))=(COL);}             \
  zv=(T*)AV(z);                                              \
  for(i=1;i<n;++i,k=*++e)while(j=k){                         \
   d=j+jt->sympv;                                            \
   k=d->next;                                                \
   if((d->name)&&(SELECT)){                                  \
    if(m==*AS(z)){RZ(z=ext(0,z)); zv=(m*(COL))+(T*)AV(z);}   \
    {PROCESS;}                                               \
    ++m;                                                     \
  }}                                                         \
  AN(z)=m*(COL); *AS(z)=m;                                   \
  R z;                                                       \
 }

