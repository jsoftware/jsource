typedef long long I;
typedef double D;
typedef float F;

void fii(I,I);
void fiiii(I,I,I,I);
void fiiiiii(I,I,I,I,I,I);
void fiiiiiiii(I,I,I,I,I,I,I,I);
void fid(I,D);
void fidid(I,D,I,D);
void fididid(I,D,I,D,I,D);
void fid9(I,D,I,D,I,D,I,D,I,D,I,D,I,D,I,D,I,D);
void fdddd(D,D,D,D);
void fd8 (D,D,D,D,D,D,D,D);
void fd12(D,D,D,D,D,D,D,D,D,D,D,D,D*);
void ff2(F,F);
void fd2(D,D);

I a=1,b=2,c=3,d=4,e=5,f=6,g=7,h=8,i=9,j=10,k=11,l=12,m=13,n=14,o=15,p=16,q=17;
D da=1,db=2,dc=3,dd=4,de=5,df=6,dg=7,dh=8,di=9,dj=10,dk=11,dl=12,dm=12,dn=13,dox=14,dp=15,dq=16,dr=17;
F fa=1,fb=2;
D ddata[12];

void test()
{
    fii(1,2);
    fiiii(1,2,3,4);
    fiiiiii(1,2,3,4,5,6);
    fiiiiiiii(a,b,c,d,e,f,g,h);
    fid(1,2.2);
    fidid(1,2.2,3,4.4);
    fididid(a,db,c,dd,e,df);
    fid9(a,db,c,dd,e,df,g,dh,i,dj,k,dl,m,dn,o,dp,q,dr);
    fdddd(1.1,2.2,3.3,4.4);
    fd8(9.1,9.2,9.3,9.4,9.5,9.6,9.7,9.8);
    fd12(da,db,dc,dd,de,df,dg,dh,di,dj,dk,dl,ddata);
    ff2(da,db);
    fd2(da,db);
}

D foo(D a,D b,D c,D d,D e,D f,D g,D h,D i,D j,D k, D l)
{
    return a+b+c+d+e+f+g+h+i+j+k+l;
}
