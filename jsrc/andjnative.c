#include "j.h"
#include "jeload.h"
#include "com_jsoftware_j_JInterface.h"
#include <stdint.h>
#ifdef MMSC_VER
#pragma warning(disable: 4244)
#else
#include <strings.h>
#endif

#ifdef _WIN32
 #define JDLLNAME "j.dll"
 #define filesepx "\\"
#else
 #define filesepx "/"
 #ifdef __MACH__
  #define JDLLNAME "libj.dylib"
 #else
  #define JDLLNAME "libj.so"
 #endif
#endif

#define LOCALOGTAG "libj"
#ifdef ANDROID
#include <android/log.h>
#define LOGD(msg) __android_log_write(ANDROID_LOG_DEBUG,LOCALOGTAG,msg)
#define LOGFD(...) __android_log_print(ANDROID_LOG_DEBUG,LOCALOGTAG,__VA_ARGS__)
#else
#define LOGD(msg)
#define LOGFD(...)
#endif

typedef struct A_RECORD {
  I k,flag,m,t,c,n,r,s[1];
}* _A;

typedef struct AREP_RECORD {
  I n,t,c,r,s[1];
}* AREP;

static JavaVM *jvm;
static JNIEnv *local_jnienv;
static jclass local_basecls;
static char* next_line_ptr = 0;

static jmethodID inputId = 0;
static jmethodID outputId = 0;
static jmethodID wdId = 0;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
  LOGD("OnLoad");
  jvm = vm;
  if ((*vm)->GetEnv(vm, (void **)&local_jnienv, JNI_VERSION_1_6) != JNI_OK) {
    return -1;
  }

  // Get jclass with env->FindClass.
  // Register methods with env->RegisterNatives.

  return JNI_VERSION_1_6;
}

static int GetJavaVM(JavaVM ** pvm, JNIEnv ** penv)
{
  LOGD("GetJavaVM");
  *pvm = jvm;
//  (*jvm)->AttachCurrentThread(jvm,&local_jnienv,NULL);  // libj thread
  *penv = local_jnienv;
  return 0;
}

static char* javaInput(JNIEnv *env, jclass jcls, const char* chars)
{
  LOGD("javaInput");
  const char *line;
  /* this method blocks via sleep until a line is available */
  jstring str = (*env)->NewStringUTF(env,(chars)?chars:(char*)"");
  jstring res = (jstring) (*env)->CallStaticObjectMethod(env,jcls,inputId,str);
  (*env)->DeleteLocalRef(env,str);
  line = (*env)->GetStringUTFChars(env, res, 0);
  next_line_ptr=realloc(next_line_ptr,1+strlen(line));
  strcpy(next_line_ptr,line);
  (*env)->ReleaseStringUTFChars(env, res, line);
  (*env)->DeleteLocalRef(env,res);
  return next_line_ptr;
}

static void javaOutput(JNIEnv *env, jclass jcls, int type, const char*chars)
{
  LOGD("javaOutput");
  jstring str = (*env)->NewStringUTF(env,chars);
  (*env)->CallStaticVoidMethod(env,jcls,outputId,(jint)type,str);
  (*env)->DeleteLocalRef(env,str);
}

