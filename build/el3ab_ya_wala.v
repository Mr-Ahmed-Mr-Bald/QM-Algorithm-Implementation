module minimized_module (
    input x0, x1, x2, x3, x4,
    output f
);

// Alternative minimized solutions:
// Solution 1: (~x0 & ~x1 & x3 & x4) | (~x0 & x1 & ~x2 & x4) | (~x0 & x2 & ~x3 & x4) | (x0 & x1 & ~x4) | (~x1 & ~x2 & ~x4) | (x1 & x2 & x3 & ~x4)
// Solution 2: (~x0 & ~x1 & x2 & x4) | (~x0 & x1 & ~x3 & x4) | (~x0 & ~x2 & x3 & x4) | (x0 & x1 & ~x4) | (~x1 & ~x2 & ~x4) | (x1 & x2 & x3 & ~x4)

    assign f = (~x0 & ~x1 & x3 & x4) | (~x0 & x1 & ~x2 & x4) | (~x0 & x2 & ~x3 & x4) | (x0 & x1 & ~x4) | (~x1 & ~x2 & ~x4) | (x1 & x2 & x3 & ~x4);

endmodule
