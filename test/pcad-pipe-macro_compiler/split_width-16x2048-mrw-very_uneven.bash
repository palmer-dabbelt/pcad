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
  input [0:0] clock,
  output [15:0] RW0O,
  input [15:0] RW0I,
  input [10:0] RW0A,
  input [7:0] RW0M,
  input [0:0] RW0E,
  input [0:0] RW0W
);
  wire [9:0] RW0O_0_0;
  wire [9:0] RW0I_0_0;
  wire [10:0] RW0A_0_0;
  wire [0:0] RW0M_0_0_AND_RW0E;
  wire [9:0] RW0O_0_1;
  wire [9:0] RW0I_0_1;
  wire [10:0] RW0A_0_1;
  wire [0:0] RW0M_0_1_AND_RW0E;
  wire [9:0] RW0O_0_2;
  wire [9:0] RW0I_0_2;
  wire [10:0] RW0A_0_2;
  wire [0:0] RW0M_0_2_AND_RW0E;
  wire [9:0] RW0O_0_3;
  wire [9:0] RW0I_0_3;
  wire [10:0] RW0A_0_3;
  wire [0:0] RW0M_0_3_AND_RW0E;
  wire [9:0] RW0O_0_4;
  wire [9:0] RW0I_0_4;
  wire [10:0] RW0A_0_4;
  wire [0:0] RW0M_0_4_AND_RW0E;
  wire [9:0] RW0O_0_5;
  wire [9:0] RW0I_0_5;
  wire [10:0] RW0A_0_5;
  wire [0:0] RW0M_0_5_AND_RW0E;
  wire [9:0] RW0O_0_6;
  wire [9:0] RW0I_0_6;
  wire [10:0] RW0A_0_6;
  wire [0:0] RW0M_0_6_AND_RW0E;
  wire [9:0] RW0O_0_7;
  wire [9:0] RW0I_0_7;
  wire [10:0] RW0A_0_7;
  wire [0:0] RW0M_0_7_AND_RW0E;
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
  vendor_sram mem_0_4(
    .clock(clock),
    .RW0O(RW0O_0_4),
    .RW0I(RW0I_0_4),
    .RW0A(RW0A_0_4),
    .RW0E(RW0M_0_4_AND_RW0E),
    .RW0W(RW0W)
  );
  vendor_sram mem_0_5(
    .clock(clock),
    .RW0O(RW0O_0_5),
    .RW0I(RW0I_0_5),
    .RW0A(RW0A_0_5),
    .RW0E(RW0M_0_5_AND_RW0E),
    .RW0W(RW0W)
  );
  vendor_sram mem_0_6(
    .clock(clock),
    .RW0O(RW0O_0_6),
    .RW0I(RW0I_0_6),
    .RW0A(RW0A_0_6),
    .RW0E(RW0M_0_6_AND_RW0E),
    .RW0W(RW0W)
  );
  vendor_sram mem_0_7(
    .clock(clock),
    .RW0O(RW0O_0_7),
    .RW0I(RW0I_0_7),
    .RW0A(RW0A_0_7),
    .RW0E(RW0M_0_7_AND_RW0E),
    .RW0W(RW0W)
  );
  assign RW0I_0_0 = RW0I[1:0];
  assign RW0A_0_0 = RW0A[10:0];
  assign RW0M_0_0_AND_RW0E = (RW0M[0:0] && RW0E);
  assign RW0I_0_1 = RW0I[3:2];
  assign RW0A_0_1 = RW0A[10:0];
  assign RW0M_0_1_AND_RW0E = (RW0M[1:1] && RW0E);
  assign RW0I_0_2 = RW0I[5:4];
  assign RW0A_0_2 = RW0A[10:0];
  assign RW0M_0_2_AND_RW0E = (RW0M[2:2] && RW0E);
  assign RW0I_0_3 = RW0I[7:6];
  assign RW0A_0_3 = RW0A[10:0];
  assign RW0M_0_3_AND_RW0E = (RW0M[3:3] && RW0E);
  assign RW0I_0_4 = RW0I[9:8];
  assign RW0A_0_4 = RW0A[10:0];
  assign RW0M_0_4_AND_RW0E = (RW0M[4:4] && RW0E);
  assign RW0I_0_5 = RW0I[11:10];
  assign RW0A_0_5 = RW0A[10:0];
  assign RW0M_0_5_AND_RW0E = (RW0M[5:5] && RW0E);
  assign RW0I_0_6 = RW0I[13:12];
  assign RW0A_0_6 = RW0A[10:0];
  assign RW0M_0_6_AND_RW0E = (RW0M[6:6] && RW0E);
  assign RW0I_0_7 = RW0I[15:14];
  assign RW0A_0_7 = RW0A[10:0];
  assign RW0M_0_7_AND_RW0E = (RW0M[7:7] && RW0E);
  assign RW0O = {RW0O_0_7[1:0] ,RW0O_0_6[1:0] ,RW0O_0_5[1:0] ,RW0O_0_4[1:0] ,RW0O_0_3[1:0] ,RW0O_0_2[1:0] ,RW0O_0_1[1:0] ,RW0O_0_0[1:0]};
endmodule
EOF

#include "_harness.bash"
