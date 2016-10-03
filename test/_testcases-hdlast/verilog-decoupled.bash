#include "_tempdir.bash"

cat >"${TOP}.v" <<"EOF"
module Top(
  input io_mem_axi_0_aw_ready,
  output io_mem_axi_0_aw_valid,
  output [31:0] io_mem_axi_0_aw_bits_addr
);

  assign io_mem_axi_0_aw_valid = 0;

endmodule
EOF

cat >"${TOP}.io.json.gold" <<EOF
{
  "name": "Top",
  "ports": [
    {
      "name": "io_mem_axi_0_aw_ready",
      "direction": "input",
      "type": "wire",
      "width": "1"
    },
    {
      "name": "io_mem_axi_0_aw_valid",
      "direction": "output",
      "type": "wire",
      "width": "1"
    },
    {
      "name": "io_mem_axi_0_aw_bits_addr",
      "direction": "output",
      "type": "wire",
      "width": "32"
    }
  ]
}
EOF

cat >"${TOP}.decoupled.json.gold" <<EOF
{
  "name": "Top",
  "decoupled io": [
    {
      "base": "io_mem_axi_0_aw",
      "direction": "output",
      "bits": [
        {
          "name": "io_mem_axi_0_aw_bits_addr",
          "width": "32"
        }
      ]
    }
  ]
}
EOF

#include "_harness.bash"
