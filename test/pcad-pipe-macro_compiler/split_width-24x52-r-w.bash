#include "_tempdir.bash"

cat >"$INPUT" <<"EOF"
[
  {
    "type": "sram",
    "name": "entries_info_ext",
    "depth": 24,
    "width": 52,
    "ports": [
      {
        "clock port name": "W0_clk",
        "input port name": "W0_data",
        "address port name": "W0_addr",
        "chip enable port name": "W0_en"
      },
      {
        "clock port name": "R0_clk",
        "output port name": "R0_data",
        "address port name": "R0_addr",
        "chip enable port name": "R0_en"
      }
    ]
  }
]
EOF

LIBS+=("-l" "library.json")
cat >"library.json" <<"EOF"
[
{"depth": 32, "ports": [{"clock port name": "CE1", "read enable port name": "OEB1", "address port name": "A1", "write enable port name": "WEB1", "address port polarity": "active high", "write enable port polarity": "active low", "output port name": "O1", "chip enable port polarity": "active low", "output port polarity": "active high", "input port polarity": "active high", "read enable port polarity": "active low", "clock port polarity": "positive edge", "input port name": "I1", "chip enable port name": "CSB1"}, {"clock port name": "CE2", "read enable port name": "OEB2", "address port name": "A2", "write enable port name": "WEB2", "address port polarity": "active high", "write enable port polarity": "active low", "output port name": "O2", "chip enable port polarity": "active low", "output port polarity": "active high", "input port polarity": "active high", "read enable port polarity": "active low", "clock port polarity": "positive edge", "input port name": "I2", "chip enable port name": "CSB2"}], "name": "SRAM2RW32x32", "family": "2rw", "width": 32, "type": "sram"}
]
EOF

cat >"$OUTPUT".v.gold <<"EOF"
module entries_info_ext(
  input [4:0] W0_addr,
  input [0:0] W0_clk,
  input [51:0] W0_data,
  input [0:0] W0_en,
  input [4:0] R0_addr,
  input [0:0] R0_clk,
  output [51:0] R0_data,
  input [0:0] R0_en
);
  wire [31:0] R0_data_0_0;
  wire [31:0] R0_data_0_1;
  wire [51:0] R0_data_0;
  SRAM2RW32x32 mem_0_0(
    .CE1(W0_clk),
    .I1(W0_data[32'd31:32'd0]),
    .A1(W0_addr[32'd4:32'd0]),
    .OEB1(~(~(1'd1))),
    .WEB1(~((1'd1 && 1'd1))),
    .CSB1(~(W0_en)),
    .CE2(R0_clk),
    .O2(R0_data_0_0),
    .A2(R0_addr[32'd4:32'd0]),
    .OEB2(~(~(1'd0))),
    .WEB2(~((1'd0 && 1'd1))),
    .CSB2(~(R0_en))
  );
  SRAM2RW32x32 mem_0_1(
    .CE1(W0_clk),
    .I1({12'd0 ,W0_data[32'd51:32'd32]}),
    .A1(W0_addr[32'd4:32'd0]),
    .OEB1(~(~(1'd1))),
    .WEB1(~((1'd1 && 1'd1))),
    .CSB1(~(W0_en)),
    .CE2(R0_clk),
    .O2(R0_data_0_1),
    .A2(R0_addr[32'd4:32'd0]),
    .OEB2(~(~(1'd0))),
    .WEB2(~((1'd0 && 1'd1))),
    .CSB2(~(R0_en))
  );
  assign R0_data_0 = {R0_data_0_1[32'd19:32'd0] ,R0_data_0_0[32'd31:32'd0]};
  assign R0_data = ((32'd0 == 32'd0)) ? (R0_data_0) : (52'd0);
endmodule
EOF

#include "_harness.bash"
