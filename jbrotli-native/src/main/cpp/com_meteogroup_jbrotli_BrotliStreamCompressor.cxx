/*
 * Copyright (c) 2015 MeteoGroup Deutschland GmbH
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* exporting methods */
#if (__GNUC__ >= 4) || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
#  ifndef GCC_HASCLASSVISIBILITY
#    define GCC_HASCLASSVISIBILITY
#  endif
#endif

/* Deal with Apple's deprecated 'AssertMacros.h' from Carbon-framework */
#if defined(__APPLE__) && !defined(__ASSERT_MACROS_DEFINE_VERSIONS_WITHOUT_UNDERSCORES)
# define __ASSERT_MACROS_DEFINE_VERSIONS_WITHOUT_UNDERSCORES 0
#endif

/* Intel's compiler complains if a variable which was never initialised is
 * cast to void, which is a common idiom which we use to indicate that we
 * are aware a variable isn't used.  So we just silence that warning.
 * See: https://github.com/swig/swig/issues/192 for more discussion.
 */
#ifdef __INTEL_COMPILER
# pragma warning disable 592
#endif

/* Fix for jlong on some versions of gcc on Windows */
#if defined(__GNUC__) && !defined(__INTEL_COMPILER)
typedef long long __int64;
#endif

/* Fix for jlong on 64-bit x86 Solaris */
#if defined(__x86_64)
# ifdef _LP64
#   undef _LP64
# endif
#endif

#include <jni.h>
#include <stdlib.h>
#include <string.h>

#include "../../../../brotli/enc/encode.h"
#include "./type_converters.h"
#include "./param_converter.h"
#include "./com_meteogroup_jbrotli_BrotliCompressor.h"
#include "./com_meteogroup_jbrotli_BrotliError.h"


