$PTEST_BINARY --macro-list "${INPUT}" --verilog "${OUTPUT}" ${LIBS[@]}

cat "${OUTPUT}"

diff -u "${OUTPUT}" "${OUTPUT}.gold"
