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
  assign write_enable_0 = ~(1'd0);
  assign mask_0 = ~(8'd0);
  assign read_data_0 = mem[address_buffer_0];
  always @(posedge clock)
  begin
    address_buffer_0 <= R0A;
  end
  assign R0O[8'd0] = ((read_data_0[8'd0] === 1'bX)) ? ($random) : (read_data_0[8'd0]);
  assign R0O[8'd1] = ((read_data_0[8'd1] === 1'bX)) ? ($random) : (read_data_0[8'd1]);
  assign R0O[8'd2] = ((read_data_0[8'd2] === 1'bX)) ? ($random) : (read_data_0[8'd2]);
  assign R0O[8'd3] = ((read_data_0[8'd3] === 1'bX)) ? ($random) : (read_data_0[8'd3]);
  assign R0O[8'd4] = ((read_data_0[8'd4] === 1'bX)) ? ($random) : (read_data_0[8'd4]);
  assign R0O[8'd5] = ((read_data_0[8'd5] === 1'bX)) ? ($random) : (read_data_0[8'd5]);
  assign R0O[8'd6] = ((read_data_0[8'd6] === 1'bX)) ? ($random) : (read_data_0[8'd6]);
  assign R0O[8'd7] = ((read_data_0[8'd7] === 1'bX)) ? ($random) : (read_data_0[8'd7]);
  assign write_enable_1 = ~(1'd0);
  assign mask_1 = ~(8'd0);
  assign read_data_1 = mem[address_buffer_1];
  always @(posedge clock)
  begin
    address_buffer_1 <= W1A;
  end
  assign output_1[8'd0] = ((read_data_1[8'd0] === 1'bX)) ? ($random) : (read_data_1[8'd0]);
  assign output_1[8'd1] = ((read_data_1[8'd1] === 1'bX)) ? ($random) : (read_data_1[8'd1]);
  assign output_1[8'd2] = ((read_data_1[8'd2] === 1'bX)) ? ($random) : (read_data_1[8'd2]);
  assign output_1[8'd3] = ((read_data_1[8'd3] === 1'bX)) ? ($random) : (read_data_1[8'd3]);
  assign output_1[8'd4] = ((read_data_1[8'd4] === 1'bX)) ? ($random) : (read_data_1[8'd4]);
  assign output_1[8'd5] = ((read_data_1[8'd5] === 1'bX)) ? ($random) : (read_data_1[8'd5]);
  assign output_1[8'd6] = ((read_data_1[8'd6] === 1'bX)) ? ($random) : (read_data_1[8'd6]);
  assign output_1[8'd7] = ((read_data_1[8'd7] === 1'bX)) ? ($random) : (read_data_1[8'd7]);
  always @(posedge clock)
  begin
    if (((W1E && write_enable_1) && mask_1[3'd0]))
    begin
      mem[W1A][3'd0] <= W1I[3'd0];
    end
    else
    begin
    end
    if (((W1E && write_enable_1) && mask_1[3'd1]))
    begin
      mem[W1A][3'd1] <= W1I[3'd1];
    end
    else
    begin
    end
    if (((W1E && write_enable_1) && mask_1[3'd2]))
    begin
      mem[W1A][3'd2] <= W1I[3'd2];
    end
    else
    begin
    end
    if (((W1E && write_enable_1) && mask_1[3'd3]))
    begin
      mem[W1A][3'd3] <= W1I[3'd3];
    end
    else
    begin
    end
    if (((W1E && write_enable_1) && mask_1[3'd4]))
    begin
      mem[W1A][3'd4] <= W1I[3'd4];
    end
    else
    begin
    end
    if (((W1E && write_enable_1) && mask_1[3'd5]))
    begin
      mem[W1A][3'd5] <= W1I[3'd5];
    end
    else
    begin
    end
    if (((W1E && write_enable_1) && mask_1[3'd6]))
    begin
      mem[W1A][3'd6] <= W1I[3'd6];
    end
    else
    begin
    end
    if (((W1E && write_enable_1) && mask_1[3'd7]))
    begin
      mem[W1A][3'd7] <= W1I[3'd7];
    end
    else
    begin
    end
  end
endmodule
EOF

#include "_harness.bash"
