#include "_tempdir.bash"

cat >"$INPUT" <<"EOF"
[
  {
    "name": "name_of_sram_module",
    "depth": 2048,
    "width": 20,
    "ports": [
      {
        "clock port name": "clock",
	"clock port polarity": "positive edge",
        "mask granularity": 10,
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
    "depth": 2048,
    "width": 8,
    "ports": [
      {
        "clock port name": "clock",
        "output port name": "RW0O",
        "input port name": "RW0I",
        "address port name": "RW0A",
        "chip enable port name": "RW0E",
	"write enable port name": "RW0W",
	"clock port polarity": "positive edge",
	"output port polarity": "active high",
	"input port polarity": "active high",
	"address port polarity": "active high",
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
  output [19:0] RW0O,
  input [19:0] RW0I,
  input [10:0] RW0A,
  input [1:0] RW0M,
  input [0:0] RW0E,
  input [0:0] RW0W
);
  wire [7:0] RW0O_0_0;
  wire [7:0] RW0I_0_0;
  wire [10:0] RW0A_0_0;
  wire [0:0] RW0M_0_0_AND_RW0E;
  wire [7:0] RW0O_0_1;
  wire [7:0] RW0I_0_1;
  wire [10:0] RW0A_0_1;
  wire [0:0] RW0M_0_1_AND_RW0E;
  wire [7:0] RW0O_0_2;
  wire [7:0] RW0I_0_2;
  wire [10:0] RW0A_0_2;
  wire [0:0] RW0M_0_2_AND_RW0E;
  wire [7:0] RW0O_0_3;
  wire [7:0] RW0I_0_3;
  wire [10:0] RW0A_0_3;
  wire [0:0] RW0M_0_3_AND_RW0E;
  vendor_sram mem_0_0(
    .clock(clock),
    .RW0O(RW0O_0_0),
    .RW0I(RW0I_0_0),
    .RW0A(RW0A_0_0),
    .RW0E(RW0M_0_0_AND_RW0E),
    .RW0W(RW0W)
  );
  vendor_sram mem_0_1(
    .clock(clock),
    .RW0O(RW0O_0_1),
    .RW0I(RW0I_0_1),
    .RW0A(RW0A_0_1),
    .RW0E(RW0M_0_1_AND_RW0E),
    .RW0W(RW0W)
  );
  vendor_sram mem_0_2(
    .clock(clock),
    .RW0O(RW0O_0_2),
    .RW0I(RW0I_0_2),
    .RW0A(RW0A_0_2),
    .RW0E(RW0M_0_2_AND_RW0E),
    .RW0W(RW0W)
  );
  vendor_sram mem_0_3(
    .clock(clock),
    .RW0O(RW0O_0_3),
    .RW0I(RW0I_0_3),
    .RW0A(RW0A_0_3),
    .RW0E(RW0M_0_3_AND_RW0E),
    .RW0W(RW0W)
  );
  assign RW0I_0_0 = RW0I[32'd7:32'd0];
  assign RW0A_0_0 = RW0A[32'd10:32'd0];
  assign RW0M_0_0_AND_RW0E = (RW0M[32'd0:32'd0] && RW0E);
  assign RW0I_0_1 = RW0I[32'd9:32'd8];
  assign RW0A_0_1 = RW0A[32'd10:32'd0];
  assign RW0M_0_1_AND_RW0E = (RW0M[32'd1:32'd1] && RW0E);
  assign RW0I_0_2 = RW0I[32'd17:32'd10];
  assign RW0A_0_2 = RW0A[32'd10:32'd0];
  assign RW0M_0_2_AND_RW0E = (RW0M[32'd2:32'd2] && RW0E);
  assign RW0I_0_3 = RW0I[32'd19:32'd18];
  assign RW0A_0_3 = RW0A[32'd10:32'd0];
  assign RW0M_0_3_AND_RW0E = (RW0M[32'd3:32'd3] && RW0E);
  assign RW0O = {RW0O_0_3[32'd1:32'd0] ,RW0O_0_2[32'd7:32'd0] ,RW0O_0_1[32'd1:32'd0] ,RW0O_0_0[32'd7:32'd0]};
endmodule
EOF

#include "_harness.bash"
