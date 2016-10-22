$PTEST_BINARY --macro-list "${INPUT}" --verilog "${OUTPUT}" ${LIBS[@]}

cat "${OUTPUT}"

if test -e "${OUTPUT}.gold"
then
    diff -u "${OUTPUT}" "${OUTPUT}.gold"
fi
