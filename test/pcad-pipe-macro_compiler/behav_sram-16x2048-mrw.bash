#include "_tempdir.bash"

cat >"$INPUT" <<"EOF"
[
  {
    "name": "name_of_sram_module",
    "depth": 2048,
    "width": 16,
    "ports": [
      {
        "direction": "read+write",
        "clock name": "clock",
        "mask granularity": 8,
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
  output [15:0] RW0O,
  input [15:0] RW0I,
  input RW0W,
  input [7:0] RW0M,
  input [10:0] RW0A,
  input RW0E
);
  reg [15:0] mem [2047:0];
  wire [15:0] read_data_0;
  always @(posedge clock)
  begin
    read_data_0 <= mem[RW0A];
    RW0O <= read_data_0;
  end
  always @(posedge clock)
  begin
    if (((RW0E & RW0W) & RW0M[0]))
    begin
      mem[RW0A][0] <= RW0I[0];
    end
    else
    begin
    end
    if (((RW0E & RW0W) & RW0M[0]))
    begin
      mem[RW0A][1] <= RW0I[1];
    end
    else
    begin
    end
    if (((RW0E & RW0W) & RW0M[0]))
    begin
      mem[RW0A][2] <= RW0I[2];
    end
    else
    begin
    end
    if (((RW0E & RW0W) & RW0M[0]))
    begin
      mem[RW0A][3] <= RW0I[3];
    end
    else
    begin
    end
    if (((RW0E & RW0W) & RW0M[0]))
    begin
      mem[RW0A][4] <= RW0I[4];
    end
    else
    begin
    end
    if (((RW0E & RW0W) & RW0M[0]))
    begin
      mem[RW0A][5] <= RW0I[5];
    end
    else
    begin
    end
    if (((RW0E & RW0W) & RW0M[0]))
    begin
      mem[RW0A][6] <= RW0I[6];
    end
    else
    begin
    end
    if (((RW0E & RW0W) & RW0M[0]))
    begin
      mem[RW0A][7] <= RW0I[7];
    end
    else
    begin
    end
    if (((RW0E & RW0W) & RW0M[1]))
    begin
      mem[RW0A][8] <= RW0I[8];
    end
    else
    begin
    end
    if (((RW0E & RW0W) & RW0M[1]))
    begin
      mem[RW0A][9] <= RW0I[9];
    end
    else
    begin
    end
    if (((RW0E & RW0W) & RW0M[1]))
    begin
      mem[RW0A][10] <= RW0I[10];
    end
    else
    begin
    end
    if (((RW0E & RW0W) & RW0M[1]))
    begin
      mem[RW0A][11] <= RW0I[11];
    end
    else
    begin
    end
    if (((RW0E & RW0W) & RW0M[1]))
    begin
      mem[RW0A][12] <= RW0I[12];
    end
    else
    begin
    end
    if (((RW0E & RW0W) & RW0M[1]))
    begin
      mem[RW0A][13] <= RW0I[13];
    end
    else
    begin
    end
    if (((RW0E & RW0W) & RW0M[1]))
    begin
      mem[RW0A][14] <= RW0I[14];
    end
    else
    begin
    end
    if (((RW0E & RW0W) & RW0M[1]))
    begin
      mem[RW0A][15] <= RW0I[15];
    end
    else
    begin
    end
  end
endmodule
EOF

#include "_harness.bash"
