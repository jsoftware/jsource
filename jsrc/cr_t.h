/* Copyright 1990-2009, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* cr.c templates                                                          */

/* template 0 used by the rank operator general cases (monad and dyad)     */
/* requires:                                                               */
/*  VALENCE 1 or 2                                                         */

#if TEMPLATE==0
#if VALENCE==1
#define RCALL   CALL1(f1,yw,fs)
#define RDIRECT (wt&DIRECT)
#define RAC     (1==AC(yw))
#define RFLAG   (!(AFLAG(w)&AFNJA+AFSMM+AFREL))
#define RARG    {if(1<AC(yw))NEWYW; MOVEYW;}
#define RARGX   {if(1<AC(yw)){RZ(yw=ca(yw)); vv=CAV(yw);}}
#else
#define RCALL   CALL2(f2,ya,yw,fs)
#define RDIRECT (at&DIRECT&&wt&DIRECT)
#define RAC     (1==AC(ya)&&1==AC(yw))
#define RFLAG   (!(AFLAG(a)&AFNJA+AFSMM+AFREL)&&!(AFLAG(w)&AFNJA+AFSMM+AFREL))
// Set up y? with the next cell data.  The data might be unchanged from the previous, for the argument
// with the shorter frame (when jj==n we have run out of repeats).  Whenever we have to copy, we first
// check to see if the cell-workarea has been incorporated into a boxed noun; if so, we have to
// reallocate.  We assume that the cell-workarea is not modified by RCALL, because we reuse it in situ
// when a cell is to be repeated.
#define RARG    {++jj; if(!b||jj==n){if(1<AC(ya))NEWYA; MOVEYA;}  \
                       if( b||jj==n){if(1<AC(yw))NEWYW; MOVEYW;} if(jj==n)jj=0;}
// If the use-count in y? has been incremented, it means that y? was incorporated into an indirect
// noun and must not be modified.  In that case, we reallocate it
#define RARGX   {if(1<AC(ya)){RZ(ya=ca(ya)); uu=CAV(ya);}  \
                 if(1<AC(yw)){RZ(yw=ca(yw)); vv=CAV(yw);}}
#endif

// Assignments from cr.c:
// ?r=rank, ?s->shape, ?cr=effective rank, ?f=#frame, ?b=relative flag, for each argument
// ?cn=number of atoms in a cell, ?k=#bytes in a cell, uv point to one cell before aw data
// Allocate y? to hold one cell of ?, with uu,vv pointing to the data of y?
// b means 'w frame is larger'; p=#larger frame; q=#shorter frame; s->larger frame
// mn=#cells in larger frame (& therefore #cells in result); n=# times to repeat each cell
//  from shorter-frame argument

{B cc=1;C*zv;I j=0,jj=0,old;
 // Execute the verb on the first cells.
 if(mn){y0=y=RCALL; RZ(y);}  // normal case, y0=y=result of first cell
 else{I d;
  // empty arg, execution on cell of fills.  Run the verb with debug turned off.
  d=jt->db; jt->db=0; y=RCALL; jt->db=d;
  if(jt->jerr){y=zero; RESETERR;}  // if there is an error, clear it & use 0 as the result
 }
 // yt=type, yr=rank, ys->shape, yn=#atoms k=#bytes  of first-cell result
 yt=AT(y); yr=AR(y); ys=AS(y); yn=AN(y); k=yn*bp(yt);
 // First shot: zip through the cells, laying the results into the output area
 // one by one.  We can do this if the results are direct (i. e. not pointers),
 // or if there are no results at all; and we can continue until we hit an incompatible result-type.
 // With luck this will process the entire input.
 if(!mn||yt&DIRECT&&RFLAG){I zn;
  RARGX; RE(zn=mult(mn,yn));   // Reallocate y? if needed; zn=number of stoms in all result cells (if they stay homogeneous)
  GA(z,yt,zn,p+yr,0L); ICPY(AS(z),s,p); ICPY(p+AS(z),ys,yr);  // allocate output area, move in long frame followed by result-shape
  if(mn){zv=CAV(z); MC(zv,AV(y),k);}   // If there was a first cell, copy it in
  old=jt->tbase+jt->ttop;
  for(j=1;j<mn;++j){   // for each result-cell...
   RARG;    // establish argument cells
   RZ(y=RCALL);  // call the function
   if(yt!=AT(y)||yr!=AR(y)||yr&&ICMP(AS(y),ys,yr))break;  // break if there is a change of cell type/rank/shape
   MC(zv+=k,AV(y),k);   // move the result-cell to the output
   if(cc&&RAC)tpop(old); else cc=0;  // as long as argument cells are not incorporated into boxed nouns, pop the stack after each cell (??)
 }}
 if(j<mn){A q,*x,yz;
  // Here we were not able to build the result in the output area; type/rank/shape changed.
  // We will create a boxed result, boxing each cell, and then open it.  If this works, great.
  jj=j%n;   // jj = #cells we processed before the wreck
  GA(yz,BOX,mn,p,s); x=AAV(yz);   // allocate place for boxed result
  // For each previous result, put it into a box and store the address in the result area
  if(j){
   zv=CAV(z)-k;
   DO(j, GA(q,AT(y0),AN(y0),AR(y0),AS(y0)); MC(AV(q),zv+=k,k); *x++=q;);  // We know the type/shape/rank of y0 matches them all
  }
  *x++=y;   // move in the result that caused the wreck
  DO(mn-j-1, RARG; RZ(y=RCALL); *x++=y;);   // for all the rest, execute the cells and move pointer to output area
  z=ope(yz);  // We have created x <@f y; this creates > x <@f y which is the final result
 }
 EPILOG(z);
}
#undef VALENCE
#undef RAC
#undef RARG
#undef RARGX
#undef RCALL
#undef RDIRECT
#undef RFLAG
#endif  /* TEMPLATE 0 */


#undef TEMPLATE