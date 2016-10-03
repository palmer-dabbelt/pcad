#include "_tempdir.bash"

TOP=memory

cat >"${TOP}".v <<EOF
module memory (input clk, input [7:0] raddr, input [7:0] waddr, input [1:0] data, output [1:0] Q);
  wire [1:0] array [0:127];
  wire [1:0] Q_buf;
  assign Q = Q_buf;
  always #(1)
  begin
    Q_buf <= array[raddr];
    array[waddr] <= data;
  end
endmodule
EOF

cat >"${TOP}".io.json.gold <<EOF
{
  "name": "memory",
  "ports": [
    {
      "name": "clk",
      "direction": "input",
      "type": "wire",
      "width": "1"
    },
    {
      "name": "raddr",
      "direction": "input",
      "type": "wire",
      "width": "8"
    },
    {
      "name": "waddr",
      "direction": "input",
      "type": "wire",
      "width": "8"
    },
    {
      "name": "data",
      "direction": "input",
      "type": "wire",
      "width": "2"
    },
    {
      "name": "Q",
      "direction": "output",
      "type": "wire",
      "width": "2"
    }
  ]
}
EOF

cat >"${TOP}".decoupled.json.gold <<EOF
{
  "name": "memory",
  "decoupled io": [
  ]
}
EOF

#include "_harness.bash"
