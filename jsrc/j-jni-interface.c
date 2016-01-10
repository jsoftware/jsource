#include "j.h"
#include "com_jsoftware_j_android_JInterface.h"
#include "j-jni-interface.h"
#include <strings.h>

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

void _stdcall logcat(char *msg){
	LOGD(msg);
}

int _stdcall GetJavaVM(JavaVM ** pvm, JNIEnv ** penv){
	LOGD("GetJavaVM");
 *pvm = jvm;
//  (*jvm)->AttachCurrentThread(jvm,&local_jnienv,NULL);  // libj thread
 *penv = local_jnienv;
 return 0;
}

void consoleAppend(JNIEnv *env, jobject obj,int type, const char*chars) {
		LOGD("consoleAppend");
	if(outputId == 0) {
  	   jclass the_class = (*env)->GetObjectClass(env,obj);
		outputId = (*env)->GetMethodID(env,the_class,"output","(ILjava/lang/String;)V" );
	}
	if(outputId == 0) {
		LOGD("failed to get the method id for " "output:" "(ILjava/lang/String;)V");
	} else {
		jstring str = (*env)->NewStringUTF(env,chars);
		(*env)->CallVoidMethod(env,obj,outputId,(jint)type,str);
	}
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

JNIEXPORT jint JNICALL Java_com_jsoftware_j_JInterface_callJNative
  (JNIEnv * env, jobject obj, jlong inst, jstring js) {
	LOGD("callJNative");
	J jengine = (J)inst;

	const char *nativeString = (*env)->GetStringUTFChars(env, js, 0);
	int jc = JDo(jengine,(C*)nativeString);
	return (jint) jc;
}

JNIEXPORT void JNICALL Java_com_jsoftware_j_JInterface_destroyJNative
  (JNIEnv *env, jobject obj, jlong inst) {
	LOGD("destroyJNative");
	JFree((J)inst);
	outputId = 0;
}

JNIEXPORT jobject JNICALL Java_com_jsoftware_j_JInterface_getVariableNative
  (JNIEnv *env, jobject obj, jlong inst, jstring jname) {
	LOGD("getVariableNative");
	J jengine = (J)inst;
	const char *name = (*env)->GetStringUTFChars(env, jname, 0);
	long type, rank;
	long *shape;
	char *data;

	JGetM(jengine,name,(I*)&type,(I*)&rank,(I*)&shape,(I*)&data);
//LOGFD("name=%s,type=%l,rank=%l,shapehead=%l,data=%p",name,type,rank,*shape,data);
  free(name);
		return NULL;
 }

void _stdcall outputHandler(J jt,int type, const char* s) {
	LOGD("outputHandler");
		 consoleAppend(local_jnienv,local_baseobj,type,s);
}

JNIEXPORT jstring JNICALL Java_com_jsoftware_j_JInterface_getLocaleNative
  (JNIEnv *env, jobject obj, jlong inst) {
	LOGD("getLocaleNative");
  return (*env)->NewStringUTF(env,inst?(C*)JGetLocale((J)inst):"base");
 }

JNIEXPORT void JNICALL Java_com_jsoftware_j_JInterface_setEnvNative
  (JNIEnv *env, jobject obj, jstring jkey, jstring jval) {
	LOGD("setEnvNative");
  char*key =  (*env)->GetStringUTFChars(env, jkey, 0);
  char*val =  (*env)->GetStringUTFChars(env, jval, 0);
  setenv(key,val,0);
  free(key);
  free(val);
}

#ifdef ANDROID
const char* android_next_ptr = NULL;
const char* __nextLineFromAndroid(
		JNIEnv *env,
		jobject obj) {
	LOGD("extLineFromAndroid");
	jclass the_class = (*env)->GetObjectClass(env,obj);
	jmethodID nextLineId = (*env)->GetMethodID(env,the_class,"nextLine","()Ljava/lang/String;" );
	/* this method blocks via sleep until a line is available */
	jstring res = (jstring) (*env)->CallObjectMethod(env,obj,nextLineId);
	if(android_next_ptr != NULL) {
		free((char*)android_next_ptr);
	}
	android_next_ptr = (*env)->GetStringUTFChars(env, res, 0);
	return android_next_ptr;
}

const char * _stdcall android_next_line() {
	return __nextLineFromAndroid(local_jnienv,local_baseobj);
}

void __quitViaAndroid(
		JNIEnv *env,
		jobject obj) {
	jclass the_class = (*env)->GetObjectClass(env,obj);
	jmethodID quitId = (*env)->GetMethodID(env,the_class,"quit","()V" );
	(*env)->CallVoidMethod(env,obj,quitId);
}

void _stdcall android_quit() {
  __quitViaAndroid(local_jnienv,local_baseobj);
}

#endif

/*
 * Class:     com_jsoftware_j_android_JActivity
 * Method:    initializeJNative
 * Signature: ()V
 */

JNIEXPORT jlong JNICALL Java_com_jsoftware_j_JInterface_initializeJNative
  (JNIEnv * env, jobject obj, jboolean async) {
	LOGD("initializeJNative");
  if (async) local_jnienv = env;
	local_baseobj = (*env)->NewGlobalRef(env,obj);

  (*env)->ExceptionClear(env);
	outputId = 0;
	 J j = JInit();
#ifdef ANDROID
	 void* callbacks[] = {outputHandler,0,android_next_line,0,(void*)SMJAVA};
#else
	 void* callbacks[] = {outputHandler,0,0,0,(void*)SMJAVA};
#endif
	 JSM(j,callbacks);
	return (jlong) j;
}

/*
 *   dorsNative  cmd to j return string
 */

JNIEXPORT jstring JNICALL Java_com_jsoftware_j_JInterface_dorsNative
  (JNIEnv * env, jobject obj, jlong jt0,  jstring cmd) {
  int err=1;
  char inputline[BUFLEN+1];
  J jt = (J)jt0;
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