static int javaWd(JNIEnv *env, jclass jcls, int type, A w, A *pz, const char*locale)
{
  LOGD("javaWd");
  int i,j=0,len,rc=0;
  if(wdId == 0) {
    LOGD("failed to get the method id for wd" );
    return 3;
  }

// check argument type
  if (BOX&AT(w)) {
    A* wi= (A*)AV(w);
    for (i=0; i<AN(w); i++)
      if(!(INT&AT(*(wi+i))||LIT&AT(*(wi+i)))) {
        LOGD("argument error for wd box" );
        rc=3;
        break;
      }
  } else if (AN(w) && !(INT&AT(w)||LIT&AT(w))) {
    LOGD("argument error for wd non-box" );
    rc=3;
  }
  if (rc) {
    return rc;
  }

// inta: type shape0 shape1 .... repeat for each inarr element
  int ninarr=(BOX&AT(w))?AN(w):1;
  jclass objcls = (*env)->FindClass(env,"java/lang/Object");
  jobject inarr= (*env)->NewObjectArray(env, ninarr, objcls, 0);
  jobject outarr= (*env)->NewObjectArray(env, 2, objcls, 0);
  (*env)->DeleteLocalRef(env,objcls);
  jintArray inta= (*env)->NewIntArray(env, 3*ninarr);
  jint* pinta  = (*env)->GetIntArrayElements(env, inta, 0);

  A* w1;
  if (BOX&AT(w))
    w1= (A*)AV(w);
  else
    w1= &w;

  for (i=0; i<ninarr; i++) {
    if (LIT&AT(*w1) || 0==AN(*w1)) {
      pinta[3*i] = LIT;
      if (AR(*w1)>1 && AN(*w1)) {
        pinta[3*i+1] = (AS(*w1))[0];
        pinta[3*i+2] = (AS(*w1))[1];
      } else {
        pinta[3*i+1] = -1;
        pinta[3*i+2] = -1;
      }
      jbyteArray bytea= (*env)->NewByteArray(env, AN(*w1));
      jbyte* pbytea = (*env)->GetByteArrayElements(env, bytea, 0);
      memcpy(pbytea, CAV(*w1), AN(*w1));
      (*env)->ReleaseByteArrayElements(env, bytea, pbytea, 0);
      (*env)->SetObjectArrayElement(env, inarr, i, bytea);
      (*env)->DeleteLocalRef(env,bytea);

    } else if (INT&AT(*w1)) {
      pinta[3*i] = INT;
      if (AR(*w1)>1) {
        pinta[3*i+1] = (AS(*w1))[0];
        pinta[3*i+2] = (AS(*w1))[1];
      } else {
        pinta[3*i+1] = -1;
        pinta[3*i+2] = -1;
      }
      jintArray intb= (*env)->NewIntArray(env, AN(*w1));
#if SY_64
      jint *pintb = (*env)->GetIntArrayElements(env, intb, 0);
      for (j=0; j<AN(*w1); j++) pintb[j]=(jint)*(AV(*w1)+j);
      (*env)->ReleaseIntArrayElements(env, intb, pintb, 0);
#else
      (*env)->SetIntArrayRegion(env, intb, 0, AN(*w1), (jint*)AV(*w1));
#endif
      (*env)->SetObjectArrayElement(env, inarr, i, intb);
      (*env)->DeleteLocalRef(env,intb);
    }
    w1++;
  }

  (*env)->ReleaseIntArrayElements(env, inta, pinta, 0);
  jstring slocale = (*env)->NewStringUTF(env,locale);
  rc = (*env)->CallStaticIntMethod(env,jcls,wdId,(jint)type,inta,inarr,outarr,slocale);
  (*env)->DeleteLocalRef(env,inta);
  (*env)->DeleteLocalRef(env,inarr);
  (*env)->DeleteLocalRef(env,slocale);
//  (*env)->ExceptionClear(env);

  if (rc<0) {
    jobject array = (*env)->GetObjectArrayElement(env, outarr, 0);
    jobject inta = (*env)->GetObjectArrayElement(env, outarr, 1);
    if (0==array || 0==inta) {
      if (0==array) LOGD("array null");
      if (0==inta) LOGD("inta null");
      rc=3;
    } else {
      int leni= (*env)->GetArrayLength(env, inta);
      jint *pinta = (*env)->GetIntArrayElements(env, inta, 0);
      I itype= pinta[0];
      I ishape[2];
      ishape[0]= pinta[1];  // -1 if not rank-2
      ishape[1]= pinta[2];
      (*env)->ReleaseIntArrayElements(env, inta, pinta, 0);

      len= (*env)->GetArrayLength(env, array);
      if (itype==LIT) {
        if (ishape[0]==-1) {
          ishape[0]=len;
          *pz=jega(LIT,len,1,ishape);
        } else {
          *pz=jega(LIT,len,2,ishape);
        }
        (*env)->GetByteArrayRegion(env, array, 0, len, (jbyte*)CAV(*pz));
      } else if (itype==INT) {
        if (ishape[0]==-1) {
          ishape[0]=len;
          *pz=jega(INT,len,1,ishape);
        } else {
          *pz=jega(INT,len,2,ishape);
        }
#if SY_64
        jint *parray = (*env)->GetIntArrayElements(env, array, 0);
        for (j=0; j<len; j++) *(AV(*pz)+j)=parray[j];
        (*env)->ReleaseIntArrayElements(env, array, parray, 0);
#else
        (*env)->GetIntArrayRegion(env, array, 0, len, (jint*)AV(*pz));
#endif
      } else {
        LOGD("result not string or integers");
        rc=3;
      }
    }
    (*env)->DeleteLocalRef(env,array);
    (*env)->DeleteLocalRef(env,inta);
  }
  (*env)->DeleteLocalRef(env,outarr);
  return (rc>0)?3:rc;
}

const char * _stdcall inputHandler(J jt,const char* chars)
{
  return javaInput(local_jnienv,local_basecls, chars);
}

void _stdcall outputHandler(J jt,int type, const char* s)
{
  LOGD("outputHandler");
  javaOutput(local_jnienv,local_basecls,type,s);
}

