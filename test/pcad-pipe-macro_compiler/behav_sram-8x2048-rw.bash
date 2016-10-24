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
  assign mask_0 = ~(0);
  assign read_data_0 = mem[address_buffer_0];
  always @(posedge clock)
  begin
    address_buffer_0 <= RW0A;
  end
  assign RW0O[0] = (read_data_0[0] === 1'bX) ? $random : read_data_0[0];
  assign RW0O[1] = (read_data_0[1] === 1'bX) ? $random : read_data_0[1];
  assign RW0O[2] = (read_data_0[2] === 1'bX) ? $random : read_data_0[2];
  assign RW0O[3] = (read_data_0[3] === 1'bX) ? $random : read_data_0[3];
  assign RW0O[4] = (read_data_0[4] === 1'bX) ? $random : read_data_0[4];
  assign RW0O[5] = (read_data_0[5] === 1'bX) ? $random : read_data_0[5];
  assign RW0O[6] = (read_data_0[6] === 1'bX) ? $random : read_data_0[6];
  assign RW0O[7] = (read_data_0[7] === 1'bX) ? $random : read_data_0[7];
  always @(posedge clock)
  begin
    if (((RW0E && RW0W) && mask_0[0]))
    begin
      mem[RW0A][0] <= RW0I[0];
    end
    else
    begin
    end
    if (((RW0E && RW0W) && mask_0[1]))
    begin
      mem[RW0A][1] <= RW0I[1];
    end
    else
    begin
    end
    if (((RW0E && RW0W) && mask_0[2]))
    begin
      mem[RW0A][2] <= RW0I[2];
    end
    else
    begin
    end
    if (((RW0E && RW0W) && mask_0[3]))
    begin
      mem[RW0A][3] <= RW0I[3];
    end
    else
    begin
    end
    if (((RW0E && RW0W) && mask_0[4]))
    begin
      mem[RW0A][4] <= RW0I[4];
    end
    else
    begin
    end
    if (((RW0E && RW0W) && mask_0[5]))
    begin
      mem[RW0A][5] <= RW0I[5];
    end
    else
    begin
    end
    if (((RW0E && RW0W) && mask_0[6]))
    begin
      mem[RW0A][6] <= RW0I[6];
    end
    else
    begin
    end
    if (((RW0E && RW0W) && mask_0[7]))
    begin
      mem[RW0A][7] <= RW0I[7];
    end
    else
    begin
    end
  end
endmodule
EOF

#include "_harness.bash"
