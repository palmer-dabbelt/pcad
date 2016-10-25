#include "_tempdir.bash"

cat >"$INPUT" <<"EOF"
[
  {
    "name": "name_of_sram_module",
    "depth": 2048,
    "width": 8,
    "ports": [
      {
        "clock port name": "clock",
	"clock port polarity": "positive edge",
        "mask granularity": 8,
        "output port name": "RW0O",
	"output port polarity": "active high",
        "input port name": "RW0I",
	"input port polarity": "active high",
        "address port name": "RW0A",
	"address port polarity": "active high",
        "mask port name": "RW0M",
	"mask port polarity": "active high",
        "chip enable port name": "RW0E",
	"chip enable port polarity": "active high",
	"write enable port name": "RW0W",
	"write enable port polarity": "active high"
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
    "depth": 1024,
    "width": 8,
    "ports": [
      {
        "clock port name": "clock",
        "mask granularity": 8,
        "output port name": "RW0O",
        "input port name": "RW0I",
        "address port name": "RW0A",
        "mask port name": "RW0M",
        "chip enable port name": "RW0E",
	"write enable port name": "RW0W",
	"clock port polarity": "positive edge",
	"output port polarity": "active high",
	"input port polarity": "active high",
	"address port polarity": "active high",
	"mask port polarity": "active high",
	"chip enable port polarity": "active high",
	"write enable port polarity": "active high"
      }
    ]
  }
]
EOF

cat >"$OUTPUT".gold <<"EOF"
module name_of_sram_module(
  input [0:0] clock,
  output [7:0] RW0O,
  input [7:0] RW0I,
  input [10:0] RW0A,
  input [0:0] RW0M,
  input [0:0] RW0E,
  input [0:0] RW0W
);
  wire [7:0] RW0O_0_0;
  wire [7:0] RW0I_0_0;
  wire [9:0] RW0A_0_0;
  wire [0:0] RW0M_0_0;
  wire [7:0] RW0O_1_0;
  wire [7:0] RW0I_1_0;
  wire [9:0] RW0A_1_0;
  wire [0:0] RW0M_1_0;
  vendor_sram mem_0_0(
    .clock(clock),
    .RW0O(RW0O_0_0),
    .RW0I(RW0I_0_0),
    .RW0A(RW0A_0_0),
    .RW0M(RW0M_0_0),
    .RW0E(RW0E),
    .RW0W(RW0W)
  );
  vendor_sram mem_1_0(
    .clock(clock),
    .RW0O(RW0O_1_0),
    .RW0I(RW0I_1_0),
    .RW0A(RW0A_1_0),
    .RW0M(RW0M_1_0),
    .RW0E(RW0E),
    .RW0W(RW0W)
  );
  assign RW0I_0_0 = RW0I[7:0];
  assign RW0A_0_0 = RW0A[9:0];
  assign RW0M_0_0 = (RW0M[0:0] && (RW0A[10:10] == 0));
  assign RW0I_1_0 = RW0I[7:0];
  assign RW0A_1_0 = RW0A[9:0];
  assign RW0M_1_0 = (RW0M[0:0] && (RW0A[10:10] == 1));
  assign RW0O = {RW0O_1_0[7:0] ,RW0O_0_0[7:0]};
endmodule
EOF

#include "_harness.bash"
