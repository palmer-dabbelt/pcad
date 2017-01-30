#include "_tempdir.bash"

COMPILER_ARGS+=("--syn-flops")

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
	"clock port polarity": "positive edge",
        "output port name": "RW0O",
	"output port polarity": "active high",
        "input port name": "RW0I",
	"input port polarity": "active high",
        "address port name": "RW0A",
	"address port polarity": "active high",
        "mask granularity": 8,
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

cat >"$OUTPUT".v.gold <<"EOF"
module name_of_sram_module(
  input [0:0] clock,
  output [15:0] RW0O,
  input [15:0] RW0I,
  input [0:0] RW0W,
  input [1:0] RW0M,
  input [10:0] RW0A,
  input [0:0] RW0E
);
  reg [15:0] mem [2047:0];
  wire [15:0] read_data_0;
  reg [10:0] address_buffer_0;
  assign read_data_0 = mem[address_buffer_0];
  always @(posedge clock)
  begin
    address_buffer_0 <= RW0A;
  end
  assign RW0O = read_data_0;
  always @(posedge clock)
  begin
    if (((RW0E && RW0W) && RW0M[4'd0]))
    begin
      mem[RW0A][4'd0] <= RW0I[4'd0];
    end
    else
    begin
    end
    if (((RW0E && RW0W) && RW0M[4'd0]))
    begin
      mem[RW0A][4'd1] <= RW0I[4'd1];
    end
    else
    begin
    end
    if (((RW0E && RW0W) && RW0M[4'd0]))
    begin
      mem[RW0A][4'd2] <= RW0I[4'd2];
    end
    else
    begin
    end
    if (((RW0E && RW0W) && RW0M[4'd0]))
    begin
      mem[RW0A][4'd3] <= RW0I[4'd3];
    end
    else
    begin
    end
    if (((RW0E && RW0W) && RW0M[4'd0]))
    begin
      mem[RW0A][4'd4] <= RW0I[4'd4];
    end
    else
    begin
    end
    if (((RW0E && RW0W) && RW0M[4'd0]))
    begin
      mem[RW0A][4'd5] <= RW0I[4'd5];
    end
    else
    begin
    end
    if (((RW0E && RW0W) && RW0M[4'd0]))
    begin
      mem[RW0A][4'd6] <= RW0I[4'd6];
    end
    else
    begin
    end
    if (((RW0E && RW0W) && RW0M[4'd0]))
    begin
      mem[RW0A][4'd7] <= RW0I[4'd7];
    end
    else
    begin
    end
    if (((RW0E && RW0W) && RW0M[4'd1]))
    begin
      mem[RW0A][4'd8] <= RW0I[4'd8];
    end
    else
    begin
    end
    if (((RW0E && RW0W) && RW0M[4'd1]))
    begin
      mem[RW0A][4'd9] <= RW0I[4'd9];
    end
    else
    begin
    end
    if (((RW0E && RW0W) && RW0M[4'd1]))
    begin
      mem[RW0A][4'd10] <= RW0I[4'd10];
    end
    else
    begin
    end
    if (((RW0E && RW0W) && RW0M[4'd1]))
    begin
      mem[RW0A][4'd11] <= RW0I[4'd11];
    end
    else
    begin
    end
    if (((RW0E && RW0W) && RW0M[4'd1]))
    begin
      mem[RW0A][4'd12] <= RW0I[4'd12];
    end
    else
    begin
    end
    if (((RW0E && RW0W) && RW0M[4'd1]))
    begin
      mem[RW0A][4'd13] <= RW0I[4'd13];
    end
    else
    begin
    end
    if (((RW0E && RW0W) && RW0M[4'd1]))
    begin
      mem[RW0A][4'd14] <= RW0I[4'd14];
    end
    else
    begin
    end
    if (((RW0E && RW0W) && RW0M[4'd1]))
    begin
      mem[RW0A][4'd15] <= RW0I[4'd15];
    end
    else
    begin
    end
  end
endmodule
EOF

#include "_harness.bash"
