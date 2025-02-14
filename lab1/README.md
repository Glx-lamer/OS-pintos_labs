>  **Информация к вашему варианту задания:**  
> Индивидуальные задания: 3,5  
> Способ хранения списка "спящих" процессов: массив 

Функции в файле "timer.h":

> - void **timer_init** (void);
> - void **timer_calibrate** (void);
> - int64_t **timer_ticks** (void);
> - int64_t **timer_elapsed** (int64_t);
> - void **timer_sleep** (int64_t ticks);
> - void **timer_msleep** (int64_t milliseconds);
> - void **timer_usleep** (int64_t microseconds);
> - void **timer_nsleep** (int64_t nanoseconds);
> - void **timer_mdelay** (int64_t milliseconds);
> - void **timer_udelay** (int64_t microseconds);
> - void **timer_ndelay** (int64_t nanoseconds);
> - void **timer_print_stats** (void);

Так же функции, используемые внутри функций файла "timer.h":

> - static void **timer_interrupt** (struct intr_frame *args UNUSED)
> - static bool **too_many_loops** (unsigned loops)
> - static void NO_INLINE **busy_wait** (int64_t loops)
> - static void **real_time_sleep** (int64_t num, int32_t denom)
> - static void **real_time_delay** (int64_t num, int32_t denom)

Описание функций файла "timer.h", взятые из файла "timer.c":

|  N  |                   | Аргументы функции                                                                      | Описание функции в "timer.c"                                                                                                                               |
| :-: | :---------------- | :------------------------------------------------------------------------------------- | :--------------------------------------------------------------------------------------------------------------------------------------------------------- |
|  1  | timer_init        | -                                                                                      | Настраивает таймер на прерывание TIMER_FREQ раз в секунду<br>и регистрирует соответствующее прерывание.                                                    |
|  2  | timer_calibrate   | -                                                                                      | Калибрует loops_per_tick, используемый для реализации кратковременных задержек.                                                                            |
|  3  | timer_ticks       | -                                                                                      | Возвращает количество тиков таймера с момента загрузки операционной системы.                                                                               |
|  4  | timer_elapsed     | int64_t then - момент отсчета, значением, однажды возвращенный функцией timer_ticks(). | Возвращает количество тактов таймера, прошедших с момента "then".                                                                                          |
|  5  | timer_sleep       | int64_t ticks - некоторое количество тиков.                                            | Переключает процесс в "спящее" состояние примерно на "ticks" тиков. Должны быть включены прерывания.                                                       |
|  6  | timer_msleep      | int64_t ms - некоторое количество миллисекунд.                                         | Переключает процесс в "спящее" состояние примерно на "ms" миллисекунд. Должны быть включены прерывания.                                                    |
|  7  | timer_usleep      | int64_t us - некоторое количество микросекунд.                                         | Переключает процесс в "спящее" состояние примерно на "us" микросекунд. Должны быть включены прерывания.                                                    |
|  8  | timer_nsleep      | int64_t ns - некоторое количество наносекунд.                                          | Переключает процесс в "спящее" состояние примерно на "ns" наносекунд. Должны быть включены прерывания.                                                     |
|  9  | timer_mdelay      | int64_t ms - некоторое количество миллисекунд.                                         | Переключает процесс в состояние "активного ожидания" примерно на "ms" миллисекунд. Нет необходимости включать прерывания, но расходуются циклы процессора. |
| 10  | timer_udelay      | int64_t us - некоторое количество микросекунд.                                         | Переключает процесс в состояние "активного ожидания" примерно на "us" микросекунд. Нет необходимости включать прерывания, но расходуются циклы процессора. |
| 11  | timer_ndelay      | int64_t ns - некоторое количество наносекунд.                                          | Переключает процесс в состояние "активного ожидания" примерно на "ns" наносекунд. Нет необходимости включать прерывания, но расходуются циклы процессора.  |
| 12  | timer_print_stats | -                                                                                      | Выводит статистику таймера.                                                                                                                                |
| 13  | timer_interrupt   | _struct intr_frame* args UNUSED_ – структура стека прерываний.                         | Обрабатывает прерывания таймера.                                                                                                                           |
| 14  | too_many_loops    | unsigned loops - некоторое количество итераций.                                        | Возвращает true, если "loops" итераций ожидают более одного такта таймера, в противном случае - false.                                                     |
| 15  | busy_wait         | int64_t loops - некоторое количество итераций (счетчик).                               | Реализация "коротких" задержек.                                                                                                                            |
| 16  | real_time_sleep   | int64_t num, int32_t denom.                                                            | Переключает в "спящее" состояние примерно на "num"/"denom" секунд.                                                                                         |
| 17  | real_time_delay   | int64_t num, int32_t denom.                                                            | Переключает в состояние "активного ожидания" примерно на "num"/"denom" секунд.                                                                             |

