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
        "output port name": "R0O",
	"output port polarity": "active high",
        "address port name": "R0A",
	"address port polarity": "active high",
        "chip enable port name": "R0E",
	"chip enable port polarity": "active high"
      },
      {
        "clock port name": "clock",
	"clock port polarity": "positive edge",
	"mask width": 2,
        "input port name": "W1I",
	"input port polarity": "active high",
        "address port name": "W1A",
	"address port polarity": "active high",
        "mask port name": "W1M",
	"mask port polarity": "active high",
        "chip enable port name": "W1E",
	"chip enable port polarity": "active high"
      }
    ]
  }
]
EOF

cat >"$OUTPUT".gold <<"EOF"
module name_of_sram_module(
  input [0:0] clock,
  output [7:0] R0O,
  input [10:0] R0A,
  input [0:0] R0E,
  input [0:0] clock,
  input [7:0] W1I,
  input [10:0] W1A,
  input [0:0] W1E
);
  reg [7:0] mem [2047:0];
  wire [7:0] input_0;
  wire [0:0] write_enable_0;
  wire [7:0] mask_0;
  wire [7:0] read_data_0;
  reg [10:0] address_buffer_0;
  wire [7:0] output_1;
  wire [0:0] write_enable_1;
  wire [7:0] mask_1;
  wire [7:0] read_data_1;
  reg [10:0] address_buffer_1;
  assign write_enable_0 = ~(0);
  assign mask_0 = ~(0);
  assign read_data_0 = mem[address_buffer_0];
  always @(posedge clock)
  begin
    address_buffer_0 <= R0A;
  end
  assign R0O[0] = (read_data_0[0] === 1'bX) ? $random : read_data_0[0];
  assign R0O[1] = (read_data_0[1] === 1'bX) ? $random : read_data_0[1];
  assign R0O[2] = (read_data_0[2] === 1'bX) ? $random : read_data_0[2];
  assign R0O[3] = (read_data_0[3] === 1'bX) ? $random : read_data_0[3];
  assign R0O[4] = (read_data_0[4] === 1'bX) ? $random : read_data_0[4];
  assign R0O[5] = (read_data_0[5] === 1'bX) ? $random : read_data_0[5];
  assign R0O[6] = (read_data_0[6] === 1'bX) ? $random : read_data_0[6];
  assign R0O[7] = (read_data_0[7] === 1'bX) ? $random : read_data_0[7];
  assign write_enable_1 = ~(0);
  assign mask_1 = ~(0);
  assign read_data_1 = mem[address_buffer_1];
  always @(posedge clock)
  begin
    address_buffer_1 <= W1A;
  end
  assign output_1[0] = (read_data_1[0] === 1'bX) ? $random : read_data_1[0];
  assign output_1[1] = (read_data_1[1] === 1'bX) ? $random : read_data_1[1];
  assign output_1[2] = (read_data_1[2] === 1'bX) ? $random : read_data_1[2];
  assign output_1[3] = (read_data_1[3] === 1'bX) ? $random : read_data_1[3];
  assign output_1[4] = (read_data_1[4] === 1'bX) ? $random : read_data_1[4];
  assign output_1[5] = (read_data_1[5] === 1'bX) ? $random : read_data_1[5];
  assign output_1[6] = (read_data_1[6] === 1'bX) ? $random : read_data_1[6];
  assign output_1[7] = (read_data_1[7] === 1'bX) ? $random : read_data_1[7];
  always @(posedge clock)
  begin
    if (((W1E && write_enable_1) && mask_1[0]))
    begin
      mem[W1A][0] <= W1I[0];
    end
    else
    begin
    end
    if (((W1E && write_enable_1) && mask_1[1]))
    begin
      mem[W1A][1] <= W1I[1];
    end
    else
    begin
    end
    if (((W1E && write_enable_1) && mask_1[2]))
    begin
      mem[W1A][2] <= W1I[2];
    end
    else
    begin
    end
    if (((W1E && write_enable_1) && mask_1[3]))
    begin
      mem[W1A][3] <= W1I[3];
    end
    else
    begin
    end
    if (((W1E && write_enable_1) && mask_1[4]))
    begin
      mem[W1A][4] <= W1I[4];
    end
    else
    begin
    end
    if (((W1E && write_enable_1) && mask_1[5]))
    begin
      mem[W1A][5] <= W1I[5];
    end
    else
    begin
    end
    if (((W1E && write_enable_1) && mask_1[6]))
    begin
      mem[W1A][6] <= W1I[6];
    end
    else
    begin
    end
    if (((W1E && write_enable_1) && mask_1[7]))
    begin
      mem[W1A][7] <= W1I[7];
    end
    else
    begin
    end
  end
endmodule
EOF

#include "_harness.bash"