#ifdef __cplusplus
extern "C" {
#endif

static jfieldID brotliCompressorInstanceRefID;

/*
 * Class:     com_meteogroup_jbrotli_BrotliStreamCompressor
 * Method:    initJavaFieldIdCache
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_meteogroup_jbrotli_BrotliStreamCompressor_initJavaFieldIdCache(JNIEnv *env,
                                                                                            jclass cls) {
  brotliCompressorInstanceRefID = env->GetFieldID(cls, "brotliCompressorInstanceRef", "J");
  if (NULL == brotliCompressorInstanceRefID) {
    return com_meteogroup_jbrotli_BrotliError_NATIVE_GET_FIELD_ID_ERROR;
  }
  return 0;
}

/*
 * Class:     com_meteogroup_jbrotli_BrotliStreamCompressor
 * Method:    initBrotliCompressor
 * Signature: (IIII)I
 */
JNIEXPORT jint JNICALL Java_com_meteogroup_jbrotli_BrotliStreamCompressor_initBrotliCompressor(JNIEnv *env,
                                                                                            jobject thisObj,
                                                                                            jint mode,
                                                                                            jint quality,
                                                                                            jint lgwin,
                                                                                            jint lgblock) {
  brotli::BrotliParams params = mapToBrotliParams(env, mode, quality, lgwin, lgblock);

  brotli::BrotliCompressor *compressor = (brotli::BrotliCompressor*) GetLongFieldAsPointer(env, thisObj, brotliCompressorInstanceRefID);
  if (NULL != compressor) {
    delete compressor;
  }
  compressor = new brotli::BrotliCompressor(params);
  SetLongFieldFromPointer(env, thisObj, brotliCompressorInstanceRefID, compressor);

  return 0;
}

/*
 * Class:     com_meteogroup_jbrotli_BrotliStreamCompressor
 * Method:    freeNativeResources
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_meteogroup_jbrotli_BrotliStreamCompressor_freeNativeResources(JNIEnv *env,
                                                                                           jobject thisObj) {
  brotli::BrotliCompressor *compressor = (brotli::BrotliCompressor*) GetLongFieldAsPointer(env, thisObj, brotliCompressorInstanceRefID);
  if (NULL != compressor) {
    delete compressor;
    compressor = NULL;
    SetLongFieldFromPointer(env, thisObj, brotliCompressorInstanceRefID, compressor);
  }
  return 0;
}

/*
 * Class:     com_meteogroup_jbrotli_BrotliStreamCompressor
 * Method:    getInputBlockSize
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_meteogroup_jbrotli_BrotliStreamCompressor_getInputBlockSize(JNIEnv *env,
                                                                                         jobject thisObj) {
  brotli::BrotliCompressor *compressor = (brotli::BrotliCompressor*) GetLongFieldAsPointer(env, thisObj, brotliCompressorInstanceRefID);
  if (NULL == compressor) {
    env->ThrowNew(env->FindClass("java/lang/IllegalStateException"), "BrotliStreamCompressor was already closed. You need to create a new object before getInputBlockSize.");
    return com_meteogroup_jbrotli_BrotliError_NATIVE_ERROR;
  }
  return compressor->input_block_size();
}

/*
 * Class:     com_meteogroup_jbrotli_BrotliStreamCompressor
 * Method:    compressBytes
 * Signature: ([BIIZ)[B
 */
JNIEXPORT jbyteArray JNICALL Java_com_meteogroup_jbrotli_BrotliStreamCompressor_compressBytes(JNIEnv *env,
                                                                                           jobject thisObj,
                                                                                           jbyteArray inByteArray,
                                                                                           jint inPosition,
                                                                                           jint inLength,
                                                                                           jboolean doFlush) {

  if (inPosition < 0 || inLength < 0) {
    env->ThrowNew(env->FindClass("java/lang/IllegalArgumentException"), "Brotli: input array position and length must be greater than zero.");
    return NULL;
  }

  brotli::BrotliCompressor *compressor = (brotli::BrotliCompressor*) GetLongFieldAsPointer(env, thisObj, brotliCompressorInstanceRefID);
  if (NULL == compressor) {
    env->ThrowNew(env->FindClass("java/lang/IllegalStateException"), "BrotliStreamCompressor was already closed. You need to create a new object before start compressing.");
    return NULL;
  }

  if (inLength > 0) {
    uint8_t *inBufCritArray = (uint8_t *) env->GetPrimitiveArrayCritical(inByteArray, 0);
    if (inBufCritArray == NULL || env->ExceptionCheck()) return NULL;
    compressor->CopyInputToRingBuffer(inLength, inBufCritArray + inPosition);
    env->ReleasePrimitiveArrayCritical(inByteArray, inBufCritArray, 0);
    if (env->ExceptionCheck()) return NULL;
  }

  size_t computedOutLength = 0;
  uint8_t *brotliOutBufferPtr;
  bool writeResult = compressor->WriteBrotliData(doFlush, false, &computedOutLength, &brotliOutBufferPtr);
  if (!writeResult) {
    env->ThrowNew(env->FindClass("java/lang/IllegalStateException"), "Error in native code BrotliCompressor::WriteBrotliData().");
    return NULL;
  }

  jbyteArray outByteArray = env->NewByteArray(computedOutLength);
  if (computedOutLength > 0) {
    uint8_t *outBufCritArray = (uint8_t *) env->GetPrimitiveArrayCritical(outByteArray, 0);
    if (outBufCritArray == NULL || env->ExceptionCheck()) return NULL;
    memcpy(outBufCritArray, brotliOutBufferPtr, computedOutLength);
    env->ReleasePrimitiveArrayCritical(outByteArray, outBufCritArray, 0);
    if (env->ExceptionCheck()) return NULL;
  }

  return outByteArray;
}

/*
 * Class:     com_meteogroup_jbrotli_BrotliStreamCompressor
 * Method:    compressByteBuffer
 * Signature: (Ljava/nio/ByteBuffer;IIZ)Ljava/nio/ByteBuffer;
 */
JNIEXPORT jobject JNICALL Java_com_meteogroup_jbrotli_BrotliStreamCompressor_compressByteBuffer(JNIEnv *env,
                                                                                             jobject thisObj,
                                                                                             jobject inBuf,
                                                                                             jint inPosition,
                                                                                             jint inLength,
                                                                                             jboolean doFlush) {

  if (inPosition < 0 || inLength < 0 ) {
    env->ThrowNew(env->FindClass("java/lang/IllegalArgumentException"), "Brotli: input ByteBuffer position and length must be greater than zero.");
    return NULL;
  }

  brotli::BrotliCompressor *compressor = (brotli::BrotliCompressor *) GetLongFieldAsPointer(env, thisObj, brotliCompressorInstanceRefID);
  if (NULL == compressor) {
    env->ThrowNew(env->FindClass("java/lang/IllegalStateException"), "BrotliStreamCompressor was already closed. You need to create a new object before start compressing.");
    return NULL;
  }

  uint8_t *inBufPtr = (uint8_t *) env->GetDirectBufferAddress(inBuf);
  if (NULL == inBufPtr) {
    env->ThrowNew(env->FindClass("java/lang/IllegalStateException"), "BrotliStreamCompressor couldn't get direct address of input buffer.");
    return NULL;
  }

  if (inLength > 0) {
    compressor->CopyInputToRingBuffer(inLength, inBufPtr + inPosition);
  }

  size_t computedOutLength = 0;
  uint8_t *brotliOutBufferPtr;
  bool writeResult = compressor->WriteBrotliData(doFlush, false, &computedOutLength, &brotliOutBufferPtr);
  if (!writeResult)  {
    env->ThrowNew(env->FindClass("java/lang/IllegalStateException"), "BrotliStreamCompressor got an error while calling WriteBrotliData() method.");
    return NULL;
  }

  // in case of exception, directly return with the NULL result
  return env->NewDirectByteBuffer(brotliOutBufferPtr, computedOutLength);
}

#ifdef __cplusplus
}
#endif
