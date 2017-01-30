if [[ "${LIBS[@]}" == "" ]]
then
    exit 0
fi

$PTEST_BINARY -t $LIST_MACRO_TYPE ${LIBS[@]} > "${OUTPUT}.list"

cat "${OUTPUT}.list"

if test -e "${OUTPUT}.list.gold"
then
    diff -u "${OUTPUT}.list" "${OUTPUT}.list.gold"
fi
