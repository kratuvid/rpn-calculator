import readline
readline.set_auto_history(True)
readline.parse_and_bind('tab: complete')

print('Henceforth, enter numbers in raw format...')

quit = False
while not quit:
    try:
        ns = input('> ').strip().strip("'")
        if ns.lower() in ('quit', 'exit', 'q', 'e'):
            quit = True
            continue

        part = ns.split(';')
        fixed = part[0].strip().split(':')[1].strip().split()
        decimal = part[1].strip().split(':')[1].strip().split()

        neg = fixed[0][0] == '!'
        if neg:
            fixed[0] = fixed[0].lstrip('!')

        for i in range(len(fixed)):
            fixed[i] = int(fixed[i], 16)    
        for i in range(len(decimal)):
            decimal[i] = int(decimal[i], 16)

        fixed_one = 0
        for i in range(len(fixed)):
            fixed_one += fixed[i] << (i * 32)
        decimal_one = 0
        for i in range(len(decimal)):
            j = len(decimal)-1-i
            decimal_one += decimal[i] << (j * 32)

        one = (fixed_one << (len(decimal) * 32)) + decimal_one
        if neg:
            one = ~one + 1

        fixed_one = one >> (len(decimal) * 32)
        mask = 0
        for i in range(len(decimal)):
            mask |= 0xffffffff << (i * 32)
        decimal_one = one & mask

        dec_actual = 0
        dec_bits = len(decimal) * 32
        for j in range(dec_bits-1, -1, -1):
            places = []
            if (decimal_one >> j) & 0x1:
                places.append(dec_bits - j)

            for e in places:
                dec_actual += 1 / 2**e

        if neg and fixed_one < 0:
            fixed_one += 2**(len(fixed) * 32)

        n = fixed_one + dec_actual
        print('-' if neg else '', n, sep='')

    except EOFError:
        quit = True

    except Exception as e:
        print("Exception caught:", e)
