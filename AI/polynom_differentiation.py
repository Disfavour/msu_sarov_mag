import re

def differentiate(polynom):
    elements = re.findall(r'[+-]?\d+(?:x(?:\^\d+)?)?', polynom)
    print(elements)
    result = []
    for element in elements:
        parts = re.fullmatch(r'([+-]?)(\d+)(?:(x)(?:(\^)(\d+))?)?', element).groups()

        if parts[2] is None:
            continue

        if parts[4] is None:
            result.extend(parts[:2])
            continue
        
        parts = list(parts)
        parts[1] = str(int(parts[1]) * int(parts[4]))
        parts[4] = str(int(parts[4]) - 1)
        if parts[4] == '1':
            result.extend(tuple(parts[:3]))
        else:
            result.extend(tuple(parts))
    
    return ''.join(result)
    

if __name__ == '__main__':
    print(differentiate('32x^123-123x^2+2x-6543'))
