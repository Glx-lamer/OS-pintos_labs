> **Информация к вашему варианту задания:**    
> Ваш вариант задания: 19 


##### 1. Диаграммы исполнения процессов: 

> — для исходного алгоритма планирования pintos; 
> — для алгоритма, разработанного в процессе выполнения работы.

Дана таблица с процессами:

![[{E9C3606C-AF5A-4E64-8DF3-E1DB4169AB07}.png]]

В стандартной реализации планировщика в pintos согласно алгоритму First Come First Served выполняется первый пришедший процесс, пока остальные готовы и жду своего времени.

| Proc  |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |  9  | 10  | 11  | 12  | 13  | 14  | 15  | 16  | 17  | 18  | 19  | 20  | 21  | 22  | 23  |
| :---: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: |
| Proc0 |  И  |  И  |  И  |  И  |  И  |  И  |  И  |  И  |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |
| Proc1 |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  И  |  И  |  И  |  И  |  И  |     |     |     |     |     |     |     |     |     |     |
| Proc2 |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  И  |     |     |     |     |     |     |     |     |     |
| Proc3 |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  И  |  И  |     |     |     |     |     |     |     |
| Proc4 |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  И  |  И  |  И  |  И  |  И  |     |     |
| Proc5 |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  И  |  И  |

В случае модифицированного планировщика исполняется процесс с наибольшим приоритетом, пока процессы с меньшим или равным приоритетами ожидают.

| Proc  |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |  9  | 10  | 11  | 12  | 13  | 14  | 15  | 16  | 17  | 18  | 19  | 20  | 21  | 22  | 23  |
| :---: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: |
| Proc0 |  И  |  Г  |  И  |  Г  |  И  |  И  |  И  |  И  |  И  |  И  |     |     |     |     |     |     |     |     |     |     |     |     |     |
| Proc1 |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  И  |  И  |  И  |  И  |  И  |     |     |     |     |     |     |     |     |
| Proc2 |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  И  |     |     |     |     |     |     |     |
| Proc3 |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  И  |  И  |     |     |     |     |     |
| Proc4 |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  Г  |  И  |  И  |  И  |  И  |  И  |
| Proc5 |  Г  |  И  |  Г  |  И  |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |

##### 2. Сравнительный анализ диаграмм

По первой диаграмме мы можем посчитать:
$$t_{ср. ож. 1} = \frac{0+8+13+14+16+21}{6}=12$$
$$t_{ср. вып. 1} = \frac{8+13+14+16+21+23}{6}=15.83$$

По первой диаграмме мы можем посчитать:
$$t_{ср. ож. 2} = \frac{0+10+15+16+18+8}{6}=11.17$$
$$t_{ср. вып. 2} = \frac{8+15+16+18+23+10}{6}=15$$
Так, в случае модифицированного планировщика, учитывающего приоритет процесса, среднее время как выполнения, так и ожидания меньше, чем в случае стандартного  планировщика.

##### 3. Блок-схема нового разработанного алгоритма планирования

