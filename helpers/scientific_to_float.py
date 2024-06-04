import readline
readline.set_auto_history(True)

quit = False
while not quit:
    try:
        exp_norm = int(input('Normalised exponent: '))
        mantissa = int(input('Mantissa (with the implicit 1 if there): '), 2)

        fixed = mantissa >> 23
        decimal = mantissa & ~(0x1ff << 23)

        print(f'Fixed: {fixed}, {hex(fixed)}, {bin(fixed)}')
        print(f'Decimal: {decimal}, {hex(decimal)}, {bin(decimal)}')

        number = fixed
        for i in range(22, -1, -1):
            bit = (decimal >> i) & 0x1
            if bit:
                place = 1 / 2**(23 - i)
                number += place

        number *= 2 ** exp_norm

        print(number, '\n')

    except EOFError:
        quit = True

    except Exception as e:
        print('Exception caught:', e)
