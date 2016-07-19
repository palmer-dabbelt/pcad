cat "${TOP}.v"

$PTEST_BINARY --top $TOP --input "${TOP}.v" --output "${TOP}.io.json"

cat "${TOP}.io.json"

diff -u "${TOP}.io.json" "${TOP}.io.json.gold"

cat > parse.py << EOF
import json

json_data=open("${TOP}.io.json").read()
data = json.loads(json_data)
EOF

python2 parse.py
