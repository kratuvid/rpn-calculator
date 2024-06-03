m = int(input('How many: '))
ml = []
for i in range(m):
    n = int(input('Decimal\'s hex: '), 16)
    print(f'Parsing {hex(n)} aka {n}')
    ml.append(n)

s = 0

for i in range(m):
    r = (i+1) * 32
    places = []
    for j in range(31, -1, -1):
        if (ml[i] >> j) & 0x1:
            places.append(r - j)
    print("1's are at", places)

    for e in places:
        s += 1 / 2**e

print(s)
