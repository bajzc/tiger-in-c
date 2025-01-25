#!/usr/bin/env bash

set -e

pwd

pushd .. # root of repo
if [ ! -f tigerc ]; then # compile tigerc if needed
  cmake .
  make
fi
./tigerc sample/$1.tig
cp -v sample/$1.tig.s test/xv6/user/tiger.s
popd

pushd xv6
./test_in_xv6 $1
status=$?
popd
exit $status