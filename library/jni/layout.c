#include <string.h>
#include "layout.h"
#include <stdio.h>
#include <unistd.h>
#include <assert.h>




#define ALLOC_DEFAULT  0x00
#define ALLOC_DONT_TRACK 0x02

JavaVM* gJavaVM;
extern JNIEnv* jni_env;
extern ClassObject* classJavaLangObjectArray;
extern jclass NPEClazz;
extern jclass CastEClazz;
extern jmethodID jInvokeMethod;
extern jmethodID jClassMethod;



static void showMethodInfo(const Method* method)
{
	ALOGD("accessFlags:%d",method->accessFlags);
	ALOGD("clazz->descriptor:%s",method->clazz->descriptor);
	ALOGD("clazz->sourceFile:%s",method->clazz->sourceFile);
	ALOGD("methodIndex:%d",method->methodIndex);
	ALOGD("name:%s",method->name);
	ALOGD("shorty:%s",method->shorty);
}

JNIEXPORT void JNICALL
Java_com_github_mmin18_layoutcast_util_NDKUtil_hook(JNIEnv *env, jclass instance, jobject clazzToHook,
                                                jobject src, jobject dest) {

//		dalvik_setup(env,12);
		jobject clazz = (*env)->CallObjectMethod(env,dest, jClassMethod);
		ClassObject* clz = (ClassObject*) dvmDecodeIndirectRef_fnPtr(
				dvmThreadSelf_fnPtr(), clazz);
		clz->status = CLASS_INITIALIZED;

		Method* meth = (Method*) (*env)->FromReflectedMethod(env,src);
		Method* target = (Method*) (*env)->FromReflectedMethod(env,dest);
		ALOGD("dalvikMethod: %s", meth->name);

		meth->jniArgInfo = 0x80000000;
		meth->accessFlags |= ACC_NATIVE;

		int argsSize = dvmComputeMethodArgsSize_fnPtr(meth);
		if (!dvmIsStaticMethod_fnPtr(meth))
			argsSize++;
		meth->registersSize = meth->insSize = argsSize;
		meth->insns = (void*) target;
		ALOGD("set native func");
		meth->nativeFunc = dalvik_dispatcher;
}

JNIEXPORT void JNICALL
Java_com_github_mmin18_layoutcast_util_NDKUtil_resolvDvm(JNIEnv *env, jclass instance,jint apilevel) {
	resolv_dvm(env,(int)apilevel);
}


void
Java_com_github_mmin18_layoutcast_util_NDKUtil_customClassLoader(JNIEnv*
 pEnv, jclass pObj,jobject classloader,jstring dexPath,jstring optpath)
{
	jclass clDexClassLoader = (*pEnv)->FindClass(pEnv,"dalvik/system/BaseDexClassLoader");
    ALOGD("clDexClassLoader = %p\n", clDexClassLoader);
	jclass elementclass = (*pEnv)->FindClass(pEnv, "dalvik/system/DexPathList$Element");

	//Get PathList
	jfieldID mPathId = (*pEnv)->GetFieldID(pEnv,clDexClassLoader,"pathList","Ldalvik/system/DexPathList;");
	ALOGD("mPathId = %p\n", mPathId);
	jobject mPathList = (*pEnv)->GetObjectField(pEnv,classloader,mPathId);
    ALOGD("mPathList = %p\n", mPathList);

	//Get dexElement
	jclass pathlisClass = (*pEnv)->FindClass(pEnv, "dalvik/system/DexPathList");
	ALOGD("pathlisClass = %p\n", mPathList);
	jfieldID mElementsId = (*pEnv)->GetFieldID(pEnv,pathlisClass,"dexElements","[Ldalvik/system/DexPathList$Element;");
	ALOGD("mElementsId = %p\n", mPathList);
	jobjectArray dexElements = (jobjectArray)(*pEnv)->GetObjectField(pEnv,mPathList,mElementsId);
	ALOGD("dexElements = %p\n", dexElements);
	//Get new dex element

	jmethodID mClassLoaderConstructor = (*pEnv)->GetMethodID(pEnv,clDexClassLoader, "<init>", "(Ljava/lang/String;Ljava/io/File;Ljava/lang/String;Ljava/lang/ClassLoader;)V");
	ALOGD("mClassLoaderConstructor = %p\n", mClassLoaderConstructor);
	jmethodID mGetSystemClassLoader = (*pEnv)->GetStaticMethodID(pEnv,clDexClassLoader, "getSystemClassLoader", "()Ljava/lang/ClassLoader;");
	ALOGD("mGetSystemClassLoader = %p\n", mGetSystemClassLoader);
	jint count = (*pEnv)->GetArrayLength(pEnv,dexElements);

	if(mClassLoaderConstructor && mGetSystemClassLoader){
//		jobject classloaderobj = (*pEnv)->NewObject( clDexClassLoader, mClassLoaderConstructor,
//						(*pEnv)->NewStringUTF( dexPath), NULL,(*pEnv)->NewStringUTF(pEnv,optpath),
//						classloader);
		jobject classloaderobj = (*pEnv)->NewObject(pEnv,clDexClassLoader, mClassLoaderConstructor,
								dexPath, NULL,optpath,
								classloader);
		jobject path2 = (*pEnv)->GetObjectField(pEnv,classloaderobj,mPathId);
		jobjectArray dexElements2 = (jobjectArray)(*pEnv)->GetObjectField(pEnv,path2,mElementsId);
		jint count2 = (*pEnv)->GetArrayLength(pEnv,dexElements2);
		if(count2 > 0){
			ALOGD("count for new ClassLoader is not zero");
			jobject element2 = (*pEnv)->GetObjectArrayElement(pEnv,dexElements2,0);
			jint i;
			jclass elementclass = (*pEnv)->FindClass(pEnv,"Ldalvik/system/DexPathList$Element");
			jobjectArray newElements =(*pEnv)->NewObjectArray(pEnv,count + 1,elementclass,NULL);
			(*pEnv)->SetObjectArrayElement(pEnv,newElements,0,element2);
			for(i=1 ; i < count + 1;i++){
				(*pEnv)->SetObjectArrayElement(pEnv,newElements,i,(*pEnv)->GetObjectArrayElement(pEnv,dexElements,i-1));
			}
			(*pEnv)->SetObjectField(pEnv,mPathList,mElementsId,newElements);

		}
		else{
			ALOGD("count for new ClassLoader is  zero");

		}

	}
}



jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	JNIEnv* env = NULL;
	jint result = -1;

	if ((*vm)->GetEnv(vm,(void**) &env, JNI_VERSION_1_4) != JNI_OK) {
		ALOGE("Failed to get the environment using GetEnv()");
		return -1;
	}
	assert(env != NULL);

//	if (!registerNatives(env)) { //注册
//		return -1;
//	}
	/* success -- return valid version number */
	result = JNI_VERSION_1_4;

	return result;

}

static void dalvik_dispatcher(const u4* args, jvalue* pResult,
		const Method* method, void* self) {
	ALOGD("begin set native func");

	ClassObject* returnType;
	jvalue result;
	ArrayObject* argArray;

	ALOGD("dalvik_dispatcher source method: %s %s", method->name,
			method->shorty);
	Method* meth = (Method*) method->insns;
	meth->accessFlags = meth->accessFlags | ACC_PUBLIC;
	ALOGD("dalvik_dispatcher target method: %s %s", method->name,
			method->shorty);

	returnType = dvmGetBoxedReturnType_fnPtr(method);
	if (returnType == NULL) {
		assert(dvmCheckException_fnPtr(self));
		goto bail;
	}
	ALOGD("dalvik_dispatcher start call->");

	if (!dvmIsStaticMethod_fnPtr(meth)) {
		Object* thisObj = (Object*) args[0];
		ClassObject* tmp = thisObj->clazz;
		thisObj->clazz = meth->clazz;
		argArray = boxMethodArgs(meth, args + 1);
		if (dvmCheckException_fnPtr(self))
			goto bail;

		dvmCallMethod_fnPtr(self, (Method*) jInvokeMethod,
				dvmCreateReflectMethodObject_fnPtr(meth), &result, thisObj,
				argArray);

		thisObj->clazz = tmp;
	} else {
		argArray = boxMethodArgs(meth, args);
		if (dvmCheckException_fnPtr(self))
			goto bail;

		dvmCallMethod_fnPtr(self, (Method*) jInvokeMethod,
				dvmCreateReflectMethodObject_fnPtr(meth), &result, NULL,
				argArray);
	}
	if (dvmCheckException_fnPtr(self)) {
		Object* excep = dvmGetException_fnPtr(self);
		(*jni_env)->Throw(jni_env,(jthrowable) excep);
		goto bail;
	}

	if (returnType->primitiveType == PRIM_VOID) {
		ALOGD("+++ ignoring return to void");
	} else if (result.l == NULL) {
		if (dvmIsPrimitiveClass(returnType)) {
			(*jni_env)->ThrowNew(jni_env,NPEClazz, "null result when primitive expected");
			goto bail;
		}
		pResult->l = NULL;
	} else {
		if (!dvmUnboxPrimitive_fnPtr(result.l, returnType, pResult)) {
			char msg[1024] = { 0 };
			snprintf(msg, sizeof(msg) - 1, "%s!=%s\0",
					((Object*) result.l)->clazz->descriptor,
					returnType->descriptor);
			(*jni_env)->ThrowNew(jni_env,CastEClazz, msg);
			goto bail;
		}
	}

	bail: dvmReleaseTrackedAlloc_fnPtr((Object*) argArray, self);
}


