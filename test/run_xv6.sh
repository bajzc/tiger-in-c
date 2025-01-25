#!/usr/bin/env bash

set -e

pwd

pushd .. # root of repo
if [ ! -f Makefile ]; then
  cmake .
fi
if [ ! -f tigerc ]; then # compile tigerc if needed
  make
fi
./tigerc sample/$1.tig
cp -v sample/$1.tig.s test/xv6/user/tiger.s
popd

pushd xv6
rm -vf user/_runtime
./test_in_xv6 $1
status=$?
popd
exit $status