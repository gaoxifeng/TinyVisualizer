from sympy import *
from sympy.printing.c import C99CodePrinter
from sympy.printing.precedence import precedence
default_printer=C99CodePrinter().doprint

a0,a1=symbols('a[0] a[1]')
b0,b1=symbols('b[0] b[1]')
c0,c1=symbols('c[0] c[1]')
H=Matrix([[symbols('H[0][0]'),symbols('H[0][1]')],
          [symbols('H[1][0]'),symbols('H[1][1]')]])
e=Matrix([[b0-a0,c0-a0],
          [b1-a1,c1-a1]])
vars=[a0,a1,b0,b1,c0,c1]

#energy
inve=e.inv()*e.det()
print(inve)
energy=(inve.transpose()*H*inve).trace()
gradient=[diff(energy,i) for i in vars]
hessian=[[diff(g,i) for g in gradient] for i in vars]
exprs=[energy]+gradient
for r in hessian:
    for c in r:
        exprs.append(c)

#simplify
vars=cse(exprs)
tmps=vars[0]
exprs=vars[1]
for t in tmps:
    print("T %s=%s;"%(default_printer(t[0]),default_printer(t[1])))

offG=0
print("T E=%s;"%default_printer(exprs[offG]))
offG+=1

#gradient
off=0
print("if(grad) {")
for g in gradient:
    print("  (*grad)[%d]=%s;"%(off,default_printer(exprs[offG])))
    offG+=1
    off+=1
print("}")
    
#hessian
offr=0
print("if(hess) {")
for r in hessian:
    offc=0
    for c in r:
        print("  (*hess)(%d,%d)=%s;"%(offr,offc,default_printer(exprs[offG])))
        offG+=1
        offc+=1
    offr+=1
print("}")
print("return E;")