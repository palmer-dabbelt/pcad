cat $INFILE

$PTEST_BINARY --top $TOP --input $INFILE --output $OUTFILE

cat $OUTFILE

diff -u $OUTFILE $OUTFILE.gold
