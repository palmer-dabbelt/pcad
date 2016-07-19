tempdir=`mktemp -d -t pcad-pipe-top2json-tests.XXXXXXXXXX`
trap "rm -rf $tempdir" EXIT

TOP=Top

set -ex
cd $tempdir
