def get_points(x, y, a, b=1):
    pts = (x - y) / a * b
    return round(pts, 2) if pts > 0 else 0


def getxy(a, res):
    ans = []
    for x in range(a + 1):
        y = a - x
        points = get_points(x, y, a)
        print(x, y, points)

        if res == points:
            ans.append((x, y))

    return ans

print(getxy(6, 0.33))
