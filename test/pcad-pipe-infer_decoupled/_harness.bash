cat "${TOP}.v"

$PTEST_BINARY --top $TOP --input "${TOP}.v" --output "${TOP}.decoupled.json"

cat "${TOP}.decoupled.json"

diff -u "${TOP}.decoupled.json" "${TOP}.decoupled.json.gold"
