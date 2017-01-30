$PTEST_BINARY --macro-list "${INPUT}" --verilog "${OUTPUT}.v" ${LIBS[@]} ${COMPILER_ARGS[@]}

cat "${OUTPUT}.v"

if test -e "${OUTPUT}.v.gold"
then
    diff -u "${OUTPUT}.v" "${OUTPUT}.v.gold"
fi
