#ifndef LEVELDBJNI_H_
#define LEVELDBJNI_H_

#include <jni.h>

#include "leveldb/status.h"

jint throwException(JNIEnv* env, leveldb::Status status);
void log(const char *format, ...);

#endif /* LEVELDBJNI_H_ */
