import re

text = '''
✗	Использование второго пароля	✗
✓	Метод «рукопожатия»	✓
✓	Запоминаемость паролей	✓
✓	Методы «запрос-ответ»	✓
✓	Ограничение времени (или задержка) и числа попыток ввода пароля.	✓
✓	Ограничение срока жизни пароля	✓
✓	«Длинный пароль»	✓
✓	Методы, использующие одноразовые (динамично изменяющиеся) пароли	✓
✓	Шифрование списков паролей в АС	✓
'''

pattern = re.compile(r'^.(\s)(.+)\s(.)$', flags=re.MULTILINE)

matches = re.sub(pattern, r'\3 \2', text)

print(matches)
