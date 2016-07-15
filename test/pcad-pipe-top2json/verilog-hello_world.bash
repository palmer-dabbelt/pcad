#include "_tempdir.bash"

TOP=hello_world
INFILE=hello_world.v
OUTFILE=hello_world.io.json

cat >$INFILE <<EOF
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

cat >$OUTFILE.gold <<EOF
{
  "name": "hello_world",
  "ports": [
  ]
}
EOF

#include "_harness.bash"
