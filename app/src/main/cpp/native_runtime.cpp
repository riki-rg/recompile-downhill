#include <jni.h>

extern "C" JNIEXPORT jstring JNICALL
Java_id_dev_rikirg_downhillnative_MainActivity_nativeRuntimeName(
        JNIEnv *environment,
        jclass /* clazz */) {
    return environment->NewStringUTF("C++20 / Android NDK");
}
