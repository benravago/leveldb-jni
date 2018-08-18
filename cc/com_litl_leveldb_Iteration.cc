
#include "leveldbjni.h"
#include "com_litl_leveldb_Iteration.h"

#include "leveldb/iterator.h"

JNIEXPORT void JNICALL Java_com_litl_leveldb_Iteration_nativeDestroy
  (JNIEnv *env, jclass cl, jlong iterPtr)
{
  leveldb::Iterator* iter = reinterpret_cast<leveldb::Iterator*>(iterPtr);
  delete iter;
}

JNIEXPORT void JNICALL Java_com_litl_leveldb_Iteration_nativeSeekToFirst
  (JNIEnv *env, jclass cl, jlong iterPtr)
{
  leveldb::Iterator* iter = reinterpret_cast<leveldb::Iterator*>(iterPtr);
  iter->SeekToFirst();
}

JNIEXPORT void JNICALL Java_com_litl_leveldb_Iteration_nativeSeekToLast
  (JNIEnv *env, jclass cl, jlong iterPtr)
{
  leveldb::Iterator* iter = reinterpret_cast<leveldb::Iterator*>(iterPtr);
  iter->SeekToLast();
}

JNIEXPORT void JNICALL Java_com_litl_leveldb_Iteration_nativeSeek
  (JNIEnv *env, jclass cl, jlong iterPtr, jbyteArray keyObj)
{
  leveldb::Iterator* iter = reinterpret_cast<leveldb::Iterator*>(iterPtr);

  size_t keyLen = env->GetArrayLength(keyObj);
  jbyte *buffer = env->GetByteArrayElements(keyObj, NULL);

  iter->Seek(leveldb::Slice((const char *)buffer, keyLen));
  env->ReleaseByteArrayElements(keyObj, buffer, JNI_ABORT);
}

JNIEXPORT jboolean JNICALL Java_com_litl_leveldb_Iteration_nativeValid
  (JNIEnv *env, jclass cl, jlong iterPtr)
{
  leveldb::Iterator* iter = reinterpret_cast<leveldb::Iterator*>(iterPtr);
  return iter->Valid();
}

JNIEXPORT void JNICALL Java_com_litl_leveldb_Iteration_nativeNext
  (JNIEnv *env, jclass cl, jlong iterPtr)
{
  leveldb::Iterator* iter = reinterpret_cast<leveldb::Iterator*>(iterPtr);
  iter->Next();
}

JNIEXPORT void JNICALL Java_com_litl_leveldb_Iteration_nativePrev
  (JNIEnv *env, jclass cl, jlong iterPtr)
{
  leveldb::Iterator* iter = reinterpret_cast<leveldb::Iterator*>(iterPtr);
  iter->Prev();
}

JNIEXPORT jbyteArray JNICALL Java_com_litl_leveldb_Iteration_nativeKey
  (JNIEnv *env, jclass cl, jlong iterPtr)
{
  leveldb::Iterator* iter = reinterpret_cast<leveldb::Iterator*>(iterPtr);
  leveldb::Slice key = iter->key();

  size_t len = key.size();
  jbyteArray result = env->NewByteArray(len);
  env->SetByteArrayRegion(result, 0, len, (const jbyte *) key.data());
  return result;
}

JNIEXPORT jbyteArray JNICALL Java_com_litl_leveldb_Iteration_nativeValue
  (JNIEnv *env, jclass cl, jlong iterPtr)
{
  leveldb::Iterator* iter = reinterpret_cast<leveldb::Iterator*>(iterPtr);
  leveldb::Slice value = iter->value();

  size_t len = value.size();
  jbyteArray result = env->NewByteArray(len);
  env->SetByteArrayRegion(result, 0, len, (const jbyte *) value.data());
  return result;
}
