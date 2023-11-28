import re

text = '''
✗	IKE	✗
✗	GRE	✗
✗	Тунельный режим	✓
✓	ESP	✓
✗	UDP	✗
✗	Транспортный режим	✓
✓	AH	✓
'''

pattern = re.compile(r'^.(\s)(.+)\s(.)$', flags=re.MULTILINE)

matches = re.sub(pattern, r'\3 \2', text)

print(matches)
