#include "_tempdir.bash"

cat >"${TOP}".v <<EOF
module Top(
  input A,
  input B,
  output Q
);
  wire _A;
  wire _B;
  wire _Q;
  assign _A = A;
  assign _B = B;
  AndGate AndGate(
    .A(_A),
    .B(_B),
    .Q(_Q)
  );
  assign Q = _Q;
endmodule

module AndGate(
  input A,
  input B,
  output Q
);
  assign Q = A & B;
endmodule
EOF

cat >"${TOP}.decoupled.json.gold" <<EOF
{
  "name": "Top",
  "decoupled io": [
  ]
}
EOF

cat >"${TOP}.io.json.gold" <<EOF
{
  "name": "Top",
  "ports": [
    {
      "name": "A",
      "direction": "input",
      "type": "wire",
      "width": "1"
    },
    {
      "name": "B",
      "direction": "input",
      "type": "wire",
      "width": "1"
    },
    {
      "name": "Q",
      "direction": "output",
      "type": "wire",
      "width": "1"
    }
  ]
}
EOF

cat > "${TOP}.pcad.json.gold" <<EOF
{
  "top": "Top",
  "modules": [
    {
      "name": "Top",
      "ports": [
        {
          "name": "A",
          "width": "1",
          "depth": "1",
          "direction": "input"
        },
        {
          "name": "B",
          "width": "1",
          "depth": "1",
          "direction": "input"
        },
        {
          "name": "Q",
          "width": "1",
          "depth": "1",
          "direction": "output"
        }
      ],
      "wires": [
        {
          "name": "_A",
          "width": "1",
          "depth": "1"
        },
        {
          "name": "_B",
          "width": "1",
          "depth": "1"
        },
        {
          "name": "_Q",
          "width": "1",
          "depth": "1"
        }
      ],
      "statements": [
        {
          "type": "assign",
          "dest": {
            "type": "wire",
            "wire": {
              "name": "_A",
              "width": "1",
              "depth": "1"
            }
          },
          "source": {
            "type": "wire",
            "wire": {
              "name": "A",
              "width": "1",
              "depth": "1",
              "direction": "input"
            }
          }
        },
        {
          "type": "assign",
          "dest": {
            "type": "wire",
            "wire": {
              "name": "_B",
              "width": "1",
              "depth": "1"
            }
          },
          "source": {
            "type": "wire",
            "wire": {
              "name": "B",
              "width": "1",
              "depth": "1",
              "direction": "input"
            }
          }
        },
        {
          "type": "assign",
          "dest": {
            "type": "wire",
            "wire": {
              "name": "Q",
              "width": "1",
              "depth": "1",
              "direction": "output"
            }
          },
          "source": {
            "type": "wire",
            "wire": {
              "name": "_Q",
              "width": "1",
              "depth": "1"
            }
          }
        }
      ],
      "instances": [
        {
          "name": "AndGate",
          "module name": "AndGate",
          "assignments": [
            {
              "type": "assign",
              "dest": {
                "type": "wire",
                "wire": {
                  "name": "A",
                  "width": "1",
                  "depth": "1",
                  "direction": "input"
                }
              },
              "source": {
                "type": "wire",
                "wire": {
                  "name": "_A",
                  "width": "1",
                  "depth": "1"
                }
              }
            },
            {
              "type": "assign",
              "dest": {
                "type": "wire",
                "wire": {
                  "name": "B",
                  "width": "1",
                  "depth": "1",
                  "direction": "input"
                }
              },
              "source": {
                "type": "wire",
                "wire": {
                  "name": "_B",
                  "width": "1",
                  "depth": "1"
                }
              }
            },
            {
              "type": "assign",
              "dest": {
                "type": "wire",
                "wire": {
                  "name": "Q",
                  "width": "1",
                  "depth": "1",
                  "direction": "output"
                }
              },
              "source": {
                "type": "wire",
                "wire": {
                  "name": "_Q",
                  "width": "1",
                  "depth": "1"
                }
              }
            }
          ]
        }
      ]
    },
    {
      "name": "AndGate",
      "ports": [
        {
          "name": "A",
          "width": "1",
          "depth": "1",
          "direction": "input"
        },
        {
          "name": "B",
          "width": "1",
          "depth": "1",
          "direction": "input"
        },
        {
          "name": "Q",
          "width": "1",
          "depth": "1",
          "direction": "output"
        }
      ],
      "wires": [
      ],
      "statements": [
        {
          "type": "assign",
          "dest": {
            "type": "wire",
            "wire": {
              "name": "Q",
              "width": "1",
              "depth": "1",
              "direction": "output"
            }
          },
          "source": {
            "type": "biop",
            "op": "AND",
            "a": {
              "type": "wire",
              "wire": {
                "name": "A",
                "width": "1",
                "depth": "1",
                "direction": "input"
              }
            },
            "b": {
              "type": "wire",
              "wire": {
                "name": "B",
                "width": "1",
                "depth": "1",
                "direction": "input"
              }
            }
          }
        }
      ],
      "instances": [
      ]
    }
  ]
}
EOF

#include "_harness.bash"