Исходный код функции "timer_sleep()":

```c title=timer_sleep()
void
timer_sleep (int64_t ticks)
{
  int64_t start = timer_ticks ();
  
  ASSERT (intr_get_level () == INTR_ON);
  while (timer_elapsed (start) < ticks)
    thread_yield ();
}
```

Что происходит:

1. Принимается "ticks"
2. Устанавливается время начала "start"  с помощью "timer_ticks()"
3. Проверяется, что прерывания включены
4. Пока время выполнения функции "timer_sleep()" меньше запланированных тиков
	Загружается процессор, а процесс передается в управление планировщика
5. Конец

![[diagram 1.png]]

Что нужно сделать:

1. Добавить структуру  "спящего" потока
2. Добавить динамически расширяющийся массив "спящих" потоков

Принцип работы:
1. Поток принимается
2. Вычисляется время пробуждения
3. Поток усыпляется
4. Проверяется время первого потока массива
5. Если наступило время пробуждения, поток активируется

Был разработан код модифицированного таймера: таймер срабатывает при положительных тиках, проверяются прерывания, после чего поток заносится в массив при помощи add_thread() и "усыпляется" функцией thread_block(), далее снова включаются прерывания.

```c title=timer_sleep
void
timer_sleep (int64_t ticks)
{
  if (ticks > 0)
    {
      ASSERT(intr_get_level() == INTR_ON);
      intr_disable();
      add_thread(thread_current(), timer_ticks() + ticks);
      thread_block();
      ASSERT(intr_get_level() == INTR_OFF);
      intr_enable();
    }
}
```

*Cостояния прерываний*

```c title=intr_level
/* Interrupts on or off? */
enum intr_level
  {
    INTR_OFF,             /* Interrupts disabled. */
    INTR_ON               /* Interrupts enabled. */
  };
```

Работа модифицированного таймера основывается на массиве "усыпленных" потоков. Потоки хранятся в структуре sleeped_thread, содержащей указатель на поток и время его пробуждения

```c title=sleeped_thread struct
struct sleeped_thread {
  struct thread *t;
  int64_t tick_unsleep;
};
```

Массив динамически расширяется и "усыпленный" поток вставляется, тем самым образуя отсортированный массив, в функции add_thread

```c title=add_thread
void
add_thread (struct thread *t, int64_t ticks)
{
  struct sleeped_thread* _sleeped_array = (struct sleeped_thread *) realloc(sleeped_array, (array_size + 1) * sizeof(struct sleeped_thread));
  sleeped_array = _sleeped_array;
  int cur = array_size;
  for(; cur > 0 && sleeped_array[cur - 1].tick_unsleep > ticks; cur--)
    {
      sleeped_array[cur] = sleeped_array[cur - 1];
    }
  sleeped_array[cur].t = t;
  sleeped_array[cur].tick_unsleep = ticks;
  array_size++;
}
```

В функцию timer_interrupt() добавили функцию для разблокировки процесса unsleep_thread, реализующую процесс активного ожидания для ближайшего по времени потока, который после удаляется из массива ожидающих поток функцией pop_thread

```c title=unsleep_thread
void
unsleep_thread ()
{
  while (array_size > 0 && sleeped_array[0].tick_unsleep <= ticks)
    {
      thread_unblock(sleeped_array[0].t);
      pop_thread();
    }
}
```

```c title=pop_thread
void
pop_thread ()
{
  for (int i = 0; i < array_size; i++)
    {
      sleeped_array[i].t = sleeped_array[i + 1].t;
      sleeped_array[i].tick_unsleep = sleeped_array[i + 1].tick_unsleep;
    }
  array_size--;
}
```

Так, работу модифицированного таймера можно отобразить на диаграмме:

![[diagram2.png]]

Результаты тестов:
![[{154D209D-2C8E-4E52-B9D1-C85F1B812BEA}.png]]

