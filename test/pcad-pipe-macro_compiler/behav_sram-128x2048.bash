#include "_tempdir.bash"

cat >"$INPUT" <<"EOF"
[
  {
    "name": "name_of_sram_module",
    "depth": 2048,
    "width": 128,
    "ports": [
      {
        "direction": "read+write",
        "clock name": "clock",
        "mask granularity": 20,
        "output port name": "RW0O",
        "input port name": "RW0I",
        "address port name": "RW0A",
        "mask port name": "RW0M",
        "enable port name": "RW0E"
      }
    ]
  }
]
EOF

cat >"$OUTPUT".gold <<"EOF"
module name_of_sram_module(
  input clock,
  input RW0O,
  input RW0I,
  input RW0M,
  input RW0A,
  input RW0E
);
  wire [127:0] mem [2047:0];
  wire [127:0] read_data_0;
  always @(posedge clock)
  begin
    read_data_0 <= mem[RW0A];
    RW0O <= read_data_0;
  end
  always @(posedge clock)
  begin
    if (RW0E)
    begin
      mem[RW0A] <= RW0I;
    end
    else
    begin
    end
  end
endmodule
EOF

#include "_harness.bash"
