#include "_tempdir.bash"

TOP=OddReg

cat >"${TOP}".v <<"EOF"
`define CLOCK_PERIOD 1.0
module OddReg (input D, output Q);
  wire internal;
  assign internal = D;
  always #(`CLOCK_PERIOD/2.0) D <= internal;
endmodule
EOF

cat >"${TOP}".io.json.gold <<EOF
{
  "name": "OddReg",
  "ports": [
    {
      "name": "D",
      "direction": "input",
      "type": "wire",
      "width": "1"
    },
    {
      "name": "Q",
      "direction": "output",
      "type": "wire",
      "width": "1"
    }
  ]
}
EOF

cat >"${TOP}".decoupled.json.gold <<EOF
{
  "name": "OddReg",
  "decoupled io": [
  ]
}
EOF

#include "_harness.bash"
