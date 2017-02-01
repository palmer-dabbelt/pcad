$PTEST_BINARY --macro-list "${INPUT}" --verilog "${OUTPUT}.v" ${LIBS[@]} ${COMPILER_ARGS[@]}

cat "${OUTPUT}.v"

if test -e "${OUTPUT}.v.gold"
then
    diff -u "${OUTPUT}.v" "${OUTPUT}.v.gold"
fi

if test -e "${OUTPUT}.gold"
then
    echo "Detected old test harness" >&2
    exit 2
fi
