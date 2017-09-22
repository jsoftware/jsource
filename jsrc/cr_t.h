/* Copyright 1990-2009, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* cr.c templates                                                          */

/* template 0 used by the rank operator general cases (monad and dyad)     */
/* requires:                                                               */
/*  VALENCE 1 or 2                                                         */

#if TEMPLATE==0
#if VALENCE==1
#define RDECLS
#define RCALL   CALL1(f1,yw,fs)
#define RDIRECT (wt&DIRECT)
#define RFLAG   (!(AFLAG(w)&AFNJA+AFSMM+AFREL))
#define RARG    {if(WASINCORP1(y,yw)){cc = 0;NEWYW;} MOVEYW;}
#define RARG1   {if(WASINCORP1(y,yw)){RZ(yw=ca(yw)); vv=CAV(yw);}}
#else
#define RDECLS

#endif

// Assignments from cr.c:
// ?r=rank, ?s->shape, ?cr=effective rank, ?f=#frame, ?b=relative flag, for each argument
// ?cn=number of atoms in a cell, ?k=#bytes in a cell, uv point to one cell before aw data
// Allocate y? to hold one cell of ?, with uu,vv pointing to the data of y?
// b means 'w frame is larger'; p=#larger frame; q=#shorter frame; s->larger frame
// mn=#cells in larger frame (& therefore #cells in result); n=# times to repeat each cell
//  from shorter-frame argument

{B cc=1;C*zv;I j=0,jj=0,old;RDECLS;
 if(mn){y0=y=RCALL; RZ(y);}  // if there are cells, execute on the first one
 else{I d;
  // if there are no cells, execute on a cell of fills.  Do this quietly, because
  // if there is an error, we just want to use a value of 0 for the result; thus debug
  // mode off and RESETERR on failure.
  // However, if the error is a non-computational error, like out of memory, it
  // would be wrong to ignore it, because the verb might execute erroneously with no
  // indication that anything unusual happened.  So fail then
  d=jt->db; jt->db=0; y=RCALL; jt->db=d;
  if(jt->jerr){if(EMSK(jt->jerr)&EXIGENTERROR)RZ(y); y=zero; RESETERR;}
 } 

 // yt=type, yr=rank, ys->shape, yn=#atoms k=#bytes of first-cell result
 yt=AT(y); yr=AR(y); ys=AS(y); yn=AN(y); k=yn*bp(yt);
 // First shot: zip through the cells, laying the results into the output area
 // one by one.  We can do this if the results are direct (i. e. not pointers),
 // or if there are no results at all; and we can continue until we hit an incompatible result-type.
 // With luck this will process the entire input.
 if(!mn||yt&DIRECT&&RFLAG){I zn;
  RARG1; RE(zn=mult(mn,yn));   // Reallocate y? if needed; zn=number of atoms in all result cells (if they stay homogeneous)
  GA(z,yt,zn,p+yr,0L); ICPY(AS(z),s,p); ICPY(p+AS(z),ys,yr);  // allocate output area, move in long frame followed by result-shape
  if(mn){zv=CAV(z); MC(zv,AV(y),k);}   // If there was a first cell, copy it in
  // Establish the point we will free to after each call.  This must be after the allocated result area, and
  // after the starting result cell.  After we call the verb we will free up what it allocated, until we have to
  // reallocate the result cell; then we would be wiping out a cell that we ourselves allocated, so we stop
  // freeing then
  old=jt->tnextpushx;
  for(j=1;j<mn;++j){   // for each additional result-cell...
   RARG;    // establish argument cells
   RZ(y=RCALL);  // call the function
   if(TYPESNE(yt,AT(y))||yr!=AR(y)||yr&&ICMP(AS(y),ys,yr))break;  // break if there is a change of cell type/rank/shape
   MC(zv+=k,AV(y),k);   // move the result-cell to the output
   if(cc)tpop(old);  // Now that we have copied to the output area: if we have not reallocated a cell on the stack, free what the verb did
  }
 }
 if(j<mn){A q,*x,yz;
  // Here we were not able to build the result in the output area; type/rank/shape changed.
  // We will create a boxed result, boxing each cell, and then open it.  If this works, great.
  jj=j%n;   // j = #cells we processed before the wreck; jj=position in the smaller cell (not used for monad; compiler should optimize it away)
  GATV(yz,BOX,mn,p,s); x=AAV(yz);   // allocate place for boxed result
  // For each previous result, put it into a box and store the address in the result area
  if(j){
   zv=CAV(z)-k;
   DO(j, GA(q,AT(y0),AN(y0),AR(y0),AS(y0)); MC(AV(q),zv+=k,k); *x++=q;);  // We know the type/shape/rank of y0 matches them all
  }
  *x++=y;   // move in the result that caused the wreck
  DO(mn-j-1, RARG; RZ(y=RCALL); *x++=y;);   // for all the rest, execute the cells and move pointer to output area
  z=ope(yz);  // We have created x <@f y; this creates > x <@f y which is the final result
 }
 EPILOG(z);  // If the result is boxed, we know we had no wastage at this level except for yz, which is small compared to z
}
#undef VALENCE
#undef RARG
#undef RARG1
#undef RCALL
#undef RDIRECT
#undef RFLAG
#undef RDECLS
#endif  /* TEMPLATE 0 */


#undef TEMPLATE
