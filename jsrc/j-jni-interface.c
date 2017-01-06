#include "j.h"
#include "com_jsoftware_j_android_JInterface.h"
#include "j-jni-interface.h"
#include <strings.h>
#include <stdint.h>

extern I jdo(J jt, C* lp);
extern C* getlocale(J jt);

// #define JNIGNULL (*env)->NewGlobalRef(env, NULL)
#define JNIGNULL (void*)0

#define BUFLEN 30000

typedef struct A_RECORD {
  I k,flag,m,t,c,n,r,s[1];
}* _A;

typedef struct AREP_RECORD {
  I n,t,c,r,s[1];
}* AREP;


static JavaVM *jvm;
static JNIEnv *local_jnienv;
static jobject local_baseobj;

jmethodID outputId = 0;
jmethodID wdId = 0;

void _stdcall logcat(char *msg)
{
  LOGD(msg);
}

int _stdcall GetJavaVM(JavaVM ** pvm, JNIEnv ** penv)
{
  LOGD("GetJavaVM");
  *pvm = jvm;
//  (*jvm)->AttachCurrentThread(jvm,&local_jnienv,NULL);  // libj thread
  *penv = local_jnienv;
  return 0;
}

void javaOutput(JNIEnv *env, jobject obj,int type, const char*chars)
{
  LOGD("javaOutput");
  if(outputId == 0) {
    jclass the_class = (*env)->GetObjectClass(env,obj);
    outputId = (*env)->GetMethodID(env,the_class,"output","(ILjava/lang/String;)V" );
    (*env)->DeleteLocalRef(env,the_class);
  }
  if(outputId == 0) {
    LOGD("failed to get the method id for " "output:" "(ILjava/lang/String;)V");
  } else {
    jstring str = (*env)->NewStringUTF(env,chars);
    (*env)->CallVoidMethod(env,obj,outputId,(jint)type,str);
    (*env)->DeleteLocalRef(env,str);
  }
}

int _stdcall wdHandler(J jt,int type, A w, A *pz)
{
  R javaWd(local_jnienv,local_baseobj,jt,type,w,pz,getlocale(jt));
}

