#include "_tempdir.bash"

TOP=hello_world

cat >"${TOP}".v <<EOF
//-----------------------------------------------------
// This is my first Verilog Program
// Design Name : hello_world
// File Name : hello_world.v
// Function : This program will print 'hello world'
// Coder    : Deepak
//-----------------------------------------------------
module hello_world ;

initial begin
  $display ("Hello World by Deepak");
  #10 $finish;
end

endmodule // End of Module hello_world
EOF

cat >"${TOP}.io.json.gold" <<EOF
{
  "name": "hello_world",
  "ports": [
  ]
}
EOF

cat >"${TOP}.decoupled.json.gold" <<EOF
{
  "name": "hello_world",
  "decoupled io": [
  ]
}
EOF

#include "_harness.bash"
