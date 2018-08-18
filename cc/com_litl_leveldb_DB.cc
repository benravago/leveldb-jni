
#include "leveldbjni.h"
#include "com_litl_leveldb_DB.h"

#include "leveldb/db.h"
#include "leveldb/write_batch.h"

JNIEXPORT jlong JNICALL Java_com_litl_leveldb_DB_nativeOpen
  (JNIEnv *env, jclass cl, jstring dbpath)
{
  const char *path = env->GetStringUTFChars(dbpath, 0);
  log("Opening database %s", path);

  leveldb::DB* db;
  leveldb::Options options;
  options.create_if_missing = true;
  leveldb::Status status = leveldb::DB::Open(options, path, &db);
  env->ReleaseStringUTFChars(dbpath, path);

  if (!status.ok()) {
    throwException(env, status);
  } else {
    log("Database opened");
  }

  return reinterpret_cast<jlong>(db);
}

JNIEXPORT void JNICALL Java_com_litl_leveldb_DB_nativeClose
  (JNIEnv *env, jclass cl, jlong dbPtr)
{
  leveldb::DB* db = reinterpret_cast<leveldb::DB*>(dbPtr);
  if (db) {
    delete db;
  }

  log("Database closed");
}

JNIEXPORT void JNICALL Java_com_litl_leveldb_DB_nativePut
  (JNIEnv *env, jclass cl, jlong dbPtr, jbyteArray keyObj, jbyteArray valObj)
{
  leveldb::DB* db = reinterpret_cast<leveldb::DB*>(dbPtr);

  size_t keyLen = env->GetArrayLength(keyObj);
  jbyte *keyBuf = env->GetByteArrayElements(keyObj, NULL);

  size_t valLen = env->GetArrayLength(valObj);
  jbyte *valBuf = env->GetByteArrayElements(valObj, NULL);

  leveldb::Status status =
    db->Put(leveldb::WriteOptions(),
      leveldb::Slice((const char *) keyBuf, keyLen),
      leveldb::Slice((const char *) valBuf, valLen));

  env->ReleaseByteArrayElements(keyObj, keyBuf, JNI_ABORT);
  env->ReleaseByteArrayElements(valObj, valBuf, JNI_ABORT);

  if (!status.ok()) {
    throwException(env, status);
  }
}

JNIEXPORT jbyteArray JNICALL Java_com_litl_leveldb_DB_nativeGet
  (JNIEnv *env, jclass cl, jlong dbPtr, jlong snapshotPtr, jbyteArray keyObj)
{
  leveldb::DB* db = reinterpret_cast<leveldb::DB*>(dbPtr);
  leveldb::ReadOptions options = leveldb::ReadOptions();
  options.snapshot = reinterpret_cast<leveldb::Snapshot*>(snapshotPtr);

  size_t keyLen = env->GetArrayLength(keyObj);
  jbyte *buffer = env->GetByteArrayElements(keyObj, NULL);
  jbyteArray result;

  leveldb::Slice key = leveldb::Slice((const char *)buffer, keyLen);
  leveldb::Iterator* iter = db->NewIterator(options);
  iter->Seek(key);
  if (iter->Valid() && key == iter->key()) {
    leveldb::Slice value = iter->value();
    size_t len = value.size();
    result = env->NewByteArray(len);
    env->SetByteArrayRegion(result, 0, len, (const jbyte *) value.data());
  } else {
    result = NULL;
  }

  env->ReleaseByteArrayElements(keyObj, buffer, JNI_ABORT);
  delete iter;

  return result;
}

JNIEXPORT void JNICALL Java_com_litl_leveldb_DB_nativeDelete
  (JNIEnv *env, jclass cl, jlong dbPtr, jbyteArray keyObj)
{
  leveldb::DB* db = reinterpret_cast<leveldb::DB*>(dbPtr);

  size_t keyLen = env->GetArrayLength(keyObj);
  jbyte *buffer = env->GetByteArrayElements(keyObj, NULL);

  leveldb::Status status = db->Delete(leveldb::WriteOptions(), leveldb::Slice((const char *) buffer, keyLen));
  env->ReleaseByteArrayElements(keyObj, buffer, JNI_ABORT);

  if (!status.ok()) {
    throwException(env, status);
  }
}

JNIEXPORT void JNICALL Java_com_litl_leveldb_DB_nativeWrite
  (JNIEnv *env, jclass cl, jlong dbPtr, jlong batchPtr)
{
  leveldb::DB* db = reinterpret_cast<leveldb::DB*>(dbPtr);

  leveldb::WriteBatch *batch = (leveldb::WriteBatch *) batchPtr;
  leveldb::Status status = db->Write(leveldb::WriteOptions(), batch);
  if (!status.ok()) {
    throwException(env, status);
  }
}

JNIEXPORT void JNICALL Java_com_litl_leveldb_DB_nativeDestroy
  (JNIEnv *env, jclass cl, jstring dbpath)
{
  const char* path = env->GetStringUTFChars(dbpath,0);
  leveldb::Options options;
  options.create_if_missing = true;
  leveldb::Status status = DestroyDB(path, options);
  if (!status.ok()) {
    throwException(env, status);
  }
}

JNIEXPORT jlong JNICALL Java_com_litl_leveldb_DB_nativeIterator
  (JNIEnv *env, jclass cl, jlong dbPtr, jlong snapshotPtr)
{
  leveldb::DB* db = reinterpret_cast<leveldb::DB*>(dbPtr);
  leveldb::ReadOptions options = leveldb::ReadOptions();
  options.snapshot = reinterpret_cast<leveldb::Snapshot*>(snapshotPtr);

  leveldb::Iterator *iter = db->NewIterator(options);
  return reinterpret_cast<jlong>(iter);
}

JNIEXPORT jlong JNICALL Java_com_litl_leveldb_DB_nativeGetSnapshot
  (JNIEnv *env, jclass cl, jlong dbPtr)
{
  leveldb::DB* db = reinterpret_cast<leveldb::DB*>(dbPtr);
  const leveldb::Snapshot* snapshot = db->GetSnapshot();
  return reinterpret_cast<jlong>(snapshot);
}

JNIEXPORT void JNICALL Java_com_litl_leveldb_DB_nativeReleaseSnapshot
  (JNIEnv *env, jclass cl, jlong dbPtr, jlong snapshotPtr)
{
  leveldb::DB* db = reinterpret_cast<leveldb::DB*>(dbPtr);
  const leveldb::Snapshot *snapshot = reinterpret_cast<leveldb::Snapshot*>(snapshotPtr);
  db->ReleaseSnapshot(snapshot);
}
