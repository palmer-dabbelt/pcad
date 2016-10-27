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
	"write enable port polarity": "active high",
	"read enable port name": "RW0W",
	"read enable port polarity": "active low"
      }
    ]
  },
  {
    "name": "another_sram_module",
    "depth": 2048,
    "width": 8,
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
        "mask port name": "RW0M",
	"mask port polarity": "active high",
        "chip enable port name": "RW0E",
	"chip enable port polarity": "active high",
	"write enable port name": "RW0W",
	"write enable port polarity": "active high",
	"read enable port name": "RW0W",
	"read enable port polarity": "active low"
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
  input [0:0] RW0W,
  input [10:0] RW0A,
  input [0:0] RW0E
);
  reg [7:0] mem [2047:0];
  wire [7:0] mask_0;
  wire [7:0] read_data_0;
  reg [10:0] address_buffer_0;
  assign mask_0 = ~(8'd0);
  assign read_data_0 = mem[address_buffer_0];
  always @(posedge clock)
  begin
    address_buffer_0 <= RW0A;
  end
  assign RW0O[8'd0] = (read_data_0[8'd0] === 1'bX) ? $random : read_data_0[8'd0];
  assign RW0O[8'd1] = (read_data_0[8'd1] === 1'bX) ? $random : read_data_0[8'd1];
  assign RW0O[8'd2] = (read_data_0[8'd2] === 1'bX) ? $random : read_data_0[8'd2];
  assign RW0O[8'd3] = (read_data_0[8'd3] === 1'bX) ? $random : read_data_0[8'd3];
  assign RW0O[8'd4] = (read_data_0[8'd4] === 1'bX) ? $random : read_data_0[8'd4];
  assign RW0O[8'd5] = (read_data_0[8'd5] === 1'bX) ? $random : read_data_0[8'd5];
  assign RW0O[8'd6] = (read_data_0[8'd6] === 1'bX) ? $random : read_data_0[8'd6];
  assign RW0O[8'd7] = (read_data_0[8'd7] === 1'bX) ? $random : read_data_0[8'd7];
  always @(posedge clock)
  begin
    if (((RW0E && RW0W) && mask_0[3'd0]))
    begin
      mem[RW0A][3'd0] <= RW0I[3'd0];
    end
    else
    begin
    end
    if (((RW0E && RW0W) && mask_0[3'd1]))
    begin
      mem[RW0A][3'd1] <= RW0I[3'd1];
    end
    else
    begin
    end
    if (((RW0E && RW0W) && mask_0[3'd2]))
    begin
      mem[RW0A][3'd2] <= RW0I[3'd2];
    end
    else
    begin
    end
    if (((RW0E && RW0W) && mask_0[3'd3]))
    begin
      mem[RW0A][3'd3] <= RW0I[3'd3];
    end
    else
    begin
    end
    if (((RW0E && RW0W) && mask_0[3'd4]))
    begin
      mem[RW0A][3'd4] <= RW0I[3'd4];
    end
    else
    begin
    end
    if (((RW0E && RW0W) && mask_0[3'd5]))
    begin
      mem[RW0A][3'd5] <= RW0I[3'd5];
    end
    else
    begin
    end
    if (((RW0E && RW0W) && mask_0[3'd6]))
    begin
      mem[RW0A][3'd6] <= RW0I[3'd6];
    end
    else
    begin
    end
    if (((RW0E && RW0W) && mask_0[3'd7]))
    begin
      mem[RW0A][3'd7] <= RW0I[3'd7];
    end
    else
    begin
    end
  end
endmodule
module another_sram_module(
  input [0:0] clock,
  output [7:0] RW0O,
  input [7:0] RW0I,
  input [0:0] RW0W,
  input [10:0] RW0A,
  input [0:0] RW0E
);
  reg [7:0] mem [2047:0];
  wire [7:0] mask_0;
  wire [7:0] read_data_0;
  reg [10:0] address_buffer_0;
  assign mask_0 = ~(8'd0);
  assign read_data_0 = mem[address_buffer_0];
  always @(posedge clock)
  begin
    address_buffer_0 <= RW0A;
  end
  assign RW0O[8'd0] = (read_data_0[8'd0] === 1'bX) ? $random : read_data_0[8'd0];
  assign RW0O[8'd1] = (read_data_0[8'd1] === 1'bX) ? $random : read_data_0[8'd1];
  assign RW0O[8'd2] = (read_data_0[8'd2] === 1'bX) ? $random : read_data_0[8'd2];
  assign RW0O[8'd3] = (read_data_0[8'd3] === 1'bX) ? $random : read_data_0[8'd3];
  assign RW0O[8'd4] = (read_data_0[8'd4] === 1'bX) ? $random : read_data_0[8'd4];
  assign RW0O[8'd5] = (read_data_0[8'd5] === 1'bX) ? $random : read_data_0[8'd5];
  assign RW0O[8'd6] = (read_data_0[8'd6] === 1'bX) ? $random : read_data_0[8'd6];
  assign RW0O[8'd7] = (read_data_0[8'd7] === 1'bX) ? $random : read_data_0[8'd7];
  always @(posedge clock)
  begin
    if (((RW0E && RW0W) && mask_0[3'd0]))
    begin
      mem[RW0A][3'd0] <= RW0I[3'd0];
    end
    else
    begin
    end
    if (((RW0E && RW0W) && mask_0[3'd1]))
    begin
      mem[RW0A][3'd1] <= RW0I[3'd1];
    end
    else
    begin
    end
    if (((RW0E && RW0W) && mask_0[3'd2]))
    begin
      mem[RW0A][3'd2] <= RW0I[3'd2];
    end
    else
    begin
    end
    if (((RW0E && RW0W) && mask_0[3'd3]))
    begin
      mem[RW0A][3'd3] <= RW0I[3'd3];
    end
    else
    begin
    end
    if (((RW0E && RW0W) && mask_0[3'd4]))
    begin
      mem[RW0A][3'd4] <= RW0I[3'd4];
    end
    else
    begin
    end
    if (((RW0E && RW0W) && mask_0[3'd5]))
    begin
      mem[RW0A][3'd5] <= RW0I[3'd5];
    end
    else
    begin
    end
    if (((RW0E && RW0W) && mask_0[3'd6]))
    begin
      mem[RW0A][3'd6] <= RW0I[3'd6];
    end
    else
    begin
    end
    if (((RW0E && RW0W) && mask_0[3'd7]))
    begin
      mem[RW0A][3'd7] <= RW0I[3'd7];
    end
    else
    begin
    end
  end
endmodule
EOF

#include "_harness.bash"
