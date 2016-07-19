cat "${TOP}.v"

$PTEST_BINARY --top $TOP --input "${TOP}.v" --output "${TOP}.io.json"

cat "${TOP}.io.json"

diff -u "${TOP}.io.json" "${TOP}.io.json.gold"