int _stdcall wdHandler(J jt,int type, A w, A *pz)
{
  R javaWd(local_jnienv,local_basecls,type,w,pz,jegetlocale());
}

JNIEXPORT jint JNICALL Java_com_jsoftware_j_JInterface_JDo
(JNIEnv *env, jclass jcls, jstring js)
{
  LOGD("JDo");
  local_jnienv = env;

  const char *nativeString = (*env)->GetStringUTFChars(env, js, 0);
  int jc = jedo((char*)nativeString);
  (*env)->ReleaseStringUTFChars(env, js, nativeString);
  return (jint) jc;
}

JNIEXPORT void JNICALL Java_com_jsoftware_j_JInterface_JFree
(JNIEnv *env, jclass jcls)
{
  LOGD("JFree");
  jefree();
  inputId = 0;
  outputId = 0;
  wdId = 0;
}

JNIEXPORT jstring JNICALL Java_com_jsoftware_j_JInterface_JGetLocale
(JNIEnv *env, jclass jcls)
{
  LOGD("JGetLocale");
  return (*env)->NewStringUTF(env,jegetlocale());
}

JNIEXPORT void JNICALL Java_com_jsoftware_j_JInterface_JSetEnv
(JNIEnv *env, jclass jcls, jstring jkey, jstring jval)
{
  LOGD("JSetEnv");
  const char* key =  (*env)->GetStringUTFChars(env, jkey, 0);
  const char* val =  (*env)->GetStringUTFChars(env, jval, 0);
#ifdef MMSC_VER
  _putenv_s(key,val);
#else
  setenv(key,val,0);
#endif
  (*env)->ReleaseStringUTFChars(env, jkey, key);
  (*env)->ReleaseStringUTFChars(env, jval, val);
}

/*
 * Class:     com_jsoftware_j_JInterface
 * Method:    JInit
 * Signature: (Ljava/lang/String;)J
 */

JNIEXPORT jlong JNICALL Java_com_jsoftware_j_JInterface_JInit
(JNIEnv *env, jclass jcls, jstring libpath)
{
  LOGD("JInit");
  return Java_com_jsoftware_j_JInterface_JInit2(env, jcls, libpath, 0);
}

/*
 * Class:     com_jsoftware_j_JInterface
 * Method:    JInit2
 * Signature: (Ljava/lang/String;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_jsoftware_j_JInterface_JInit2
  (JNIEnv *env, jclass jcls, jstring libpath, jstring libj)
{
  LOGD("JInit2");
  local_jnienv = env;
  local_basecls = (*env)->NewGlobalRef(env,jcls);
  (*env)->ExceptionClear(env);
  if(inputId == 0) {
    inputId = (*env)->GetStaticMethodID(env,jcls,"input","(Ljava/lang/String;)Ljava/lang/String;" );
  }
  if(inputId == 0) {
    LOGD("failed to get the method id for " "input:" "(Ljava/lang/String;)Ljava/lang/String;" );
  }
  if(outputId == 0) {
    outputId = (*env)->GetStaticMethodID(env,jcls,"output","(ILjava/lang/String;)V" );
  }
  if(outputId == 0) {
    LOGD("failed to get the method id for " "output:" "(ILjava/lang/String;)V" );
  }
  if(wdId == 0) {
    wdId = (*env)->GetStaticMethodID(env,jcls,"wd","(I[I[Ljava/lang/Object;[Ljava/lang/Object;Ljava/lang/String;)I" );
  }
  if(wdId == 0) {
    LOGD("failed to get the method id for " "wd:" "(I[I[Ljava/lang/Object;[Ljava/lang/Object;Ljava/lang/String;)I" );
  }
  void* callbacks[] = {outputHandler,wdHandler,(void*)inputHandler,0,(void*)(SMJA|(SMOPTMTH<<8))};  // assume multithreaded
  const char *nativeString = (*env)->GetStringUTFChars(env, libpath, 0);
  const char *nativelibj;
  char *arg;
  if(libj) nativelibj = (*env)->GetStringUTFChars(env, libj, 0);
  if(libj&&strlen(nativelibj))
    arg=malloc(strlen(nativelibj)+1+strlen(nativeString)+1);
  else
    arg=malloc(strlen(JDLLNAME)+1+strlen(nativeString)+1);
  strcpy(arg,nativeString);
  if(strlen(arg))strcat(arg,filesepx);
  strcat(arg,(libj&&strlen(nativelibj))?nativelibj:JDLLNAME);
  jesetpath(arg);
  free(arg);
  (*env)->ReleaseStringUTFChars(env, libpath, nativeString);
  if(libj) (*env)->ReleaseStringUTFChars(env, libj, nativelibj);
  R (jlong)(intptr_t)jeload(callbacks);
}

/*
 *   JDoR  cmd to j return string
 */

