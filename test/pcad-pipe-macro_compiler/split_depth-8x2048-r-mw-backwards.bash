#include "_tempdir.bash"

cat >"$INPUT" <<"EOF"
[
  {
    "type": "sram",
    "name": "name_of_sram_module",
    "depth": 2048,
    "width": 8,
    "ports": [
      {
        "clock port name": "clock",
	"clock port polarity": "positive edge",
        "mask granularity": 8,
        "input port name": "W0I",
	"input port polarity": "active high",
        "address port name": "W0A",
	"address port polarity": "active high",
        "mask port name": "W0M",
	"mask port polarity": "active high",
        "chip enable port name": "W0E",
	"chip enable port polarity": "active high",
      },
      {
        "clock port name": "clock",
	"clock port polarity": "positive edge",
        "output port name": "R0O",
	"output port polarity": "active high",
        "address port name": "R0A",
	"address port polarity": "active high"
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
    "depth": 1024,
    "width": 8,
    "ports": [
      {
        "clock port name": "clock",
        "mask granularity": 8,
        "output port name": "R0O",
        "address port name": "R0A",
	"clock port polarity": "positive edge",
	"output port polarity": "active high",
	"address port polarity": "active high",
      },
      {
        "clock port name": "clock",
        "mask granularity": 8,
        "input port name": "W0I",
        "address port name": "W0A",
        "mask port name": "W0M",
        "chip enable port name": "W0E",
	"write enable port name": "W0W",
	"clock port polarity": "positive edge",
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

cat > "$OUTPUT".v.gold <<"EOF"
module name_of_sram_module(
  input [10:0] W0A,
  input [0:0] clock,
  input [7:0] W0I,
  input [0:0] W0M,
  input [0:0] W0E,
  input [10:0] R0A,
  output [7:0] R0O
);
  wire [7:0] R0O_0_0;
  wire [7:0] R0O_0;
  wire [7:0] R0O_1_0;
  wire [7:0] R0O_1;
  vendor_sram mem_0_0(
    .clock(clock),
    .W0I(W0I[32'd7:32'd0]),
    .W0A(W0A[32'd9:32'd0]),
    .W0M(W0M[32'd0:32'd0]),
    .W0W((1'd1 && (W0A[32'd10:32'd10] == 1'd0))),
    .W0E((W0E && (W0A[32'd10:32'd10] == 1'd0))),
    .R0O(R0O_0_0),
    .R0A(R0A[32'd9:32'd0])
  );
  vendor_sram mem_1_0(
    .clock(clock),
    .W0I(W0I[32'd7:32'd0]),
    .W0A(W0A[32'd9:32'd0]),
    .W0M(W0M[32'd0:32'd0]),
    .W0W((1'd1 && (W0A[32'd10:32'd10] == 1'd1))),
    .W0E((W0E && (W0A[32'd10:32'd10] == 1'd1))),
    .R0O(R0O_1_0),
    .R0A(R0A[32'd9:32'd0])
  );
  assign R0O_0 = {R0O_0_0[32'd7:32'd0]};
  assign R0O_1 = {R0O_1_0[32'd7:32'd0]};
  assign R0O = ((32'd1 == R0A[32'd10:32'd10])) ? (R0O_1) : (((32'd0 == R0A[32'd10:32'd10])) ? (R0O_0) : (8'd0));
endmodule
EOF

cat > "${OUTPUT}.list.gold" <<EOF
vendor_sram
EOF

#include "_harness.bash"
