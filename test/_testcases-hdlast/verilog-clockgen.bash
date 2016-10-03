#include "_tempdir.bash"

TOP=ClkGen

cat >"${TOP}".v <<EOF
module ClkGen (output clk);
  reg internal = 1'b0;
  always #(1.0) internal = ~internal;
  assign clk = internal;
endmodule
EOF

cat >"${TOP}".io.json.gold <<EOF
{
  "name": "ClkGen",
  "ports": [
    {
      "name": "clk",
      "direction": "output",
      "type": "wire",
      "width": "1"
    }
  ]
}
EOF

cat >"${TOP}".decoupled.json.gold <<EOF
{
  "name": "ClkGen",
  "decoupled io": [
  ]
}
EOF

#include "_harness.bash"
