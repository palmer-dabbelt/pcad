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
	"clock port polarity": "positive edge",
        "mask granularity": 2,
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
    "width": 10,
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
  input [10:0] RW0A,
  input [0:0] clock,
  output [15:0] RW0O,
  input [15:0] RW0I,
  input [7:0] RW0M,
  input [0:0] RW0W,
  input [0:0] RW0E
);
  wire [9:0] RW0O_0_0;
  wire [9:0] RW0O_0_1;
  wire [9:0] RW0O_0_2;
  wire [9:0] RW0O_0_3;
  wire [9:0] RW0O_0_4;
  wire [9:0] RW0O_0_5;
  wire [9:0] RW0O_0_6;
  wire [9:0] RW0O_0_7;
  wire [15:0] RW0O_0;
  vendor_sram mem_0_0(
    .clock(clock),
    .RW0O(RW0O_0_0),
    .RW0I(RW0I[32'd1:32'd0]),
    .RW0A(RW0A[32'd10:32'd0]),
    .RW0W((RW0W && RW0M[32'd0:32'd0])),
    .RW0E(RW0E)
  );
  vendor_sram mem_0_1(
    .clock(clock),
    .RW0O(RW0O_0_1),
    .RW0I(RW0I[32'd3:32'd2]),
    .RW0A(RW0A[32'd10:32'd0]),
    .RW0W((RW0W && RW0M[32'd1:32'd1])),
    .RW0E(RW0E)
  );
  vendor_sram mem_0_2(
    .clock(clock),
    .RW0O(RW0O_0_2),
    .RW0I(RW0I[32'd5:32'd4]),
    .RW0A(RW0A[32'd10:32'd0]),
    .RW0W((RW0W && RW0M[32'd2:32'd2])),
    .RW0E(RW0E)
  );
  vendor_sram mem_0_3(
    .clock(clock),
    .RW0O(RW0O_0_3),
    .RW0I(RW0I[32'd7:32'd6]),
    .RW0A(RW0A[32'd10:32'd0]),
    .RW0W((RW0W && RW0M[32'd3:32'd3])),
    .RW0E(RW0E)
  );
  vendor_sram mem_0_4(
    .clock(clock),
    .RW0O(RW0O_0_4),
    .RW0I(RW0I[32'd9:32'd8]),
    .RW0A(RW0A[32'd10:32'd0]),
    .RW0W((RW0W && RW0M[32'd4:32'd4])),
    .RW0E(RW0E)
  );
  vendor_sram mem_0_5(
    .clock(clock),
    .RW0O(RW0O_0_5),
    .RW0I(RW0I[32'd11:32'd10]),
    .RW0A(RW0A[32'd10:32'd0]),
    .RW0W((RW0W && RW0M[32'd5:32'd5])),
    .RW0E(RW0E)
  );
  vendor_sram mem_0_6(
    .clock(clock),
    .RW0O(RW0O_0_6),
    .RW0I(RW0I[32'd13:32'd12]),
    .RW0A(RW0A[32'd10:32'd0]),
    .RW0W((RW0W && RW0M[32'd6:32'd6])),
    .RW0E(RW0E)
  );
  vendor_sram mem_0_7(
    .clock(clock),
    .RW0O(RW0O_0_7),
    .RW0I(RW0I[32'd15:32'd14]),
    .RW0A(RW0A[32'd10:32'd0]),
    .RW0W((RW0W && RW0M[32'd7:32'd7])),
    .RW0E(RW0E)
  );
  assign RW0O_0 = {RW0O_0_7[32'd1:32'd0] ,RW0O_0_6[32'd1:32'd0] ,RW0O_0_5[32'd1:32'd0] ,RW0O_0_4[32'd1:32'd0] ,RW0O_0_3[32'd1:32'd0] ,RW0O_0_2[32'd1:32'd0] ,RW0O_0_1[32'd1:32'd0] ,RW0O_0_0[32'd1:32'd0]};
  assign RW0O = ((32'd0 == 32'd0)) ? (RW0O_0) : (16'd0);
endmodule
EOF

#include "_harness.bash"
