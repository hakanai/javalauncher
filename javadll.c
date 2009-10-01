
/*
 * Implementation of dynamically loaded JVM.
 */

// Why do Microsoft have to make new names for functions which already exist?
#if defined(WIN32)
#include <windows.h>
#define dlopen(libname,flags) LoadLibrary(libname)
#define dlsym(lib,fnname) GetProcAddress((HINSTANCE)lib,fnname)
#define dlclose(lib) FreeLibrary((HINSTANCE)lib)
#else
#include <dlfcn.h>
#endif

#include <jni.h>
#include <stdlib.h>

#include "javadll.h"


#ifdef __APPLE__
#  define CREATE_JVM_FUNCTION_NAME "JNI_CreateJavaVM_Impl"
#else
#  define CREATE_JVM_FUNCTION_NAME "JNI_CreateJavaVM"
#endif

// Type of the pointer to the JNI_CreateJavaVM function.
typedef jint (*P_JNI_CreateJavaVM)(JavaVM **pvm, JNIEnv ** penv, void *args);

// Pointer to the JNI_CreateJavaVM function.
P_JNI_CreateJavaVM pfnCreateJavaVM = NULL;

// Pointer to the JNI library.
void *hLib = NULL;


/**
 * Launch Java.
 */
void LaunchJavaVM(int nvmOptions, char **vmOptions,
                  char *mainclass, char *mainmethod,
                  int nappOptions, char **appOptions)
{
    // Set up the Java init args from the vm options.
    JavaVMOption *jvmOptions = (JavaVMOption *)calloc(nvmOptions, sizeof(JavaVMOption));
    for (int i = 0; i < nvmOptions; i++) {
        jvmOptions[i].optionString = vmOptions[i];
    }
    JavaVMInitArgs vm_args;
    vm_args.version = JNI_VERSION_1_2;
    vm_args.options = jvmOptions;
    vm_args.nOptions = nvmOptions;
    vm_args.ignoreUnrecognized = JNI_TRUE;

    // Create the Java VM.
    JavaVM *jvm;
    JNIEnv *env;
    jint res = (*pfnCreateJavaVM)(&jvm, &env, &vm_args);
    if (res < 0) {
        fprintf(stderr, "Can't create the Java VM, exiting.\n");
        exit(-1);
    }

    // Find the class...
    jclass cls = (*env)->FindClass(env, mainclass);
    if (cls == 0) {
        fprintf(stderr, "Can't find class: %s\n", mainclass);
        exit(-1);
    }

    // Find the main method...
    jmethodID mid = (*env)->GetStaticMethodID(env, cls, mainmethod, "([Ljava/lang/String;)V");
    if (mid == 0) {
        fprintf(stderr, "Can't find method: %s.%s\n", mainclass, mainmethod);
        exit(-1);
    }

    // Copy the application arguments into a Java string array.
    jstring jstr = (*env)->NewStringUTF(env, "");
    jobjectArray args = (*env)->NewObjectArray(env, nappOptions, (*env)->FindClass(env, "java/lang/String"), jstr);
    for (int i = 0; i < nappOptions; i++) {
        jstr = (*env)->NewStringUTF(env, appOptions[i]);
        (*env)->SetObjectArrayElement(env, args, i, jstr);
    }

    // Pass the array to the main method.
    (*env)->CallStaticVoidMethod(env, cls, mid, args);

    // Check to see if the termination was due to an unexpected exception.
    if ((*env)->ExceptionOccurred(env)) {
        (*env)->ExceptionDescribe(env);
    }

    // Destroy the Java VM.
    (*jvm)->DestroyJavaVM(jvm);
}

/**
 * Unload the Java DLL.
 */
void UnloadJavaDLL()
{
    dlclose(hLib);
    hLib = NULL;
}

/**
 * Load the Java DLL.
 */
void LoadJavaDLL(char *dllPath)
{
    // If the library is already loaded, unload it first.
    if (hLib != NULL) {
        UnloadJavaDLL();
    }

    // Load the library.
    hLib = dlopen(dllPath, 0);
    if (hLib == NULL) {
        printf("Unable to load library %s, exiting.\n", dllPath);
        exit(-1);
    }

    // Set up the required functions.
    pfnCreateJavaVM = (P_JNI_CreateJavaVM)dlsym(hLib, CREATE_JVM_FUNCTION_NAME);
    if (pfnCreateJavaVM == NULL) {
        fprintf(stderr, "Can't find CreateJavaVM function, exiting.\n");
        exit(-1);
    }
}