Проверка локальных тестов:

```bash title="Повторная сборка"
cd / && cd pintos/src && make clean && cd ../../ && cd pintos/src/threads && make && cd build
```

```bash title="Запуск тестов"
(make tests/threads/alarm-zero.result && make tests/threads/alarm-negative.result && make tests/threads/alarm-single.result && make tests/threads/alarm-simultaneous.result && make tests/threads/alarm-multiple.result) | grep -E "pass|FAIL"
```

Подготовка к выгрузке на ibks:

```bash title="Сборка pintos для сдачи"
cd / && cd pintos/src && make clean && cd ../../ && sudo tar -zcf pintos_lab<LABnum>.tar.gz pintos
```

Выгрузка с Docker'а на пк:

```bash title="Перенос с Docker-conteiner'а на PC"
docker cp <CONTAINERname>:pintos_lab<LABnum>.tar.gz C:\
```

Индивидуальные задания:

> Реализуйте тесты "max-mem-malloc", "max-mem-calloc", "max-mem-palloc" в файле tests/threads/max-mem.c, измеряющий количество памяти, которая доступна в куче ядра. 
> Тесты должны подсчитывать максимально возможное количество вызовов подряд, без освобождения памяти: 
> - функции malloc, за каждый вызов выделяется 256 байт (тест "max-mem-malloc"); 
> - функции calloc, за каждый вызов выделяется память для хранения массива int из 128 элементов (тест "max-mem-calloc"); 
> - функции palloc, за каждый вызов запрашивается 1 страница ("max-mem-palloc"). 
> Объясните полученные результаты в отчете.

В цикле выделяется память, после чего проверяется, была ли она выделена и счетчик проведенных выделений памяти увеличивается на 1.
В конце теста с помощью msg выводится сообщение с показателем счетчика.

В pintos выделяемая память представляет из себя страницы размером 4 КБ (одна из которых и выделяется при использовании функции palloc_get_page), которая при необходимости делится на блоки размером (некоторой степени 2) байт, которые и выделяются при использовании функций malloc и calloc

Так, были получены результаты тестов:

| Тест           | Количество выделенных блоков | Размер блока | Размер выделенной памяти |
| -------------- | ---------------------------- | ------------ | ------------------------ |
| max-mem-malloc | 5 700                        | 256          | 1 459 200                |
| max-mem-calloc | 2 660                        | 128 * 4      | 1 361 920                |
| max-mem-palloc | 380                          | 4 096        | 1 556 480                |

Можно заметить, что все 3 значения колеблются около 1.5 МБ. Так как в pintos выделено 4 МБ памяти, из которых между пулом ядра и пулом пользователя (из которого и выделяется память) по умолчанию поровну делятся 3 МБ. Таким образом, максимальная доступная память составляет 1.5 МБ, которые и выделятся в полной мере функцией palloc. Остальные функции, как мы видим, задействуют не всю доступную память. Это происходит из-за разбиения страниц на блоки, информация о котором (метаданные) и хранятся, как правило, в первом блоке, по этой причине один блок из страницы не может быть выделен для хранения данных. Так, получаем, что в случае теста max-mem-malloc страница делится на 4096/256 = 16 блоков, из которых под данные выделяется только 15 в каждой из 5700/15 = 380 страницах, что и соответствует полученному значению max-mem-palloc. Аналогично, для max-mem-calloc выделяется 4096/512 = 8 блоков, из которых используется 7 в 2660/7 = 380 страницах, что так же совпадает со значением, полученным в тесте.


```c title=max-mem-malloc
void
test_max_mem_malloc ()
{
    int64_t iteracts = 0;
    char *used_mem = (char *) malloc(256 * sizeof(char));
    for(; used_mem; iteracts++)
    {
        used_mem = (char *) malloc(256 * sizeof(char));
    }
    msg("\n>>> %d times mem can be allocated with malloc()", iteracts);
}
```

