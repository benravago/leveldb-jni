
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include "leveldbjni.h"

jint
throwException(JNIEnv* env, leveldb::Status status) {
  const char* exceptionClass = "java/lang/Exception";

  jclass clazz = env->FindClass(exceptionClass);
  if (!clazz) {
    log("Can't find exception class %s", exceptionClass);
    return -1;
  }
  return env->ThrowNew(clazz, status.ToString().c_str());
}

void
log(const char *fmt, ...) {
  int fd = open("/dev/console", O_WRONLY);
  if (fd < 0) return;

  char buffer[1000];
  va_list ap;
  va_start(ap, fmt);
  vsprintf(buffer, fmt, ap);
  va_end(ap);

  write(fd, buffer, strlen(buffer));
  close(fd);
}
