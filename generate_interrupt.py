class CodePrinter:

    def __init__(self):
        self.indentation = 0

    def indent(self):
        self.indentation += 4

    def unindent(self):
        self.indentation -= 4

    def emit(self, line):
        print(' ' * self.indentation + line)

if __name__ == '__main__':
    irqs = [
        0,
        1,
        2,
        3,
        4,
        5,
        6,
        7,
        8,
        9,
        10,
        11,
        12,
        13,
        14,
        15,
        16,
        32,
        33,
        34,
        35,
        36,
        37,
        38,
        39,
        40,
        41,
        42,
        43,
        44,
        45,
        46,
        47
    ]
    p = CodePrinter()
    p.emit('void interrupt(uint32 irq) {')
    p.indent()
    p.emit('switch (irq) {')
    p.indent()
    for irq in irqs:
        p.emit('case ' + str(irq) + ':')
        p.indent()
        p.emit('interrupt_' + str(irq) + '();')
        p.emit('break;')
        p.unindent()
    p.emit('default:')
    p.indent()
    p.emit('interrupt_15();')
    p.emit('break;')
    p.unindent()
    p.unindent()
    p.emit('}')
    p.unindent()
    p.emit('}')

