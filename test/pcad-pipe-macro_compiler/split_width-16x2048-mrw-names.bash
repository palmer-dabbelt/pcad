#include "_tempdir.bash"

cat >"$INPUT" <<"EOF"
[
  {
    "name": "name_of_sram_module",
    "depth": 2048,
    "width": 16,
    "ports": [
      {
        "clock port name": "clock",
        "mask granularity": 8,
        "output port name": "RW0O",
        "input port name": "RW0I",
        "address port name": "RW0A",
        "mask port name": "RW0M",
        "chip enable port name": "RW0E",
	"write enable port name": "RW0W"
      }
    ]
  }
]
EOF

LIBS+=("-l" "library.json")
cat >"library.json" <<"EOF"
[
  {
    "name": "vendor_sram",
    "depth": 2048,
    "width": 8,
    "ports": [
      {
        "clock port name": "vendor_clock",
        "mask granularity": 8,
        "output port name": "vendor_RW0O",
        "input port name": "vendor_RW0I",
        "address port name": "vendor_RW0A",
        "mask port name": "vendor_RW0M",
        "chip enable port name": "vendor_RW0E",
	"write enable port name": "vendor_RW0W"
      }
    ]
  }
]
EOF

cat >"$OUTPUT".gold <<"EOF"
module name_of_sram_module(
  input [0:0] clock,
  output [15:0] RW0O,
  input [15:0] RW0I,
  input [10:0] RW0A,
  input [1:0] RW0M,
  input [0:0] RW0E,
  input [0:0] RW0W
);
  wire [7:0] RW0O_0_0;
  wire [7:0] RW0I_0_0;
  wire [10:0] RW0A_0_0;
  wire [0:0] RW0M_0_0;
  wire [7:0] RW0O_0_1;
  wire [7:0] RW0I_0_1;
  wire [10:0] RW0A_0_1;
  wire [0:0] RW0M_0_1;
  vendor_sram mem_0_0(
    .vendor_clock(clock),
    .vendor_RW0O(RW0O_0_0),
    .vendor_RW0I(RW0I_0_0),
    .vendor_RW0A(RW0A_0_0),
    .vendor_RW0M(RW0M_0_0),
    .vendor_RW0E(RW0E),
    .vendor_RW0W(RW0W)
  );
  vendor_sram mem_0_1(
    .vendor_clock(clock),
    .vendor_RW0O(RW0O_0_1),
    .vendor_RW0I(RW0I_0_1),
    .vendor_RW0A(RW0A_0_1),
    .vendor_RW0M(RW0M_0_1),
    .vendor_RW0E(RW0E),
    .vendor_RW0W(RW0W)
  );
  assign RW0I_0_0 = RW0I[7:0];
  assign RW0A_0_0 = RW0A[10:0];
  assign RW0M_0_0 = RW0M[0:0];
  assign RW0I_0_1 = RW0I[15:8];
  assign RW0A_0_1 = RW0A[10:0];
  assign RW0M_0_1 = RW0M[1:1];
  assign RW0O = {RW0O_0_1 ,RW0O_0_0};
endmodule
EOF

#include "_harness.bash"
