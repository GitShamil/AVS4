# AVS4
![image](https://user-images.githubusercontent.com/113286731/206899378-1b17bc2e-7284-46b3-88ab-fc5f421f186f.png)

# 1. Моя модель вычислений
Модель называется производитель - потребитель.
У меня три потока. Первый производит данные для второго. Второй ждет эти данные, а когда появляются начинает их делать. Потом если данных нет второй опять ждет. Третий действуйет аналогично второму. Он ждет данные и при получении что-то делает. 
В целом у такой модели могут быть аналогично больше 3-х потребителей.

# 2. Входные данные
У программы много вариантов входных и выходных данных.
Входные данные.
1) из командной строки просто перечисляем имена обьектов. Например "cat dog me wall" (максимум 10 обьектов)
2) из консоли пишем сначала количество, а затем имена обьектов. Например "2 lion bottle" (максимум 10 обьектов)
3) из файла пишем имя файла, а в файле должны быть записаны имена обьектов на разных строках. (максимум 10 обьектов)
4) рандомом. ничего писать не надро
Выходные данные.
1) в командую строку.
2) в командую строку и в файл одновременно. Для этого нужно будет еще ввести имя файла.

# 3. Сценарий
Сценарий заключается в том, что есть три человека, которые воруют предметы со склада. Иван выносит обьеты Петрову, и ставит рядом с машиной. А Петров грузит их. Третий же человек (Нечепорук) просто стоит и оценивает стоимость обьектов. Они все это делают слабо зависимо, поэтому логично использовать несколько потоков. Ведь иногда один человек может сделать быстро, а другой долго.

# 4. Подробный обобщенный алгоритм используемый, при реализации программы исходного словесного сценария.
Работа каждого человека соответствует функции.
вынос Иваномом объектов из склада - workIvan
погрузка на машину обьектов - workPetrov
оценка обьектов Нечепоруком - workNecheporuk
1. workIvan 
В этой функции происходит переброс обьектов из items_at_store в items_near_car (симуляция того, что Иван вынес обьект)
2. workPetrov
В этой функции происходит переброс обьектов из items_near_car в items_in_car (симуляция того, что Петров погрузио обьект)
3. workNecheporuk
Происходит оценка стоимости объекта

Создаются 3 потока для каждой из этих функций. И запускаются, а в конце все join к основному потока (то есть мы ждем каждую функцию, пока она не отработает до конца)

Во всех этих функциях используются mutex. Это сделано для того, чтобы не возникло проблем с многопоточностью. Например, вывод в файл и консоль заблочены, пока одна из функций это делает. Это нужно для того, чтобы не возникло каши в консоле, когда один вывод залезает на другой. Кроме этого mutex обвешены вокруг вектора, иначе возникают проблемы с вектором, когда один объект может взяться дважды.

