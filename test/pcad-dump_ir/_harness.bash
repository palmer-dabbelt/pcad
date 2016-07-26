$PTEST_BINARY --top $TOP --input "${TOP}.v" --output "${TOP}.pcad.json"

cat "${TOP}.pcad.json"

cat > parse.py << EOF
import json

json_data=open("${TOP}.pcad.json").read()
data = json.loads(json_data)
EOF

python2 parse.py

if test ! -e "${TOP}.pcad.json.gold"
then
    exit 0
fi

diff -u "${TOP}.pcad.json" "${TOP}.pcad.json.gold"
