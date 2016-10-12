#include "_tempdir.bash"

cat >"$INPUT" <<"EOF"
[
  {
    "name": "name_of_sram_module",
    "depth": 2048,
    "width": 8,
    "ports": [
      {
        "direction": "read+write",
        "clock name": "clock",
        "output port name": "RW0O",
        "input port name": "RW0I",
        "address port name": "RW0A",
        "mask port name": "RW0M",
        "enable port name": "RW0E",
	"write enable port name": "RW0W"
      }
    ]
  }
]
EOF

cat >"$OUTPUT".gold <<"EOF"
module name_of_sram_module(
  input clock,
  output [7:0] RW0O,
  input [7:0] RW0I,
  input RW0W,
  input [10:0] RW0A,
  input RW0E
);
  reg [7:0] mem [2047:0];
  wire [7:0] mask_0;
  reg [7:0] read_data_0;
  assign mask_0 = ~(0);
  always @(posedge clock)
  begin
    read_data_0 <= mem[RW0A];
    RW0O <= read_data_0;
  end
  always @(posedge clock)
  begin
    if (((RW0E & RW0W) & mask_0[0]))
    begin
      mem[RW0A][0] <= RW0I[0];
    end
    else
    begin
    end
    if (((RW0E & RW0W) & mask_0[1]))
    begin
      mem[RW0A][1] <= RW0I[1];
    end
    else
    begin
    end
    if (((RW0E & RW0W) & mask_0[2]))
    begin
      mem[RW0A][2] <= RW0I[2];
    end
    else
    begin
    end
    if (((RW0E & RW0W) & mask_0[3]))
    begin
      mem[RW0A][3] <= RW0I[3];
    end
    else
    begin
    end
    if (((RW0E & RW0W) & mask_0[4]))
    begin
      mem[RW0A][4] <= RW0I[4];
    end
    else
    begin
    end
    if (((RW0E & RW0W) & mask_0[5]))
    begin
      mem[RW0A][5] <= RW0I[5];
    end
    else
    begin
    end
    if (((RW0E & RW0W) & mask_0[6]))
    begin
      mem[RW0A][6] <= RW0I[6];
    end
    else
    begin
    end
    if (((RW0E & RW0W) & mask_0[7]))
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
