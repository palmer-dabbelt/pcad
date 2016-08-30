$PTEST_BINARY --input "${TOP}.v" --output "${TOP}.bb.json"

cat "${TOP}.bb.json"

cat > parse.py << EOF
import json

json_data=open("${TOP}.bb.json").read()
data = json.loads(json_data)
EOF

python2 parse.py

if test ! -e "${TOP}.bb.json.gold"
then
    exit 0
fi

diff -u "${TOP}.bb.json" "${TOP}.bb.json.gold"
