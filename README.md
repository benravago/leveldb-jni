[leveldb](http://leveldb.org/) for jdk10

Very simple bindings for using leveldb with jdk10, derived from [android-leveldb](https://github.com/litl/android-leveldb).

To build:
1. edit the <code>Makefile</code> as appropriate
2. run <code>make</code>
3. copy the generated <code>leveldbjni.jar</code> to your classpath and <code>libleveldbjni.so</code> to your <code>java.library.path</code>

Also includes <code>leveldb.mk</code>, a script for downloading and building <code>libleveldb.so</code> from google's leveldb [repo](https://github.com/google/leveldb).
