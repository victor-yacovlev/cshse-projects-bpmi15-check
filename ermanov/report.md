Результат проверки работы
=========================

Студент: Данат Ерманов
Проверил: Виктор Яковлев


1. Заявленная функциональность
------------------------------

 * Работает выдача статических страниц (4 балла)
 * Сервер умеет выполнять сторонние программы и выдавать результат их работы
   (6 баллов)
 * Умеет обрабатывать POST-запросы (+1 балл)


2. Проверка соответствия заявленных пунктов
-------------------------------------------

### Работает выдача статических страних

Соответствует действительности (4 балла).

### Сервер умеет выполнять сторонние программы и выдавать результат их работы

Сервер действительно умеет выполнять сторонние программы, но взаимодействие
реализовано некорректно, поэтому о результате работы можно наблюдать только
по побочным эффектам, выполняемым программой на сервере (можно засчитать
только на половину, т.е. 5 баллов).

Проверка выполнялась с помощью теста:
```
#!/usr/bin/python3

print("Content-Type: text/plain")
print()

import cgi
fields = dict(cgi.FieldStorage())

print(fields)

```

### Умеет обрабатывать POST-запросы

Не соответствует действительности.

Проверка выполнялась с помощью теста:
```
#!/usr/bin/env python3
import os, sys, json
data = sys.stdin.read(int(os.environ.get('HTTP_CONTENT_LENGTH', 0)))
if data:
    print(list(json.loads(data).keys()])
```

И запроса с использованием `cURL`:
```
curl -v -d '{"key1":"value1", "key2":"value2"}' -H "Content-Type: application/json" -X POST http://localhost:8000/post_test.py
```

3. Другие замечания
-------------------

Студент выложил в репозиторий исходные тексты с аттрибутом "выполняемый" и
артефакт сборки. За не умение работать с VCS это все студенты штрафуются на 1
балл, о чем они неоднократно предупреждались.

4. Итог
-------

Оценка за содержание работы: *4 балла*.