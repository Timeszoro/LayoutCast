
#include <time.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>

#include <stdbool.h>
#include <fcntl.h>
#include <dlfcn.h>

#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <utime.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "dalvik_base.h"
#include "common.h"


 JNIEnv* jni_env;
 ClassObject* classJavaLangObjectArray;
 jclass NPEClazz;
 jclass CastEClazz;
 jmethodID jInvokeMethod;
 jmethodID jClassMethod;

static void* dvm_dlsym(void *hand, const char *name) {
	void* ret = dlsym(hand, name);
	char msg[1024] = { 0 };
	snprintf(msg, sizeof(msg) - 1, "0x%x", ret);
	ALOGD("%s = %s\n", name, msg);
	return ret;
}

 jboolean resolv_dvm(JNIEnv* env,int apilevel) {
	ALOGD("init dalvik vm");
	jni_env = env;
	void* dvm_hand = dlopen("libdvm.so", RTLD_NOW);
	if (dvm_hand) {
		dvmStringFromCStr_fnPtr = dvm_dlsym(dvm_hand,"_Z32dvmCreateStringFromCstrAndLengthPKcj");
		dvmIsStaticMethod_fnPtr = dvm_dlsym(dvm_hand, "_Z17dvmIsStaticMethodPK6Method");
		dvmComputeMethodArgsSize_fnPtr = dvm_dlsym(dvm_hand,
				apilevel > 10 ?
						"_Z24dvmComputeMethodArgsSizePK6Method" :
						"dvmComputeMethodArgsSize");
		if (!dvmComputeMethodArgsSize_fnPtr) {
			throwNPE(env, "dvmComputeMethodArgsSize_fnPtr");
			return JNI_FALSE;
		}
		dvmCallMethod_fnPtr = dvm_dlsym(dvm_hand,
				apilevel > 10 ?
						"_Z13dvmCallMethodP6ThreadPK6MethodP6ObjectP6JValuez" :
						"dvmCallMethod");
		if (!dvmCallMethod_fnPtr) {
			throwNPE(env, "dvmCallMethod_fnPtr");
			return JNI_FALSE;
		}
		dexProtoGetParameterCount_fnPtr = dvm_dlsym(dvm_hand,
				apilevel > 10 ?
						"_Z25dexProtoGetParameterCountPK8DexProto" :
						"dexProtoGetParameterCount");
		if (!dexProtoGetParameterCount_fnPtr) {
			throwNPE(env, "dexProtoGetParameterCount_fnPtr");
			return JNI_FALSE;
		}

		dvmAllocArrayByClass_fnPtr = dvm_dlsym(dvm_hand,
				"dvmAllocArrayByClass");
		if (!dvmAllocArrayByClass_fnPtr) {
			throwNPE(env, "dvmAllocArrayByClass_fnPtr");
			return JNI_FALSE;
		}
		dvmBoxPrimitive_fnPtr = dvm_dlsym(dvm_hand,
				apilevel > 10 ?
						"_Z15dvmBoxPrimitive6JValueP11ClassObject" :
						"dvmWrapPrimitive");
		if (!dvmBoxPrimitive_fnPtr) {
			throwNPE(env, "dvmBoxPrimitive_fnPtr");
			return JNI_FALSE;
		}
		dvmFindPrimitiveClass_fnPtr = dvm_dlsym(dvm_hand,
				apilevel > 10 ?
						"_Z21dvmFindPrimitiveClassc" : "dvmFindPrimitiveClass");
		if (!dvmFindPrimitiveClass_fnPtr) {
			throwNPE(env, "dvmFindPrimitiveClass_fnPtr");
			return JNI_FALSE;
		}
		dvmReleaseTrackedAlloc_fnPtr = dvm_dlsym(dvm_hand,
				"dvmReleaseTrackedAlloc");
		if (!dvmReleaseTrackedAlloc_fnPtr) {
			throwNPE(env, "dvmReleaseTrackedAlloc_fnPtr");
			return JNI_FALSE;
		}
		dvmCheckException_fnPtr = dvm_dlsym(dvm_hand,
				apilevel > 10 ?
						"_Z17dvmCheckExceptionP6Thread" : "dvmCheckException");
		if (!dvmCheckException_fnPtr) {
			throwNPE(env, "dvmCheckException_fnPtr");
			return JNI_FALSE;
		}

		dvmGetException_fnPtr = dvm_dlsym(dvm_hand,
				apilevel > 10 ?
						"_Z15dvmGetExceptionP6Thread" : "dvmGetException");
		if (!dvmGetException_fnPtr) {
			throwNPE(env, "dvmGetException_fnPtr");
			return JNI_FALSE;
		}
		dvmFindArrayClass_fnPtr = dvm_dlsym(dvm_hand,
				apilevel > 10 ?
						"_Z17dvmFindArrayClassPKcP6Object" :
						"dvmFindArrayClass");
		if (!dvmFindArrayClass_fnPtr) {
			throwNPE(env, "dvmFindArrayClass_fnPtr");
			return JNI_FALSE;
		}
		dvmCreateReflectMethodObject_fnPtr = dvm_dlsym(dvm_hand,
				apilevel > 10 ?
						"_Z28dvmCreateReflectMethodObjectPK6Method" :
						"dvmCreateReflectMethodObject");
		if (!dvmCreateReflectMethodObject_fnPtr) {
			throwNPE(env, "dvmCreateReflectMethodObject_fnPtr");
			return JNI_FALSE;
		}

		dvmGetBoxedReturnType_fnPtr = dvm_dlsym(dvm_hand,
				apilevel > 10 ?
						"_Z21dvmGetBoxedReturnTypePK6Method" :
						"dvmGetBoxedReturnType");
		if (!dvmGetBoxedReturnType_fnPtr) {
			throwNPE(env, "dvmGetBoxedReturnType_fnPtr");
			return JNI_FALSE;
		}
		dvmUnboxPrimitive_fnPtr = dvm_dlsym(dvm_hand,
				apilevel > 10 ?
						"_Z17dvmUnboxPrimitiveP6ObjectP11ClassObjectP6JValue" :
						"dvmUnwrapPrimitive");
		if (!dvmUnboxPrimitive_fnPtr) {
			throwNPE(env, "dvmUnboxPrimitive_fnPtr");
			return JNI_FALSE;
		}
		dvmDecodeIndirectRef_fnPtr = dvm_dlsym(dvm_hand,
				apilevel > 10 ?
						"_Z20dvmDecodeIndirectRefP6ThreadP8_jobject" :
						"dvmDecodeIndirectRef");
		if (!dvmDecodeIndirectRef_fnPtr) {
			throwNPE(env, "dvmDecodeIndirectRef_fnPtr");
			return JNI_FALSE;
		}
		dvmThreadSelf_fnPtr = dvm_dlsym(dvm_hand,
				apilevel > 10 ? "_Z13dvmThreadSelfv" : "dvmThreadSelf");
		if (!dvmThreadSelf_fnPtr) {
			throwNPE(env, "dvmThreadSelf_fnPtr");
			return JNI_FALSE;
		}

		classJavaLangObjectArray = dvmFindArrayClass_fnPtr(
				"[Ljava/lang/Object;", NULL);
		jclass clazz = (*env)->FindClass(env,"java/lang/reflect/Method");
		jInvokeMethod = (*env)->GetMethodID(env,clazz, "invoke",
				"(Ljava/lang/Object;[Ljava/lang/Object;)Ljava/lang/Object;");
		jClassMethod = (*env)->GetMethodID(env,clazz, "getDeclaringClass",
				"()Ljava/lang/Class;");
		NPEClazz = (*env)->FindClass(env,"java/lang/NullPointerException");
		CastEClazz = (*env)->FindClass(env,"java/lang/ClassCastException");
		return JNI_TRUE;
	} else {
		return JNI_FALSE;
	}
}

 bool dvmIsStaticMethod(const Method* method) {
	return (method->accessFlags & ACC_STATIC) != 0;
}


