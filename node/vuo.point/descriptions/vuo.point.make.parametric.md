Creates points that form a shape defined by parametric math expressions. 

The math expressions can use the variable *u*. This node calculates each expression for evenly-spaced values of *u* ranging from 0 to 1. Each calculation becomes one of the resulting points. 

- `time` — The time at which to calculate the expressions.  (This only affects the output if the `time` variable is used in the expressions.)
- `xExpression`, `yExpression`, `zExpression` — The math expressions for the (x,y,z) position of each point. 
- `subdivisions` — Each of the above expressions is calculated for this many values of *u*. This is the total number of points created. 

The math expressions can use numbers, [operators](http://muparser.beltoforion.de/mup_features.html#idDef3) such as *+* and *-*, [functions](http://muparser.beltoforion.de/mup_features.html#idDef2) such as *sin(u)* and *log(u)*, and the constants *PI*, *DEG2RAD*, and *RAD2DEG*. 

For help understanding the math used by this node, see [Introduction to parametric equations](https://www.khanacademy.org/video/parametric-equations-1). 
