
JAVA_HOME = /opt/jdk
ARCH = linux

JAVAC = $(JAVA_HOME)/bin/javac
JAR = $(JAVA_HOME)/bin/jar

JAVA_SRC = \
  java/com/litl/leveldb/DB.java \
  java/com/litl/leveldb/Iteration.java \
  java/com/litl/leveldb/NativeObject.java \
  java/com/litl/leveldb/WriteBatch.java

LEVELDB_INCLUDE = /usr/local/include
LEVELDB_LIB = /usr/local/lib64

CC = cc

CC_INC = \
  -I$(JAVA_HOME)/include \
  -I$(JAVA_HOME)/include/$(ARCH) \
  -I$(LEVELDB_INCLUDE) \
  -I./h

CC_LD = \
  -L$(LEVELDB_LIB) -lleveldb 

CC_SRC = \
  cc/com_litl_leveldb_DB.cc \
  cc/com_litl_leveldb_Iteration.cc \
  cc/com_litl_leveldb_WriteBatch.cc \
  cc/leveldbjni.cc


all: leveldbjni.jar libleveldbjni.so

bin/com: $(JAVA_SRC)
	$(JAVAC) -d bin -h h $^

leveldbjni.jar: bin/com
	$(JAR) cvf $@ -C bin com

libleveldbjni.so: $(CC_SRC)
	$(CC) -g -shared -fpic $(CC_INC) $(CC_LD) $^ -o $@

clean:
	rm -fr *.jar *.so bin h

