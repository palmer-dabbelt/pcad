#include "_tempdir.bash"

cat >"$INPUT" <<"EOF"
[
  {
    "type": "sram",
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

LIBS+=("-l" "library.json")
cat >"library.json" <<"EOF"
[
  {
    "type": "sram",
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
	"write enable port name": "vendor_RW0W",
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

cat >"$OUTPUT".v.gold <<"EOF"
module name_of_sram_module(
  input [10:0] RW0A,
  input [0:0] clock,
  output [15:0] RW0O,
  input [15:0] RW0I,
  input [1:0] RW0M,
  input [0:0] RW0W,
  input [0:0] RW0E
);
  wire [7:0] RW0O_0_0;
  wire [7:0] RW0O_0_1;
  wire [15:0] RW0O_0;
  vendor_sram mem_0_0(
    .vendor_clock(clock),
    .vendor_RW0O(RW0O_0_0),
    .vendor_RW0I(RW0I[32'd7:32'd0]),
    .vendor_RW0A(RW0A[32'd10:32'd0]),
    .vendor_RW0M(RW0M[32'd0:32'd0]),
    .vendor_RW0W(RW0W),
    .vendor_RW0E(RW0E)
  );
  vendor_sram mem_0_1(
    .vendor_clock(clock),
    .vendor_RW0O(RW0O_0_1),
    .vendor_RW0I(RW0I[32'd15:32'd8]),
    .vendor_RW0A(RW0A[32'd10:32'd0]),
    .vendor_RW0M(RW0M[32'd1:32'd1]),
    .vendor_RW0W(RW0W),
    .vendor_RW0E(RW0E)
  );
  assign RW0O_0 = {RW0O_0_1[32'd7:32'd0] ,RW0O_0_0[32'd7:32'd0]};
  assign RW0O = ((32'd0 == 32'd0)) ? (RW0O_0) : (16'd0);
endmodule
EOF

#include "_harness.bash"