![diagram](https://github.com/Glx-lamer/OS-pintos_labs/blob/main/lab2/info/diagram.png?raw=true)

##### 4. Описание функций, которые применяются для организации планирования: их назначение, описание аргументов, возвращаемого значения, связей друг с другом


| Функция                                                                      | Описание функции                                                                                                                                                                               |
| ---------------------------------------------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| void **thread_block** (void)                                                 | Поток блокируется, после чего вызывается планировщик                                                                                                                                           |
| void **thread_unblock** (struct thread *t)                                   | Поток разблокируется, после чего заносится в список потоков, готовых к исполнению                                                                                                              |
| void **thread_yield** (void)                                                 | Поток заносится в список потоков, готовых к исполнению, после чего вызывается планировщик                                                                                                      |
| void **thread_set_priority** (int new_priority)                              | Изменяет приоритет потока на new_priority                                                                                                                                                      |
| int **thread_get_priority** (void)                                           | Возвращает приоритет потока                                                                                                                                                                    |
| static struct thread * **next_thread_to_run** (void)                         | Возвращает указатель на следующий по порядку исполнения поток                                                                                                                                  |
| void **thread_schedule_tail** (struct thread *prev)                          | Помечает текущий поток как выполняющийся и освобождает страницу, соответствующую выполненному потоку, если он завершился                                                                       |
| static void **schedule** (void)                                              | Планировщик, выбирающий новый поток и переключающий выполнение на него                                                                                                                         |
| struct thread ***switch_threads** (struct thread *cur, struct thread *next); | Переключение контекстов                                                                                                                                                                        |
| void **sema_down** (struct semaphore *sema)                                  | Выполняет операцию «уменьшить» для семафора в ожидании, пока его значение станет положительным, а затем уменьшает его на единицу (Усыпляет поток, после чего добавляет его в список ожидания). |
| void **sema_up** (struct semaphore *sema)                                    | Выполняет операцию «вверх» для семафора, увеличивая его значение. Если какие-либо потоки ожидают семафор, пробуждает один из них (Пробуждает поток из списка ожидания).                        |
| bool **lock_held_by_current_thread** (const struct lock *lock)               | Возвращает, является ли текущий поток "обладателем" замка.                                                                                                                                     |
| void **lock_acquire** (struct lock *lock)                                    | Назначает текущий поток "обладателем" замка, в противном случае - блокирует его.                                                                                                               |
| void **lock_release** (struct lock *lock)                                    | Обнуляет "обладателя" замка, после чего пробуждается поток, заблокированный замком.                                                                                                            |
| void **cond_wait** (struct condition *cond, struct lock *lock)               | Поток блокируется в мониторе и становится "обладателем" замка.                                                                                                                                 |
| void **cond_signal** (struct condition *cond, struct lock *lock UNUSED)      | Пробуждает поток в мониторе.                                                                                                                                                                   |

##### 5. Исходные коды системы планирования Pintos с внесенными модификациями и комментариями

Были внесены изменения в файлы "thread.h", "thread.c", "synch.h", "synch.h".

В файле "thread.h" была дополнена структура потока:

```c title:"Updated thread struct"
	int primal_priority;
	struct list donators;
	struct list_elem donator;
	struct lock *lock;
```

И объявлена новая функция sort_func.

В файле "thread.c":

- При создании потока в thread_create он заносится в список готовых к исполнению потоков с помощью thread_yield

- Изменена функция присваивания приоритетов thread_set_priority:

```c title="Updated thread_set_priority"
void
thread_set_priority (int new_priority)
{
  int temp_primal_priority = thread_current()->primal_priority;
  
  if (!list_empty(&thread_current()->donators)) thread_current()->primal_priority = new_priority;
  else thread_current()->priority = thread_current()->primal_priority = new_priority;
  
  if (temp_primal_priority > new_priority)
    {
      intr_disable();
      thread_yield();
      intr_enable();
    }
}
```

Теперь функция учитывает, что поток имеет дополнительные поля, предназначенные для реализации деления приоритетов и что он может иметь "доноров" приоритетов, которые не позволяют вновь изменить его приоритет.
Так же, если приоритет потока отличается от "исходного", он заносится в список готовых к исполнению.

- В функции init_thread додавлено заполение новых полей структуры потока:

```c title="New struct rlrmrnts in thread"
  t->primal_priority = priority;
  list_init(&t->donators);
  t->lock = NULL;
```

- Добавлена функция sort_func сортировки списка, сортирующая его в порядке убывания приоритетов.

```c title="Sorting func"
bool
sort_func (struct list_elem *first, struct list_elem *second, void *aux)
{
  struct thread *first_thread = list_entry(first, struct thread, elem);
  struct thread *second_thread = list_entry(second, struct thread, elem);
  return first_thread->priority > second_thread->priority;
}
```

- В саму функцию планировщика была добавлена функция list_sort, сортирующая список готовых к исполнению потоков согласно функции sort_func:

```c title="Sorting list func"
list_sort(&ready_list, sort_func, NULL);
```

В файле "synch.h" были определены новые функции sem_sort_func, anti_priority_donation, priority_donation.

В файле "synch.c":

- В функцию sema_down  были добавлены функции добавления потока в список ожидания, а также последующая сортировка этого списка в порядке убывания приоритетов. Также в функцию sema_up была добавлена функция list_sort.

```c title="Sorting wait-list"
list_push_back (&sema->waiters, &thread_current()->elem);
list_sort(&sema->waiters, sort_func, NULL);
```

Это позволяет хранить потоки сразу в отсортированном состоянии и распечатывать из списка ожидания семафора сразу поток с наибольшим приоритетом.

- Для структуры списка семафоров для реализации монитора было добавлено поле приоритета.
- Была добавлена функция sem_sort_func, аналогичная функции sort_func, предназначенная для задачи сортирующей функции списка семафоров:

```c title="Sorting func for sems"
bool
sem_sort_func (struct list_elem *first, struct list_elem *second, void *aux)
{
  struct thread *first_sem = list_entry(first, struct semaphore_elem, elem);
  struct thread *second_sem = list_entry(second, struct semaphore_elem, elem);
  return first_sem->priority > second_sem->priority;
}
```

- В функцию cond_wait, аналогично sema_down были добавлены функции, реализующие вставку в список и последующую его сортировку при помощи функции sem_sort_func. Также в функцию cond_signal была добавлена функция list_sort. 

```c title="Sorting semafors list"
waiter.priority = thread_current()->priority;
list_push_back (&cond->waiters, &waiter.elem);
list_sort(&cond->waiters, sem_sort_func, NULL);
```

- Для реализации распределения приоритетов в функция замков были разработаны функции preority-donation и anti-preority-donation для передачи приоритета от потока-"донора" с большим приоритетом потоку, занявшему ресурс и наоборот, восстановления приоритетов потока-"донора" и потока-"акцептора", соответственно:

```c title="Donation func"
void
priority_donation (struct lock *lock, struct thread *donator)
{
  thread_current()->lock = lock; /* Current thread geting lock */
  list_push_back(&lock->holder->donators, &donator->donator); /* Current thread inserting in donators list of locked thread */
  list_sort(&lock->holder->donators, sort_func, NULL); /* Sorting donators list */
  while(lock && lock->holder->priority < donator->priority) /* While locked thread cant be ended because of the low priority */
    {
      lock->holder->priority = donator->priority;
      donator = lock->holder;
      lock = donator->lock;
    }
}
```

```c title="Acception func"
void
anti_priority_donation (struct lock *lock, struct thread *acceptor)
{
  if(!list_empty(&acceptor->donators))
    {
      struct list_elem *begin = list_begin(&acceptor->donators);
      while(begin != list_end(&acceptor->donators))
        {
          struct thread *t = list_entry(begin, struct thread, donator);
          if (t->lock == lock) begin = list_remove(begin);
          else begin = list_next(begin);
        }
    }
  acceptor->priority = acceptor->primal_priority;
  
  if(!list_empty(&acceptor->donators))
    {
      struct thread *t = list_entry(list_max(&acceptor->donators, sort_func, NULL), struct thread, donator);
      if (t->priority > acceptor->priority) acceptor->priority = t->priority;
    }
}
```

Эти функции позволяют, благодаря отсортированному списку потоков-"доноров", т.е. потоков, зависящих от потока-"акцептора", занимающего ресурс, достигнуть быстрой разблокировки потоков.

##### 6. Описание тестовых задач: имя теста и описание действий, которые в нем совершаются; полученные при запуске результаты, анализ полученных результатов

**alarm-priority** - тест на порядок разблокировки ("пробуждения") потоков.
**priority-change** - тест на изменение приоритета потока.
**priority-fifo** - тест на выполнение алгоритма Round-Robin (FCFS).
**priority-preempt** - тест на порядок исполнения потока с высоким приоритетом.
**priority-sema** - тест на порядок "пробуждения" из списка ожидания семафора.
**priority-condvar** - тест на порядок "пробуждения" потоков в мониторе.
**priority-donate-one** - тест на порядок блокировки потоков замком.
**priority-donate-lower** - тест на корректность порядка действий потоков при передаче приоритета.
**priority-donate-multiple** - тест на корректность порядка передачи приоритетов при блокировке на замках.
**priority-donate-multiple2** - аналогично priority-donate-multiple
**priority-donate-sema** - тест на корректность порядка передачи приоритетов в случае семафоров.
**priority-donate-nest** - тест на порядок передачи приоритетов при множественной блокировке на замках.
**priority-donate-chain** - тест на порядок передачи приоритетов при блокировке "по цепи".

Результаты прохождения тестов:

![[{D7C39ED9-E24D-481F-8381-3F224B14BA0F}.png]]

##### 7. Описание теста test-new-alg, полученный вывод и анализ полученного вывода

Был разработан тест test-new-alg:

```c title="test"
#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/init.h"
#include "threads/synch.h"
#include "threads/thread.h"
#include "devices/timer.h"
#include "threads/interrupt.h"
  
int priority_array[6] = {63, 43, 33, 25, 1, 63};
int64_t burst_array[6] = {8, 5, 1, 2, 5, 2};
  
  
void
new_alg_func (void *aux UNUSED)
{
    int64_t burst = thread_current()->cpu_burst;
    msg(">>> %s created with priority: %d, burst %d\n", thread_current()->name, thread_current()->priority, burst);
    int64_t ticks = timer_ticks();
    int64_t now = timer_ticks();
    while(now < ticks + burst)
        {
            intr_disable();
            if (timer_ticks() != now)
                {
                    msg(">>> %s: tick %d\n", thread_current()->name, now - ticks + 1);
                    now = timer_ticks();
                }
            intr_enable();
        }
    msg(">>> %s: completed\n", thread_current()->name);
}
  
void
test_new_alg ()
{
    msg(">>> Test main thread priority set to MAX.");
    thread_set_priority(PRI_MAX);
    for(int i = 0; i < 6; i++)
        {
            char name[16];
            snprintf(name, sizeof(name), "Thread %d", i);
            thread_create_burst(name, priority_array[i], burst_array[i], new_alg_func, NULL);
        }
    msg(">>> Test main thread priority set to MIN.");
    thread_set_priority(PRI_MIN);
    thread_yield();
}
```

Его вывод сходится с показаниями таблицы, рассчитанной в начале работы:

```bash title="Out"
begin
>>> Test main thread priority set to MAX.
>>> Test main thread priority set to MIN.
>>> Thread 0 created with priority: 63, burst 8
>>> Thread 5 created with priority: 63, burst 2
>>> Thread 0: tick 1
>>> Thread 5: tick 1
>>> Thread 0: tick 4
>>> Thread 5: completed
>>> Thread 0: completed
>>> Thread 1 created with priority: 43, burst 5
>>> Thread 1: tick 1
>>> Thread 1: tick 2
>>> Thread 1: tick 3
>>> Thread 1: tick 4
>>> Thread 1: tick 5
>>> Thread 1: completed
>>> Thread 2 created with priority: 33, burst 1
>>> Thread 2: tick 1
>>> Thread 2: completed
>>> Thread 3 created with priority: 25, burst 2
>>> Thread 3: tick 1
>>> Thread 3: tick 2
>>> Thread 3: completed
>>> Thread 4 created with priority: 1, burst 5
>>> Thread 4: tick 1
>>> Thread 4: tick 2
>>> Thread 4: tick 3
>>> Thread 4: tick 4
>>> Thread 4: tick 5 
>>> Thread 4: completed
end
```

А именно: последним исполняется 4 поток, перед ним - 3, 2 и 1 соответственно, потоки 0 и 5 исполняются первыми и, так как имеют одинаковый приоритет, согласно алгоритму исполняются по подалгоритму RR.
