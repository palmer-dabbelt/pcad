cat "${TOP}.v"

$PTEST_BINARY --top $TOP --input "${TOP}.v" --output "${TOP}.decoupled.json"

cat "${TOP}.decoupled.json"

diff -u "${TOP}.decoupled.json" "${TOP}.decoupled.json.gold"

cat > parse.py << EOF
import json

json_data=open("${TOP}.decoupled.json").read()
data = json.loads(json_data)
EOF

python2 parse.py