int javaWd(JNIEnv *env, jobject obj, J jt,int type, A w, A *pz, const char*locale)
{
  LOGD("javaWd");
  int i,j,len,rc=0;
  if(wdId == 0) {
    jclass the_class = (*env)->GetObjectClass(env,obj);
    wdId = (*env)->GetMethodID(env,the_class,"wd","(I[I[Ljava/lang/Object;[Ljava/lang/Object;Ljava/lang/String;)I" );
    (*env)->DeleteLocalRef(env,the_class);
  }
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
  rc = (*env)->CallIntMethod(env,obj,wdId,(jint)type,inta,inarr,outarr,slocale);
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
          GATV(*pz,LIT,len,1,0);
        } else {
          GATV(*pz,LIT,len,2,ishape);
        }
        (*env)->GetByteArrayRegion(env, array, 0, len, CAV(*pz));
      } else if (itype==INT) {
        if (ishape[0]==-1) {
          GATV(*pz,INT,len,1,0);
        } else {
          GATV(*pz,INT,len,2,ishape);
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

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved)
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

JNIEXPORT jint JNICALL Java_com_jsoftware_j_JInterface_JDo
(JNIEnv * env, jobject obj, jlong inst, jstring js)
{
  LOGD("JDo");
  J jengine = (J)(intptr_t)inst;

  const char *nativeString = (*env)->GetStringUTFChars(env, js, 0);
  int jc = JDo(jengine,(C*)nativeString);
  (*env)->ReleaseStringUTFChars(env, js, nativeString);
  return (jint) jc;
}

JNIEXPORT void JNICALL Java_com_jsoftware_j_JInterface_JFree
(JNIEnv *env, jobject obj, jlong inst)
{
  LOGD("JFree");
  JFree((J)(intptr_t)inst);
  outputId = 0;
  wdId = 0;
}

void _stdcall outputHandler(J jt,int type, const char* s)
{
  LOGD("outputHandler");
  javaOutput(local_jnienv,local_baseobj,type,s);
}

JNIEXPORT jstring JNICALL Java_com_jsoftware_j_JInterface_JGetLocale
(JNIEnv *env, jobject obj, jlong inst)
{
  LOGD("JGetLocale");
  return (*env)->NewStringUTF(env,inst?(C*)JGetLocale((J)(intptr_t)inst):"base");
}

JNIEXPORT void JNICALL Java_com_jsoftware_j_JInterface_JSetEnv
(JNIEnv *env, jobject obj, jstring jkey, jstring jval)
{
  LOGD("JSetEnv");
  const char* key =  (*env)->GetStringUTFChars(env, jkey, 0);
  const char* val =  (*env)->GetStringUTFChars(env, jval, 0);
  setenv(key,val,0);
  (*env)->ReleaseStringUTFChars(env, jkey, key);
  (*env)->ReleaseStringUTFChars(env, jval, val);
}

#ifdef ANDROID
static char* android_next_ptr = NULL;
char* javaInput(
  JNIEnv *env,
  jobject obj)
{
  LOGD("javaInput");
  const char *line;
  jclass the_class = (*env)->GetObjectClass(env,obj);
  jmethodID nextLineId = (*env)->GetMethodID(env,the_class,"nextLine","()Ljava/lang/String;" );
  /* this method blocks via sleep until a line is available */
  jstring res = (jstring) (*env)->CallObjectMethod(env,obj,nextLineId);
  line = (*env)->GetStringUTFChars(env, res, 0);
  android_next_ptr=realloc(android_next_ptr,1+strlen(line));
  strcpy(android_next_ptr,line);
  (*env)->ReleaseStringUTFChars(env, res, line);
  (*env)->DeleteLocalRef(env,res);
  (*env)->DeleteLocalRef(env,the_class);
  return android_next_ptr;
}

const char * _stdcall inputHandler()
{
  return javaInput(local_jnienv,local_baseobj);
}

#endif

/*
 * Class:     com_jsoftware_j_android_JActivity
 * Method:    JInit
 * Signature: ()V
 */

JNIEXPORT jlong JNICALL Java_com_jsoftware_j_JInterface_JInit
(JNIEnv * env, jobject obj, jboolean async)
{
  LOGD("JInit");
  if (async) local_jnienv = env;
  local_baseobj = (*env)->NewGlobalRef(env,obj);
  (*env)->ExceptionClear(env);
  outputId = 0;
  wdId = 0;
  J j = JInit();
#ifdef ANDROID
  void* callbacks[] = {outputHandler,wdHandler,inputHandler,0,(void*)SMWIN};  // don't use SMJAVA
#else
  void* callbacks[] = {outputHandler,0,0,0,(void*)SMJAVA};
#endif
  JSM(j,callbacks);
  return (jlong)(intptr_t)j;
}

/*
 *   JDoR  cmd to j return string
 */

JNIEXPORT jstring JNICALL Java_com_jsoftware_j_JInterface_JDoR
(JNIEnv * env, jobject obj, jlong jt0,  jstring cmd)
{
  int err=1;
  char inputline[BUFLEN+1];
  J jt = (J)(intptr_t)jt0;
  const char *nativecmd = (*env)->GetStringUTFChars(env, cmd, 0);
  if (sizeof(inputline)<8+strlen(nativecmd)) {
    (*env)->ReleaseStringUTFChars(env, cmd, nativecmd);
    (*env)->ExceptionClear(env);
    return (*env)->NewStringUTF(env,"");
  }
  strcpy(inputline,"r_jrx_=:");
  strcat(inputline,nativecmd);
  jint ret = jdo(jt,inputline);
  if (!ret) {
    if (!jdo(jt,(C*)"q_jrx_=:4!:0<'r_jrx_'")) {
      A at=JGetA(jt,6,(char*)"q_jrx_");
      AREP p=(AREP) (sizeof(struct A_RECORD) + (char*)at);
      if ((p->t==4) && (p->r==0)) {
        A r=(0==*(I*)p->s)?JGetA(jt,6,(char*)"r_jrx_"):0;
        AREP p1=(AREP) (sizeof(struct A_RECORD) + (char*)r);
        if ((p1->t==2) && (p1->r<2)) {
          if (p1->r==0) {
            strncpy(inputline,((char*)p1->s),1);
            inputline[1]=0;
            err=0;
          } else {
            if (BUFLEN>=(p1->c)) {
              strncpy(inputline,(sizeof(struct AREP_RECORD)+(char*)p1),p1->c);
              inputline[p1->c]=0;
              err=0;
            }
          }
        }
      }
    }
  }
  (*env)->ReleaseStringUTFChars(env, cmd, nativecmd);
  (*env)->ExceptionClear(env);
  if (!err)
    return (*env)->NewStringUTF(env,inputline);
  else
    return (*env)->NewStringUTF(env,"");
}

