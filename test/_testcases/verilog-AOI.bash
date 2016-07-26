#include "_tempdir.bash"

TOP=AOI

cat >"${TOP}".v <<EOF
// Verilog code for AND-OR-INVERT gate
module AOI (input A, input B, input C, input D, output F);
  assign F = ~((A & B) | (C & D));
endmodule
// end of Verilog code
EOF

cat >"${TOP}".io.json.gold <<EOF
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
    }
  ]
}
EOF

cat >"${TOP}".decoupled.json.gold <<EOF
{
  "name": "AOI",
  "decoupled io": [
  ]
}
EOF

#include "_harness.bash"
