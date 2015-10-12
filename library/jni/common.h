
#include <stdint.h>    /* C99 */
#include <android/log.h>


typedef uint8_t u1;
typedef uint16_t u2;
typedef uint32_t u4;
typedef uint64_t u8;
typedef int8_t s1;
typedef int16_t s2;
typedef int32_t s4;
typedef int64_t s8;

/*
 * access flags and masks; the "standard" ones are all <= 0x4000
 *
 * Note: There are related declarations in vm/oo/Object.h in the ClassFlags
 * enum.
 */

/*
 * Some systems might have this in <stdbool.h>.
 */
#ifndef __CPP__
#ifndef __bool_true_false_are_defined
typedef enum { false=0, true=!false } bool;
#define __bool_true_false_are_defined 1
#endif
#endif

#define TAG "LayoutCast_natvie"

#define ALOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)
#define ALOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)


typedef uint8_t u1;
typedef uint16_t u2;
typedef uint32_t u4;
typedef uint64_t u8;
typedef int8_t s1;
typedef int16_t s2;
typedef int32_t s4;
typedef int64_t s8;


void (*libdalvikhook_log_function)(char *logmsg);

void* dalvikhook_set_logfunction(void *func);

#define log(...) \
        {char __msg[1024] = {0};\
        snprintf(__msg, sizeof(__msg)-1, __VA_ARGS__);\
        libdalvikhook_log_function(__msg); }