JNIEXPORT jstring JNICALL Java_com_jsoftware_j_JInterface_JDoR
(JNIEnv *env, jclass jcls, jstring cmd)
{
  int err=1;
  local_jnienv = env;
  char *inputline;
  jstring jstr;
  const char *nativecmd = (*env)->GetStringUTFChars(env, cmd, 0);
  inputline = malloc(8+strlen(nativecmd)+1);
  strcpy(inputline,"r_jrx_=:");
  strcat(inputline,nativecmd);
  (*env)->ReleaseStringUTFChars(env, cmd, nativecmd);
  jint ret = jedo(inputline);
  (*env)->ExceptionClear(env);
  free(inputline);
  if (!ret) {
    if (!jedo("q_jrx_=:4!:0<'r_jrx_'")) {
      A at=jegeta(6,(char*)"q_jrx_");
      AREP p=(AREP) (sizeof(struct A_RECORD) + (char*)at);
      if ((p->t==4) && (p->r==0)) {
        A r=(0==*(I*)p->s)?jegeta(6,(char*)"r_jrx_"):0;
        AREP p1=(AREP) (sizeof(struct A_RECORD) + (char*)r);
        if ((p1->t==2) && (p1->r<2)) {
          if (p1->r==0) {
            inputline = malloc(2);
            memcpy(inputline,((char*)p1->s),1);
            inputline[1]=0;
            err=0;
          } else {
            inputline = malloc(1+p1->c);
            memcpy(inputline,(sizeof(struct AREP_RECORD)+(char*)p1),p1->c);
            inputline[p1->c]=0;
            err=0;
          }
        }
      }
    }
  }
  if (!err) {
    jstr = (*env)->NewStringUTF(env,inputline);
    free(inputline);
  } else
    jstr = (*env)->NewStringUTF(env,"");
  return jstr;
}

/*
 * Class:     com_jsoftware_j_JInterface
 * Method:    JGetc
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_jsoftware_j_JInterface_JGetc
  (JNIEnv *env, jclass jcls, jstring jname)
{
  int err=1;
  jstring jstr;
  local_jnienv = env;
  const char *name = (*env)->GetStringUTFChars(env, jname, 0);
  char* buf;
  I len;
  A r = jegeta((int)strlen(name),(char*)name);
  AREP p=(AREP)(sizeof(struct A_RECORD) + (char*)r);
  if ((p->t==2)&&(p->r<2)) {
  if (p->r==0) {
    buf=(char*)calloc(1,sizeof(char));
    len = 1;
    memcpy(buf,(char*)p->s,1);
    err=0;
  } else {
    buf=(char*)calloc(p->c,sizeof(char));
    len = p->c;
    memcpy(buf,(char*)(sizeof(struct AREP_RECORD)+(char*)p),p->c);
    err=0;
  }
  }
  if (!err) {
    jstr = (*env)->NewStringUTF(env,buf);
    free(buf);
  } else
    jstr = (*env)->NewStringUTF(env,"");
  return jstr;
}

/*
 * Class:     com_jsoftware_j_JInterface
 * Method:    JSetc
 * Signature: (Ljava/lang/String;Ljava/lang/String;J)V
 */
JNIEXPORT void JNICALL Java_com_jsoftware_j_JInterface_JSetc
  (JNIEnv *env, jclass jcls, jstring jname, jstring jsb, jlong slen)
{
  local_jnienv = env;
  const char *name = (*env)->GetStringUTFChars(env, jname, 0);
  const char *sb = (*env)->GetStringUTFChars(env, jsb, 0);

  int n,hlen,nlen,tlen;

  I hdr[5];
  n=sizeof(I);
  hlen=n*5;

  nlen=(int)strlen(name);

  tlen=n*(1+slen/n);

//  hdr[0]=(4==n) ? 225 : 227;
  hdr[0]=0;
  unsigned char flag[1];
#if defined(BIGENDIAN)
  flag[0]=(4==n) ? 224 : 226;
#else
  flag[0]=(4==n) ? 225 : 227;
#endif
  memcpy(hdr,flag,1);
  hdr[1]=2;
  hdr[3]=1;
  hdr[2]=hdr[4]=slen;

  C* buf=(C*)calloc(hlen+tlen,sizeof(C));
  memcpy(buf,hdr,hlen);
  memcpy(buf+hlen,sb,slen);
  jeseta(nlen,(char*)name,(hlen+tlen),(char*)buf);
  free(buf);

  (*env)->ReleaseStringUTFChars(env, jname, name);
  (*env)->ReleaseStringUTFChars(env, jsb, sb);
  (*env)->ExceptionClear(env);
}