static void throwNPE(JNIEnv* env, const char* msg) {
	ALOGE("setup error: %s", msg);
//	env->ThrowNew(NPEClazz, msg);
}


static s8 dvmGetArgLong(const u4* args, int elem) {
	s8 val;
	memcpy(&val, &args[elem], sizeof(val));
	return val;
}

 ArrayObject* boxMethodArgs(const Method* method, const u4* args) {
	const char* desc = &method->shorty[1]; // [0] is the return type.

	/* count args */
	size_t argCount = dexProtoGetParameterCount_fnPtr(&method->prototype);

	/* allocate storage */
	ArrayObject* argArray = dvmAllocArrayByClass_fnPtr(classJavaLangObjectArray,
			argCount, ALLOC_DEFAULT);
	if (argArray == NULL)
		return NULL;
	Object** argObjects = (Object**) (void*) argArray->contents;

	/*
	 * Fill in the array.
	 */

	size_t srcIndex = 0;
	size_t dstIndex = 0;
	while (*desc != '\0') {
		char descChar = *(desc++);
		jvalue value;

		switch (descChar) {
		case 'Z':
		case 'C':
		case 'F':
		case 'B':
		case 'S':
		case 'I':
			value.i = args[srcIndex++];
			argObjects[dstIndex] = (Object*) dvmBoxPrimitive_fnPtr(value,
					dvmFindPrimitiveClass_fnPtr(descChar));
			/* argObjects is tracked, don't need to hold this too */
			dvmReleaseTrackedAlloc_fnPtr(argObjects[dstIndex], NULL);
			dstIndex++;
			break;
		case 'D':
		case 'J':
			value.j = dvmGetArgLong(args, srcIndex);
			srcIndex += 2;
			argObjects[dstIndex] = (Object*) dvmBoxPrimitive_fnPtr(value,
					dvmFindPrimitiveClass_fnPtr(descChar));
			dvmReleaseTrackedAlloc_fnPtr(argObjects[dstIndex], NULL);
			dstIndex++;
			break;
		case '[':
		case 'L':
			argObjects[dstIndex++] = (Object*) args[srcIndex++];
			ALOGD("boxMethodArgs object: index = %d", dstIndex - 1);
			break;
		}
	}

	return argArray;
}

 bool dvmIsPrimitiveClass(const ClassObject* clazz) {
 	return clazz->primitiveType != PRIM_NOT;
 }

