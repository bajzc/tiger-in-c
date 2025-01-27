#!/usr/bin/env bash

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

pwd

pushd .. # root of repo
if [ ! -f tigerc ]; then
  echo tigerc not exist
  exit 1
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