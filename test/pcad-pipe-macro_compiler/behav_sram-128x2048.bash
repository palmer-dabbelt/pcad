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
        "mask port name": "RW0M",
        "enable port name": "RW0E"
      }
    ]
  }
]
EOF

cat >"$OUTPUT".gold <<"EOF"
module name_of_sram_module(
);
  wire [127:0] mem [2047:0];
endmodule
EOF

#include "_harness.bash"
