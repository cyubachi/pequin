#!/bin/bash

set -e

DEPS_DIR=$HOME/pepper_deps
UP=`pwd`
mkdir -p $DEPS_DIR/lib/python
ln -sf $DEPS_DIR/lib $DEPS_DIR/lib64

TAR="tar xvzf"

# papi
echo "installing PAPI"
$TAR papi-5.4.1.tar.gz
cd papi-5.4.1/src
./configure --prefix=$DEPS_DIR
make
make install
cd $UP

# libconfig
echo "installing libconfig"
$TAR libconfig-1.4.8.tar.gz
cd libconfig-1.4.8
./configure --prefix=$DEPS_DIR
make
make install
cd $UP

# Cheetah template library
echo "installing Cheetah template library"
$TAR Cheetah-2.4.4.tar.gz
cd Cheetah-2.4.4
export PYTHONPATH=${DEPS_DIR}/lib/python/:${PYTHONPATH}
python setup.py install --home $DEPS_DIR
cd $UP

#Kyoto Cabinet
echo "installing Kyoto Cabinet"
$TAR kyotocabinet-1.2.76.tar.gz
cp gcc6-workaround.patch kyotocabinet-1.2.76
cd kyotocabinet-1.2.76
patch -p1 < gcc6-workaround.patch
./configure --prefix=$DEPS_DIR
make
make install
cd $UP

#leveldb
echo "installing leveldb"
$TAR leveldb-1.10.0.tar.gz
cd leveldb-1.10.0
make
cp --preserve=links libleveldb.* $DEPS_DIR/lib
cp -r include/leveldb $DEPS_DIR/include
cd $UP

#libsnark
echo "installing libsnark"
[ ! -d libsnark ] && git clone https://github.com/scipr-lab/libsnark.git
cd libsnark
git checkout 746ade7ce0f30a6f6e612e50450294c8e7ade9a4
./prepare-depends.sh
make install CURVE=ALT_BN128 NO_DOCS=1 NO_SUPERCOP=1 STATIC=1 NO_PROCPS=1 PREFIX=$DEPS_DIR
cd $UP
