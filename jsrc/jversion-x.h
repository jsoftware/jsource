#define jversion  "804" 
#ifdef _WIN32
#define jplatform "windows"  // windows/linux/darwin/raspberry/android/...
#elif defined(RASPI)
#define jplatform "raspberry"
#elif defined(ANDROID)
#define jplatform "android"
#elif defined(__MACH__)
#define jplatform "darwin"
#else
#define jplatform "linux"
#endif
#define jtype     "release"     // release,beta,... may include bug level such as beta-b,release-c
#define jlicense  "commercial"
#define jbuilder  "www.jsoftware.com"  // website or email