> Реализуйте систему аудита запуска и завершения потоков. Система аудита должна добавлять в журнал запись о запущенном потоке в момент его создания и о завершившемся потоке в момент его завершения. В записи хранятся: время создания/завершения потока (измеряется в тиках таймера от старта ОС), его имя и идентификатор tid_t. Журнал событий должен храниться в памяти в виде динамического списка. Добавление записей в список может быть реализовано в src/threads.c 
> Реализуйте тест threads-audit в файле tests/threads/threads-audit.c. 
> Тест создает 10 потоков. Каждый из потоков выполняет какую-либо вычислительную задачу или выполняет активное ожидание в течение небольшого интервала времени (1-3 сек). Разные потоки должны работать разное по продолжительности время. 
> Главная функция теста создает эти потоки, ожидает 5 секунд, далее — распечатывает все зарегистрированные подсистемой аудита события. Укажите полученные результаты в отчете и объясните их.

Согласно заданию, нам нужно реализовать динамический список структур, содержащих время действия с потоком, имя потока, идентификатор потока, а так же тип действия с потоком - завершение или запуск.

Была реализована структура audit_elem:

```c title=audit_elem
struct audit_elem
   {
      tid_t tid;
      char name[16];
      int64_t ticks;
      enum thread_act act;
      struct list_elem elem;
   };
```

Содержащая перечисление thread_act, принимающая значения THREAD_STARTED и THREAD_ENDED.

```c titcle=thread_act
enum thread_act
  {
   THREAD_STARTED,
   THREAD_ENDED
  };
```

Были добавлены функции:
- audit_list_init, инициализирующая встроенную в pintos структуру списка
- add_audit_list, добавляющая новый элемент в конец списка
- out_audit_list, распечатывающая список 

```c title=audit_list_init
void
audit_list_init ()
{
  list_init(&audit_list);
}
```

```c title=add_audit_list
void
add_audit_list (struct thread *t, enum thread_act act)
{
  struct audit_elem *elem = (struct audit_elem *) malloc(sizeof(struct audit_elem));
  elem->tid = t->tid;
  strlcpy(elem->name, t->name, sizeof(elem->name));
  elem->ticks = timer_ticks();
  elem->act = act;
  list_push_back(&audit_list, &elem->elem);
}
```

```c title=out_audit_list
void
out_audit_list ()
{
  int action = 0;
  while(!list_empty(&audit_list))
    {
      action++;
      struct audit_elem *elem = list_entry(list_pop_front(&audit_list), struct audit_elem, elem);
      msg("Action %d:", action);
      msg("Thread \"%s\" #%d", elem->name, elem->tid);
      msg("Action time: %d", elem->ticks);
      msg("%s\n", elem->act == THREAD_STARTED ? "Started" : "Ended");
    }
}
```

Функция add_audit_list с записями действия: THREAD_STARTED и THREAD_ENDED была добавлена в функции thread_start и thread_exit соответственно.

Функция audit_init_list была добавлена в файл init.c

Так же был написан тест threads-audit, создающий 5 потоков, вычисляющих значений чисел Фибоначчи для 17, 22, 27, 32 и 37, и 5 потоков с активным ожиданием в 1, 1.5, 2, 2.5, 3 секунд. После чего проходит ожидание 5 секунд и распечатывает список аудита с помощью функции out_audit_list

```c title=threads-audit
#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/malloc.h"
#include "threads/thread.h"
#include "devices/timer.h"
  
long int fib (int n) {
    if (n <= 1) return n;
    return fib(n - 1) + fib(n - 2);
}
  
void
comp_thread (void *aux)
{
    int n = (int) aux;
    fib(n);
}
  
void
busy_wait (void *aux)
{
    int n = (int) aux;
    timer_mdelay(n);
}

void
test_threads_audit (void)
{
  for(int i = 0; i < 5; i++)
    {
        char name[16];
        snprintf(name, sizeof(name), "Thread %d comps", i);
        thread_create(name, PRI_DEFAULT, comp_thread, 17 + i);
    }
  
  for(int i = 0; i < 5; i++)
    {
        char name[16];
        snprintf(name, sizeof(name), "Thread %d waits", 5 + i);
        thread_create(name, PRI_DEFAULT, busy_wait, 3000 - i * 500);
    }

  timer_msleep(5000);

  out_audit_list();
}
```


Данные полученные из аудита потоков:

