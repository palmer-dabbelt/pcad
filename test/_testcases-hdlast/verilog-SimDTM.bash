#include "_tempdir.bash"

TOP=SimDTM

cat >"${TOP}".v <<EOF
// See LICENSE for license details.

import "DPI-C" function int debug_tick
(
  output bit     debug_req_valid,
  input  bit     debug_req_ready,
  output int     debug_req_bits_addr,
  output int     debug_req_bits_op,
  output longint debug_req_bits_data,

  input  bit        debug_resp_valid,
  output bit        debug_resp_ready,
  input  int        debug_resp_bits_resp,
  input  longint    debug_resp_bits_data
);

module SimDTM(
  input clk,
  input reset,

  output        debug_req_valid,
  input         debug_req_ready,
  output [ 4:0] debug_req_bits_addr,
  output [ 1:0] debug_req_bits_op,
  output [33:0] debug_req_bits_data,

  input         debug_resp_valid,
  output        debug_resp_ready,
  input  [ 1:0] debug_resp_bits_resp,
  input  [33:0] debug_resp_bits_data,

  output [31:0] exit
);

  bit r_reset;

  wire #0.1 __debug_req_ready = debug_req_ready;
  wire #0.1 __debug_resp_valid = debug_resp_valid;
  wire [31:0] #0.1 __debug_resp_bits_resp = {30'b0, debug_resp_bits_resp};
  wire [63:0] #0.1 __debug_resp_bits_data = {30'b0, debug_resp_bits_data};

  bit __debug_req_valid;
  int __debug_req_bits_addr;
  int __debug_req_bits_op;
  longint __debug_req_bits_data;
  bit __debug_resp_ready;
  int __exit;

  assign #0.1 debug_req_valid = __debug_req_valid;
  assign #0.1 debug_req_bits_addr = __debug_req_bits_addr[4:0];
  assign #0.1 debug_req_bits_op = __debug_req_bits_op[1:0];
  assign #0.1 debug_req_bits_data = __debug_req_bits_data[33:0];
  assign #0.1 debug_resp_ready = __debug_resp_ready;
  assign #0.1 exit = __exit;

  always @(posedge clk)
  begin
    r_reset <= reset;
    if (reset || r_reset)
    begin
      __debug_req_valid = 0;
      __debug_resp_ready = 0;
      __exit = 0;
    end
    else
    begin
      __exit = debug_tick(
        __debug_req_valid,
        __debug_req_ready,
        __debug_req_bits_addr,
        __debug_req_bits_op,
        __debug_req_bits_data,
        __debug_resp_valid,
        __debug_resp_ready,
        __debug_resp_bits_resp,
        __debug_resp_bits_data
      );
    end
  end
endmodule
EOF

cat > "${TOP}".decoupled.json.gold <<"EOF"
{
  "name": "SimDTM",
  "decoupled io": [
    {
      "base": "debug_req",
      "direction": "output",
      "bits": [
        {
          "name": "debug_req_bits_addr",
          "width": "5"
        },
        {
          "name": "debug_req_bits_op",
          "width": "2"
        },
        {
          "name": "debug_req_bits_data",
          "width": "34"
        }
      ]
    },
    {
      "base": "debug_resp",
      "direction": "input",
      "bits": [
        {
          "name": "debug_resp_bits_resp",
          "width": "2"
        },
        {
          "name": "debug_resp_bits_data",
          "width": "34"
        }
      ]
    }
  ]
}
EOF

cat >"${TOP}".io.json.gold <<"EOF"
{
  "name": "SimDTM",
  "ports": [
    {
      "name": "clk",
      "direction": "input",
      "type": "wire",
      "width": "1"
    },
    {
      "name": "reset",
      "direction": "input",
      "type": "wire",
      "width": "1"
    },
    {
      "name": "debug_req_valid",
      "direction": "output",
      "type": "wire",
      "width": "1"
    },
    {
      "name": "debug_req_ready",
      "direction": "input",
      "type": "wire",
      "width": "1"
    },
    {
      "name": "debug_req_bits_addr",
      "direction": "output",
      "type": "wire",
      "width": "5"
    },
    {
      "name": "debug_req_bits_op",
      "direction": "output",
      "type": "wire",
      "width": "2"
    },
    {
      "name": "debug_req_bits_data",
      "direction": "output",
      "type": "wire",
      "width": "34"
    },
    {
      "name": "debug_resp_valid",
      "direction": "input",
      "type": "wire",
      "width": "1"
    },
    {
      "name": "debug_resp_ready",
      "direction": "output",
      "type": "wire",
      "width": "1"
    },
    {
      "name": "debug_resp_bits_resp",
      "direction": "input",
      "type": "wire",
      "width": "2"
    },
    {
      "name": "debug_resp_bits_data",
      "direction": "input",
      "type": "wire",
      "width": "34"
    },
    {
      "name": "exit",
      "direction": "output",
      "type": "wire",
      "width": "32"
    }
  ]
}
EOF

#include "_harness.bash"
