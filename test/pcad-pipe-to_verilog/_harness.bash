cat ${TOP}.v

$PTEST_BINARY --input "${TOP}.v" --output "${TOP}.1.v" --top "${TOP}"

if test -f "${TOP}.canonical.v.gold"
then
    diff -u "${TOP}.1.v" "${TOP}.canonical.v.gold"
fi

cat ${TOP}.1.v

$PTEST_BINARY --input "${TOP}.1.v" --output "${TOP}.2.v" --top "${TOP}"

diff -u "${TOP}.1.v" "${TOP}.2.v"
