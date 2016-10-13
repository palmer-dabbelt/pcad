#include "_tempdir.bash"

cat >"$INPUT" <<"EOF"
[
  {
    "name": "name_of_sram_module",
    "depth": 2048,
    "width": 8,
    "ports": [
      {
        "direction": "read",
        "clock name": "clock",
        "output port name": "R0O",
        "address port name": "R0A",
        "enable port name": "R0E"
      },
      {
        "direction": "write",
        "clock name": "clock",
	"mask width": 2,
        "input port name": "W1I",
        "address port name": "W1A",
        "mask port name": "W1M",
        "enable port name": "W1E"
      }
    ]
  }
]
EOF

cat >"$OUTPUT".gold <<"EOF"
module name_of_sram_module(
  input clock,
  output [7:0] R0O,
  input [10:0] R0A,
  input R0E,
  input clock,
  input [7:0] W1I,
  input [10:0] W1A,
  input W1E
);
  reg [7:0] mem [2047:0];
  wire [7:0] input_0;
  wire write_enable_0;
  wire [7:0] mask_0;
  reg [7:0] read_data_0;
  reg [10:0] address_buffer_0;
  wire [7:0] output_1;
  wire write_enable_1;
  wire [7:0] mask_1;
  reg [7:0] read_data_1;
  reg [10:0] address_buffer_1;
  assign write_enable_0 = ~(0);
  assign mask_0 = ~(0);
  assign read_data_0 = mem[address_buffer_0];
  always @(posedge clock)
  begin
    address_buffer_0 <= R0A;
  end
  assign R0O = read_data_0;
  assign write_enable_1 = ~(0);
  assign mask_1 = ~(0);
  assign read_data_1 = mem[address_buffer_1];
  always @(posedge clock)
  begin
    address_buffer_1 <= W1A;
  end
  assign output_1 = read_data_1;
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
