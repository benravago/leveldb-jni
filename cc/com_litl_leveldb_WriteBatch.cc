
#include "leveldbjni.h"
#include "com_litl_leveldb_WriteBatch.h"

#include "leveldb/write_batch.h"

JNIEXPORT jlong JNICALL Java_com_litl_leveldb_WriteBatch_nativeCreate
  (JNIEnv *env, jclass cl)
{
  leveldb::WriteBatch* batch = new leveldb::WriteBatch();
  return reinterpret_cast<jlong>(batch);
}

JNIEXPORT void JNICALL Java_com_litl_leveldb_WriteBatch_nativeDestroy
  (JNIEnv *env, jclass cl, jlong ptr)
{
  leveldb::WriteBatch* batch = reinterpret_cast<leveldb::WriteBatch*>(ptr);
  delete batch;
}

JNIEXPORT void JNICALL Java_com_litl_leveldb_WriteBatch_nativeDelete
  (JNIEnv *env, jclass cl, jlong ptr, jbyteArray keyObj)
{
  leveldb::WriteBatch* batch = reinterpret_cast<leveldb::WriteBatch*>(ptr);

  size_t keyLen = env->GetArrayLength(keyObj);
  jbyte *buffer = env->GetByteArrayElements(keyObj, NULL);

  batch->Delete(leveldb::Slice((const char *) buffer, keyLen));
  env->ReleaseByteArrayElements(keyObj, buffer, JNI_ABORT);
}

JNIEXPORT void JNICALL Java_com_litl_leveldb_WriteBatch_nativePut
  (JNIEnv *env, jclass cl, jlong ptr, jbyteArray keyObj, jbyteArray valObj)
{
  leveldb::WriteBatch* batch = reinterpret_cast<leveldb::WriteBatch*>(ptr);

  size_t keyLen = env->GetArrayLength(keyObj);
  jbyte *keyBuf = env->GetByteArrayElements(keyObj, NULL);

  size_t valLen = env->GetArrayLength(valObj);
  jbyte *valBuf = env->GetByteArrayElements(valObj, NULL);

  batch->Put(leveldb::Slice((const char *) keyBuf, keyLen),
             leveldb::Slice((const char *) valBuf, valLen));

  env->ReleaseByteArrayElements(keyObj, keyBuf, JNI_ABORT);
  env->ReleaseByteArrayElements(valObj, valBuf, JNI_ABORT);
}

JNIEXPORT void JNICALL Java_com_litl_leveldb_WriteBatch_nativeClear
  (JNIEnv *env, jclass cl, jlong ptr)
{
  leveldb::WriteBatch* batch = reinterpret_cast<leveldb::WriteBatch*>(ptr);
  batch->Clear();
}
