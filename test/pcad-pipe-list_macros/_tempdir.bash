set -x

tempdir=`mktemp -d -t pcad-pipe-macro_compiler-tests.XXXXXXXXXX`
trap "rm -rf $tempdir" EXIT

TOP="Top"
INPUT="${TOP}.macros.json"
LIBS=()
OUTPUT="${TOP}.macros"
COMPILER_ARGS=()
LIST_MACRO_TYPE="sram"

set -ex
cd $tempdir
