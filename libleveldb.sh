#
set -x

git clone \
  --quiet \
  https://github.com/google/leveldb.git \
  leveldb-master

D=$(cd leveldb-master && git log -1 --date=short --format=%cd)
mv -v leveldb-master leveldb-${D}
cd leveldb-${D}

mkdir build
cd build
cmake \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_VERBOSE_MAKEFILE=ON \
  -DBUILD_SHARED_LIBS=ON \
  .. 
cmake \
  --build .
cd ..

cp -av include/leveldb /usr/local/include/
cp -av build/libleveldb.so /usr/local/lib64/
ldconfig

