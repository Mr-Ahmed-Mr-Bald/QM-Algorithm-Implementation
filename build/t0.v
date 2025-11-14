module minimized_module (
    input x0,
    output f
);

    // Internal wires
    wire x0_n;
    wire product0;

    // NOT gates for inverted inputs
    not(x0_n, x0);

    // AND gates for product terms
    assign product0 = x0_n;

    // OR gate for sum of products
    assign f = product0;

endmodule
