#include "_tempdir.bash"

cat >"$INPUT" <<"EOF"
[
  {
    "type": "sram",
    "name": "tag_array_ext",
    "depth": 64,
    "width": 80,
    "ports": [
      {
        "clock port name": "RW0_clk",
        "mask granularity": 20,
        "output port name": "RW0_rdata",
        "input port name": "RW0_wdata",
        "address port name": "RW0_addr",
        "mask port name": "RW0_wmask",
        "chip enable port name": "RW0_en",
        "write enable port name": "RW0_wmode"
      }
    ]
  },
  {
    "type": "sram",
    "name": "T_1090_ext",
    "depth": 512,
    "width": 64,
    "ports": [
      {
        "clock port name": "RW0_clk",
        "output port name": "RW0_rdata",
        "input port name": "RW0_wdata",
        "address port name": "RW0_addr",
        "chip enable port name": "RW0_en",
        "write enable port name": "RW0_wmode"
      }
    ]
  },
  {
    "type": "sram",
    "name": "T_406_ext",
    "depth": 512,
    "width": 64,
    "ports": [
      {
        "clock port name": "RW0_clk",
        "mask granularity": 8,
        "output port name": "RW0_rdata",
        "input port name": "RW0_wdata",
        "address port name": "RW0_addr",
        "mask port name": "RW0_wmask",
        "chip enable port name": "RW0_en",
        "write enable port name": "RW0_wmode"
      }
    ]
  },
  {
    "type": "sram",
    "name": "T_2172_ext",
    "depth": 64,
    "width": 88,
    "ports": [
      {
        "clock port name": "W0_clk",
        "mask granularity": 22,
        "input port name": "W0_data",
        "address port name": "W0_addr",
        "chip enable port name": "W0_en",
        "mask port name": "W0_mask"
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

LIBS+=("-l" "saed32.json")
cat >"saed32.json" <<"EOF"
[
  {
    "type": "sram",
    "name": "SRAM1RW1024x8",
    "width": 8,
    "depth": 1024,
    "ports": [
      {
        "address port name": "A",
        "address port polarity": "active high",
        "clock port name": "CE",
        "clock port polarity": "positive edge",
        "write enable port name": "WEB",
        "write enable port polarity": "active low",
        "read enable port name": "OEB",
        "read enable port polarity": "active low",
        "chip enable port name": "CEB",
        "chip enable port polarity": "active low",
        "output port name": "O",
        "output port polarity": "active high",
        "input port name": "I",
        "input port polarity": "active high"
      }
    ]
  },
  {
    "type": "sram",
    "name": "SRAM1RW512x32",
    "width": 32,
    "depth": 512,
    "ports": [
      {
        "address port name": "A",
        "address port polarity": "active high",
        "clock port name": "CE",
        "clock port polarity": "positive edge",
        "write enable port name": "WEB",
        "write enable port polarity": "active low",
        "read enable port name": "OEB",
        "read enable port polarity": "active low",
        "chip enable port name": "CEB",
        "chip enable port polarity": "active low",
        "output port name": "O",
        "output port polarity": "active high",
        "input port name": "I",
        "input port polarity": "active high"
      }
    ]
  },
  {
    "type": "sram",
    "name": "SRAM1RW64x128",
    "width": 128,
    "depth": 64,
    "ports": [
      {
        "address port name": "A",
        "address port polarity": "active high",
        "clock port name": "CE",
        "clock port polarity": "positive edge",
        "write enable port name": "WEB",
        "write enable port polarity": "active low",
        "read enable port name": "OEB",
        "read enable port polarity": "active low",
        "chip enable port name": "CEB",
        "chip enable port polarity": "active low",
        "output port name": "O",
        "output port polarity": "active high",
        "input port name": "I",
        "input port polarity": "active high"
      }
    ]
  },
  {
    "type": "sram",
    "name": "SRAM1RW64x32",
    "width": 32,
    "depth": 64,
    "ports": [
      {
        "address port name": "A",
        "address port polarity": "active high",
        "clock port name": "CE",
        "clock port polarity": "positive edge",
        "write enable port name": "WEB",
        "write enable port polarity": "active low",
        "read enable port name": "OEB",
        "read enable port polarity": "active low",
        "chip enable port name": "CEB",
        "chip enable port polarity": "active low",
        "output port name": "O",
        "output port polarity": "active high",
        "input port name": "I",
        "input port polarity": "active high"
      }
    ]
  },
  {
    "type": "sram",
    "name": "SRAM1RW64x8",
    "width": 8,
    "depth": 64,
    "ports": [
      {
        "address port name": "A",
        "address port polarity": "active high",
        "clock port name": "CE",
        "clock port polarity": "positive edge",
        "write enable port name": "WEB",
        "write enable port polarity": "active low",
        "read enable port name": "OEB",
        "read enable port polarity": "active low",
        "chip enable port name": "CEB",
        "chip enable port polarity": "active low",
        "output port name": "O",
        "output port polarity": "active high",
        "input port name": "I",
        "input port polarity": "active high"
      }
    ]
  },
  {
    "type": "sram",
    "name": "SRAM1RW512x8",
    "width": 8,
    "depth": 512,
    "ports": [
      {
        "address port name": "A",
        "address port polarity": "active high",
        "clock port name": "CE",
        "clock port polarity": "positive edge",
        "write enable port name": "WEB",
        "write enable port polarity": "active low",
        "read enable port name": "OEB",
        "read enable port polarity": "active low",
        "chip enable port name": "CEB",
        "chip enable port polarity": "active low",
        "output port name": "O",
        "output port polarity": "active high",
        "input port name": "I",
        "input port polarity": "active high"
      }
    ]
  },
  {
    "type": "sram",
    "name": "SRAM2RW64x32",
    "width": 32,
    "depth": 64,
    "ports": [
      {
        "address port name": "A1",
        "address port polarity": "active high",
        "clock port name": "CE1",
        "clock port polarity": "positive edge",
        "write enable port name": "WEB1",
        "write enable port polarity": "active low",
        "read enable port name": "OEB1",
        "read enable port polarity": "active low",
        "chip enable port name": "CEB1",
        "chip enable port polarity": "active low",
        "output port name": "O1",
        "output port polarity": "active high",
        "input port name": "I1",
        "input port polarity": "active high"
      },
      {
        "address port name": "A2",
        "address port polarity": "active high",
        "clock port name": "CE2",
        "clock port polarity": "positive edge",
        "write enable port name": "WEB2",
        "write enable port polarity": "active low",
        "read enable port name": "OEB2",
        "read enable port polarity": "active low",
        "chip enable port name": "CEB2",
        "chip enable port polarity": "active low",
        "output port name": "O2",
        "output port polarity": "active high",
        "input port name": "I2",
        "input port polarity": "active high"
      }
    ]
  }
]
EOF

#include "_harness.bash"

if test -f "${OUTPUT}.v"
then
    cat "${OUTPUT}.v" | grep SRAM
fi
