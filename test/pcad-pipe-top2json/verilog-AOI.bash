#include "_tempdir.bash"

TOP=AOI
INFILE=AOI.v
OUTFILE=AOI.io.json

cat >$INFILE <<EOF
// Verilog code for AND-OR-INVERT gate
module AOI (input A, input B, input C, input D, output F);
  assign F = ~((A & B) | (C & D));
endmodule
// end of Verilog code
EOF

cat >$OUTFILE.gold <<EOF
{
  "name": "AOI",
  "ports": [
    {
      "name": "A",
      "direction": "input",
      "type": "wire",
      "width": "1"
    },
    {
      "name": "B",
      "direction": "input",
      "type": "wire",
      "width": "1"
    },
    {
      "name": "C",
      "direction": "input",
      "type": "wire",
      "width": "1"
    },
    {
      "name": "D",
      "direction": "input",
      "type": "wire",
      "width": "1"
    },
    {
      "name": "F",
      "direction": "output",
      "type": "wire",
      "width": "1"
    },
  ]
}
EOF

#include "_harness.bash"
