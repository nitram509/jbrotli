/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_meteogroup_jbrotli_BrotliDeCompressor */

#ifndef _Included_com_meteogroup_jbrotli_BrotliDeCompressor
#define _Included_com_meteogroup_jbrotli_BrotliDeCompressor
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_meteogroup_jbrotli_BrotliDeCompressor
 * Method:    deCompressBytes
 * Signature: ([BII[BII)I
 */
JNIEXPORT jint JNICALL Java_com_meteogroup_jbrotli_BrotliDeCompressor_deCompressBytes
  (JNIEnv *, jclass, jbyteArray, jint, jint, jbyteArray, jint, jint);

/*
 * Class:     com_meteogroup_jbrotli_BrotliDeCompressor
 * Method:    deCompressByteBuffer
 * Signature: (Ljava/nio/ByteBuffer;IILjava/nio/ByteBuffer;II)I
 */
JNIEXPORT jint JNICALL Java_com_meteogroup_jbrotli_BrotliDeCompressor_deCompressByteBuffer
  (JNIEnv *, jclass, jobject, jint, jint, jobject, jint, jint);

#ifdef __cplusplus
}
#endif
#endif