/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* For Symbol Tables aka Locales                                           */


/* macro to define a function that walks through a symbol table */
/* f - name of derived function                                 */
/* T       - for z, C datatype                                  */
/* TYPE    - for z, J datatype                                  */
/* COUNT   - for z, # of initial items for z                    */
/* COL     - for z, # columns                                   */
/* SELECT  - selection function on a name                       */
/* PROCESS - processing on a selected name                      */

// The created function is called as f(jt,a,w).  Within SELECT and PROCESS, L* d is the symbol being processed

#define SYMWALK(f,T,TYPE,COUNT,COL,SELECT,PROCESS)  \
 F2(f){A z;LX *e,j,k;I i,m=0,n;L*d;T*zv;                         \
  ARGCHK1(w);                                                     \
  n=AN(w); e=SYMLINFOSIZE+LXAV0(w);                                  \
  GATVS(z,(TYPE),(COUNT)*(COL),(1<(COL))?2:1,0,TYPE##SIZE,GACOPYSHAPE0,R 0);                \
  if(1<(COL)){AS(z)[0]=(COUNT); AS(z)[1]=(COL);}             \
  zv=(T*)AV(z);                                              \
  for(i=SYMLINFOSIZE;i<n;++i){                  \
   k=SYMNEXT(*e++);  /* initial symbol index */  \
   while(j=k){                         \
   d=j+SYMORIGIN;                                            \
   k=SYMNEXT(d->next);                                                \
   if((d->name)&&(d->val)&&(SELECT)){                        \
    if(m==AS(z)[0]){RZ(z=ext(0,z)); zv=(m*(COL))+(T*)AV(z);}   \
    {PROCESS;}                                               \
    ++m;                                                     \
  }}}                                                         \
  AN(z)=m*(COL);   \
  AS(z)[0]=m;                                   \
  R z;                                                       \
 }