|   Имя потока   | Время начала | Время окончания |      Время исполнения      |
| :------------: | :----------: | :-------------: | :------------------------: |
| Thread 0 comps |      55      |       79        |   (79 - 55) / 100 = 0.24   |
| Thread 1 comps |      64      |       117       |  (117 - 64) / 100 = 0.53   |
| Thread 2 comps |      83      |       226       |  (226 - 83) / 100 = 1.43   |
| Thread 3 comps |      96      |       390       |  (390 - 96) / 100 = 2.94   |
| Thread 4 comps |     113      |       561       |  (561 - 113) / 100 = 4.48  |
| Thread 5 waits |     132      |      2149       | (2149 - 132) / 100 = 20.17 |
| Thread 6 waits |     169      |      1998       | (1998 - 169) / 100 = 18.29 |
| Thread 7 waits |     194      |      1785       | (1785 - 194) / 100 = 15.91 |
| Thread 8 waits |     223      |      1500       | (1500 - 223) / 100 = 12.77 |
| Thread 9 waits |     278      |      1186       | (1186 - 278) / 100 = 9.08  |

Можем заметить, что время между созданием потока и его завершением отличается от предположительного времени исполнения потока, что особенно заметно в потоках 5 - 9, для которых выполнялось активное ожидание. Это еще раз доказывает, что потоки в pintos выполняются поочередно, при чем выполняться может лишь один поток, пока остальные приостановлены.

Полный распечатанный аудит потоков:
```bash title=outed_audit
(threads-audit) Action 1:
(threads-audit) Thread "idle" #2
(threads-audit) Action time: 0
(threads-audit) Started

(threads-audit) Action 2:
(threads-audit) Thread "Thread 0 comps" #3
(threads-audit) Action time: 55
(threads-audit) Started

(threads-audit) Action 3:
(threads-audit) Thread "Thread 1 comps" #4
(threads-audit) Action time: 64
(threads-audit) Started

(threads-audit) Action 4:
(threads-audit) Thread "Thread 0 comps" #3
(threads-audit) Action time: 79
(threads-audit) Ended

(threads-audit) Action 5:
(threads-audit) Thread "Thread 2 comps" #5
(threads-audit) Action time: 83
(threads-audit) Started

(threads-audit) Action 6:
(threads-audit) Thread "Thread 3 comps" #6
(threads-audit) Action time: 96
(threads-audit) Started

(threads-audit) Action 7:
(threads-audit) Thread "Thread 4 comps" #7
(threads-audit) Action time: 113
(threads-audit) Started

(threads-audit) Action 8:
(threads-audit) Thread "Thread 1 comps" #4
(threads-audit) Action time: 117
(threads-audit) Ended

(threads-audit) Action 9:
(threads-audit) Thread "Thread 5 waits" #8
(threads-audit) Action time: 132
(threads-audit) Started

(threads-audit) Action 10:
(threads-audit) Thread "Thread 6 waits" #9
(threads-audit) Action time: 169
(threads-audit) Started

(threads-audit) Action 11:
(threads-audit) Thread "Thread 7 waits" #10
(threads-audit) Action time: 194
(threads-audit) Started

(threads-audit) Action 12:
(threads-audit) Thread "Thread 8 waits" #11
(threads-audit) Action time: 223
(threads-audit) Started

(threads-audit) Action 13:
(threads-audit) Thread "Thread 2 comps" #5
(threads-audit) Action time: 226
(threads-audit) Ended

(threads-audit) Action 14:
(threads-audit) Thread "Thread 9 waits" #12
(threads-audit) Action time: 278
(threads-audit) Started

(threads-audit) Action 15:
(threads-audit) Thread "Thread 3 comps" #6
(threads-audit) Action time: 390
(threads-audit) Ended

(threads-audit) Action 16:
(threads-audit) Thread "Thread 4 comps" #7
(threads-audit) Action time: 561
(threads-audit) Ended

(threads-audit) Action 17:
(threads-audit) Thread "Thread 9 waits" #12
(threads-audit) Action time: 1186
(threads-audit) Ended

(threads-audit) Action 18:
(threads-audit) Thread "Thread 8 waits" #11
(threads-audit) Action time: 1500
(threads-audit) Ended

(threads-audit) Action 19:
(threads-audit) Thread "Thread 7 waits" #10
(threads-audit) Action time: 1785
(threads-audit) Ended

(threads-audit) Action 20:
(threads-audit) Thread "Thread 6 waits" #9
(threads-audit) Action time: 1998
(threads-audit) Ended

(threads-audit) Action 21:
(threads-audit) Thread "Thread 5 waits" #8
(threads-audit) Action time: 2149
(threads-audit) Ended
```
