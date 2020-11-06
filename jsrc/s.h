/* Copyright 1990-2003, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* For Symbol Tables aka Locales                                           */

// The first L block in a symbol table is used to point to the path and the locale-name rather than hash chains
#define LOCPATH(g) ((LAV0(jt->symp))[LXAV0(g)[SYMLINFO]].val)
#define LOCNAME(g) ((LAV0(jt->symp))[LXAV0(g)[SYMLINFO]].name)


/* macro to define a function that walks through a symbol table */
/* f - name of derived function                                 */
/* T       - for z, C datatype                                  */
/* TYPE    - for z, J datatype                                  */
/* COUNT   - for z, # of initial items for z                    */
/* COL     - for z, # columns                                   */
/* SELECT  - selection function on a name                       */
/* PROCESS - processing on a selected name                      */

#define SYMWALK(f,T,TYPE,COUNT,COL,SELECT,PROCESS)  \
 F1(f){A z;LX *e,j,k;I i,m=0,n;L*d;T*zv;                         \
  ARGCHK1(w);                                                     \
  n=AN(w); e=SYMLINFOSIZE+LXAV0(w);                                  \
  GATVS(z,(TYPE),(COUNT)*(COL),(1<(COL))?2:1,0,TYPE##SIZE,GACOPYSHAPE0,R 0);                \
  if(1<(COL)){AS(z)[0]=(COUNT); AS(z)[1]=(COL);}             \
  zv=(T*)AV(z);                                              \
  for(i=1;i<n;++i){                  \
   k=*e++;  /* initial symbol index */  \
   while(j=k){                         \
   d=j+LAV0(jt->symp);                                            \
   k=d->next;                                                \
   if((d->name)&&(d->val)&&(SELECT)){                        \
    if(m==AS(z)[0]){RZ(z=ext(0,z)); zv=(m*(COL))+(T*)AV(z);}   \
    {PROCESS;}                                               \
    ++m;                                                     \
  }}}                                                         \
  AN(z)=m*(COL);   \
  AS(z)[0]=m;                                   \
  R z;                                                       \
 }

