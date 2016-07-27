#include "_tempdir.bash"

TOP=memory

cat >"${TOP}".v <<EOF
module memory (input clk, input [7:0] raddr, input [7:0] waddr, input [1:0] data, output [1:0] Q);
  wire [1:0] array [0:127];
  wire [1:0] Q_buf;
  assign Q = Q_buf;
  always @(posedge clk)
  begin
    Q_buf <= array[raddr];
    array[waddr] <= data;
  end
endmodule
EOF

cat >"${TOP}".io.json.gold <<EOF
{
  "name": "memory",
  "ports": [
    {
      "name": "clk",
      "direction": "input",
      "type": "wire",
      "width": "1"
    },
    {
      "name": "raddr",
      "direction": "input",
      "type": "wire",
      "width": "8"
    },
    {
      "name": "waddr",
      "direction": "input",
      "type": "wire",
      "width": "8"
    },
    {
      "name": "data",
      "direction": "input",
      "type": "wire",
      "width": "2"
    },
    {
      "name": "Q",
      "direction": "output",
      "type": "wire",
      "width": "2"
    }
  ]
}
EOF

cat >"${TOP}".decoupled.json.gold <<EOF
{
  "name": "memory",
  "decoupled io": [
  ]
}
EOF

cat >"${TOP}".pcad.json.gold <<EOF
{
  "top": "memory",
  "modules": [
    {
      "name": "memory",
      "ports": [
        {
          "name": "clk",
          "width": "1",
          "depth": "1",
          "direction": "input"
        },
        {
          "name": "raddr",
          "width": "8",
          "depth": "1",
          "direction": "input"
        },
        {
          "name": "waddr",
          "width": "8",
          "depth": "1",
          "direction": "input"
        },
        {
          "name": "data",
          "width": "2",
          "depth": "1",
          "direction": "input"
        },
        {
          "name": "Q",
          "width": "2",
          "depth": "1",
          "direction": "output"
        }
      ],
      "wires": [
        {
          "name": "array",
          "width": "2",
          "depth": "128"
        },
        {
          "name": "Q_buf",
          "width": "2",
          "depth": "1"
        }
      ],
      "statements": [
        {
          "type": "assign",
          "dest": {
            "type": "wire",
            "wire": {
              "name": "Q",
              "width": "2",
              "depth": "1",
              "direction": "output"
            }
          },
          "source": {
            "type": "wire",
            "wire": {
              "name": "Q_buf",
              "width": "2",
              "depth": "1"
            }
          }
        },
        {
          "type": "always",
          "trigger": {
            "type": "edge",
            "edge": "posedge",
            "clock": {
              "name": "clk",
              "width": "1",
              "depth": "1",
              "direction": "input"
            }
          },
          "body": [
            {
              "type": "assign",
              "dest": {
                "type": "wire",
                "wire": {
                  "name": "Q_buf",
                  "width": "2",
                  "depth": "1"
                }
              },
              "source": {
                "type": "slice",
                "source": {
                  "type": "wire",
                  "wire": {
                    "name": "array",
                    "width": "2",
                    "depth": "128"
                  }
                },
                "hi": {
                  "type": "wire",
                  "wire": {
                    "name": "raddr",
                    "width": "8",
                    "depth": "1",
                    "direction": "input"
                  }
                },
                "lo": {
                  "type": "wire",
                  "wire": {
                    "name": "raddr",
                    "width": "8",
                    "depth": "1",
                    "direction": "input"
                  }
                }
              }
            },
            {
              "type": "assign",
              "dest": {
                "type": "slice",
                "source": {
                  "type": "wire",
                  "wire": {
                    "name": "array",
                    "width": "2",
                    "depth": "128"
                  }
                },
                "hi": {
                  "type": "wire",
                  "wire": {
                    "name": "waddr",
                    "width": "8",
                    "depth": "1",
                    "direction": "input"
                  }
                },
                "lo": {
                  "type": "wire",
                  "wire": {
                    "name": "waddr",
                    "width": "8",
                    "depth": "1",
                    "direction": "input"
                  }
                }
              },
              "source": {
                "type": "wire",
                "wire": {
                  "name": "data",
                  "width": "2",
                  "depth": "1",
                  "direction": "input"
                }
              }
            }
          ]
        }
      ],
      "instances": [
      ]
    }
  ]
}
EOF

#include "_harness.bash"
