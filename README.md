# **Лабораторные работы №5-7**
## Козырев Пётр

## Цель работы
Целью является приобретение практических навыков в:
 - Управлении серверами сообщений (№5)
 - Применение отложенных вычислений (№6)
 - Интеграция программных систем друг с другом (№7)

## Задание
Реализовать распределенную систему по асинхронной обработке запросов. В данной
распределенной системе должно существовать 2 вида узлов: «управляющий» и
«вычислительный». Необходимо объединить данные узлы в соответствии с той топологией,
которая определена вариантом. Связь между узлами необходимо осуществить при помощи
технологии очередей сообщений. Также в данной системе необходимо предусмотреть проверку
доступности узлов в соответствии с вариантом. При убийстве («kill -9») любого вычислительного
узла система должна пытаться максимально сохранять свою работоспособность, а именно все
дочерние узлы убитого узла могут стать недоступными, но родительские узлы должны сохранить
свою работоспособность.
Управляющий узел отвечает за ввод команд от пользователя и отправку этих команд на
вычислительные узлы.

## Топология
Все вычислительные узлы хранятся в бинарном дереве поиска. [parent] — является
необязательным параметром

## Тип команд для вычислительных узлов
Локальный таймер.
Формат команды сохранения значения: exec id subcommand
subcommand – одна из трех команд: start, stop, time.
start – запустить таймер
stop – остановить таймер
time – показать время локального таймера в миллисекундах

## Тип проверки доступности узлов
Команда проверки 1
Формат команды: pingall
Вывод всех недоступных узлов вывести разделенные через точку запятую.
Пример:
> pingall
Ok: -1 // Все узлы доступны
> pingall
Ok: 7;10;15 // узлы 7, 10, 15 — недоступны