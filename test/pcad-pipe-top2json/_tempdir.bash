tempdir=`mktemp -d -t pcad-pipe-top2json-tests.XXXXXXXXXX`
trap "rm -rf $tempdir" EXIT

TOP=Top
INFILE=Top.v
OUTFILE=Top.io.json

set -ex
cd $tempdir
