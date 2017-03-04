// This version does the minimum number of compares in the grade section.  To be used when compares are costly

// Inputs:
// GRADEFNNAME - name of the main entry point, normally jmsortxxx
// MERGEFNNAME - name of the merge entry point, normally jmergexxx
// COMPFN - function to call to compare
// 


// sort the values in *in, using *wk as a work area of the same size.  The graded pointers will go into either
// in or wk, and the result will be the address of the graded data
static void** GRADEFNNAME(CMP comp, I compn, void *(in[]), I n, void *(wk[])){I a,b,c,d;
 switch(n){
  case 2: 
   if(VS2(0,1))XC2(0,1); 
  case 0: case 1:
   R in;
  case 3:
   if(VS2(0,1))XC2(0,1);
   if(VS2(1,2))if(VS2(0,2))P32(2,0,1) else XC2(1,2);
   R in;
  case 4:
   if(VS2(0,1))XC2(0,1);
   if(VS2(2,3))XC2(2,3);
   if(VS2(1,3)){XC2(0,2); XC2(1,3);}
   if(VS2(1,2))if(VS2(0,2))P32(2,0,1) else XC2(1,2);
   R in;
  case 5: 
   if(VS2(0,1))XC2(0,1);
   if(VS2(2,3))XC2(2,3);
   if(VS2(1,3)){XC2(0,2); XC2(1,3);}
   if(VS2(4,1))if(VS2(4,3)){a=0; b=1; c=3; d=4;}else{a=0; b=1; c=4; d=3;}
   else       if(VS2(4,0)){a=0; b=4; c=1; d=3;}else{a=4; b=0; c=1; d=3;}
   if(VS2(2,b)){if(3!=c)if(VS2(2,c))P52(a,b,c,2,d) else P52(a,b,2,c,d);}
   else       {        if(VS2(2,a))P52(a,2,b,c,d) else P52(2,a,b,c,d);}
   R in;
 default:
  // sort the low and high halves, and then merge the results, giving as workarea whatever buffer does not contain lo
  {I lohalf=n>>1; I hihalf=n-lohalf;
   void *lo=GRADEFNNAME(comp, compn, in, lohalf, wk); void *hi=GRADEFNNAME(comp, compn, in+lohalf, hihalf, wk+lohalf);
   R MERGEFNNAME(comp,compn,lo,lohalf,hi,hihalf,(void *)((I)in+(I)wk-(I)lo));
  }
 }
}

#undef COMPFN
#undef MERGEFNNAME
#undef GRADEFNNAME
