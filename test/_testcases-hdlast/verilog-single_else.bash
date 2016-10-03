#include "_tempdir.bash"

TOP=ifelse

cat >"${TOP}".v <<EOF
module ifelse (input clk, input a, input b, output c);
  always @(posedge clk)
  begin
    if (a)
    begin
      c <= b;
      c <= b;
    end
    else
      c <= ~b;
  end
endmodule
EOF

cat >"${TOP}".v <<EOF
module ifelse (
  input clk,
  input a,
  input b,
  output c
);
  always @(posedge clk)
  begin
    if (a)
    begin
      c <= b;
      c <= b;
    end
    else
    begin
      c <= ~b;
    end
  end
endmodule
EOF

cat >"${TOP}".io.json.gold <<EOF
{
  "name": "ifelse",
  "ports": [
    {
      "name": "clk",
      "direction": "input",
      "type": "wire",
      "width": "1"
    },
    {
      "name": "a",
      "direction": "input",
      "type": "wire",
      "width": "1"
    },
    {
      "name": "b",
      "direction": "input",
      "type": "wire",
      "width": "1"
    },
    {
      "name": "c",
      "direction": "output",
      "type": "wire",
      "width": "1"
    }
  ]
}
EOF

cat >"${TOP}".decoupled.json.gold <<EOF
{
  "name": "ifelse",
  "decoupled io": [
  ]
}
EOF

cat > "${TOP}".pcad.json.gold <<EOF
{
  "top": "ifelse",
  "modules": [
    {
      "name": "ifelse",
      "ports": [
        {
          "name": "clk",
          "width": "1",
          "depth": "1",
          "direction": "input"
        },
        {
          "name": "a",
          "width": "1",
          "depth": "1",
          "direction": "input"
        },
        {
          "name": "b",
          "width": "1",
          "depth": "1",
          "direction": "input"
        },
        {
          "name": "c",
          "width": "1",
          "depth": "1",
          "direction": "output"
        }
      ],
      "wires": [
      ],
      "statements": [
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
              "type": "if",
              "cond": {
                "type": "wire",
                "wire": {
                  "name": "a",
                  "width": "1",
                  "depth": "1",
                  "direction": "input"
                }
              },
              "on true": [
                {
                  "type": "assign",
                  "dest": {
                    "type": "wire",
                    "wire": {
                      "name": "c",
                      "width": "1",
                      "depth": "1",
                      "direction": "output"
                    }
                  },
                  "source": {
                    "type": "wire",
                    "wire": {
                      "name": "b",
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
                      "name": "c",
                      "width": "1",
                      "depth": "1",
                      "direction": "output"
                    }
                  },
                  "source": {
                    "type": "wire",
                    "wire": {
                      "name": "b",
                      "width": "1",
                      "depth": "1",
                      "direction": "input"
                    }
                  }
                }
              ],
              "on false": [
                {
                  "type": "assign",
                  "dest": {
                    "type": "wire",
                    "wire": {
                      "name": "c",
                      "width": "1",
                      "depth": "1",
                      "direction": "output"
                    }
                  },
                  "source": {
                    "type": "unop",
                    "statement": {
                      "type": "wire",
                      "wire": {
                        "name": "b",
                        "width": "1",
                        "depth": "1",
                        "direction": "input"
                      }
                    }
                  }
                }
              ]
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
