> **Внимание!** Для выполнения этой работы требуется использовать специально подготовленную редакцию ОС pintos, доступную в разделе [файлы](http://ibks.spbstu.ru:3508/files).  
> Ваш вариант задания: 13

Так как я использую версию для докера, были дополнительны скачаны файлы "narrow-bridge.c", "narrow-bridge.h", "narrow-bridge-test.c", "rolcoast.c" (для индивидуального задания)

В одном немецком городе издавна существует узкий мост, движение транспорта по которому ограничено следующими правилами: 
1. Движение по мосту в каждый момент времени возможно только в одном направлении; 
2. Если три автомобиля попытаются пересечь мост одновременно, он разрушится под их весом; 
3. Автомобили «Скорой помощи» должны проезжать по мосту вне очереди.

- Не более двух машин
- В одном направлении
- Скорая имеет наивысший приоритет

```c
void 
one_vehicle(enum car_priority prio, enum car_direction dir) 
{ 
	arrive_bridge(prio, dir); 
	cross_bridge(prio, dir); 
	exit_bridge(prio, dir); 
}
```

```c
enum
car_priority
{
	car_normal,
	car_emergency
};

enum
car_direction
{
	dir_left,
	dir_right
};
```

arrive_bridge -> "Up" sem
exit_bridge -> "Down" sem

- Нельзя отключать прерываний
- Нельзя обращаться к value, waiter семафоров и holder замков
- Нельзя использовать thread_block и thread_unblock
- Все процессы имеют одинаковый приоритет и регуляция происходит за счет логических выражений и примитив синхронизации
- Процесс автомобиля либо исполняется, либо заблокирован
- Изменения только в файле narrow-bridge.c

```c
void 
cross_bridge(enum car_priority prio, enum car_direction dir) 
{ 
	msg("Vehicle: %4s, prio: %s, direct: %s, ticks=%4llu", thread_current()->name, prio == car_emergency ? "emer" : "norm", dir == dir_left ? "l -> r" : "l <- r", (unsigned long long) timer_ticks ()); 
	timer_sleep(10); 
}
```

##### 1. Описание разработанного алгоритма синхронизации и обоснование его эффективности:

> - по какому алгоритму в разработанном решении работают примитивы синхронизации и причина выбора данного алгоритма; 
> - как определяется количество автомобилей, пересекающих мост в один момент времени; 
> - каким образом разработанное решение гарантирует, что автомобили «Скорой помощи» пересекут мост вне очереди;

Принцип работы алгоритма использует семафоры как примитиву синхронизации, так как благодаря ним возможно реализовать "очередь ожидания" машин. 

Машины запускаются на мост функцией arrive_bridge, которая также устанавливает направление, с которым на мост могут заехать машины, текущее количество машин на мосту, а так же формирует очередь при необходимости.

После завершения проезда моста, в функции exit_bridge изменяется количестов машин на мосту, а так же реализуется логика выбора новых машин из "очереди", как только мост освободился.

Алгоритм избегает "голодания процессов", т.к. чередует стороны, с которых проезжают машины, но при этом машины с приоритетом "Скорая помощь" преодолевают мост первыми (т.е. порядок проверки для заезда на мост при завершившемся проезде двух машин слева направо: есть ли "скорые помощи" справа -> если есть, они пропускаются, пока не проедут все или пока на мост не заедет две машины -> если их нет, аналогично проверяется, есть ли "скорые помощи" слева -> если количества машин с приоритетом "скорая помощь" меньше 2, запускаются машины обычного приоритета в таком же порядке проверки направления).

##### 2. Исходный код реализованных функций narrow_bridge_init(), arrive_bridge() и exit_bridge() с подробными комментариями

```c title=init
void
narrow_bridge_init(void)
{
  
    sema_init(&left_emergency_c, 0);
    sema_init(&right_emergency_c, 0);
    sema_init(&left_usual_c, 0);
    sema_init(&right_usual_c, 0);
  
    left_emergency_q = 0;
    left_usual_q = 0;
    right_emergency_q = 0;
    right_usual_q = 0;  
  
    bridge_direction_right_q = 0;
    bridge_direction_left_q = 0;
  
}
```

Было реализовано 4 семафора: для каждого типа машины на каждой стороне, отслеживающих количество машин "в очереди". Также инициализируются переменные, хранящие количество машин каждого типа, ожидающих освобождения моста и количество машин, проезжающих мост в данный момент.

```c title=arrive
void
arrive_bridge(enum car_priority prio UNUSED, enum car_direction dir UNUSED

{
  
    bool permit = car_permition(dir);
  
    if (!permit) sems_control(prio, dir);
}
```

С помощью функции car_permition проверяется, может ли текущая машина заехать на мост, после чего, при необходимости, ставит ее в очередь с помощью функции sems_control.

```c title=permition
bool
car_permition(enum car_direction dir)
{
    if (bridge_direction_left_q + bridge_direction_right_q == 0)     
        {
            bridge_direction_left_q = (dir == dir_left) ? 1 : 0;
            bridge_direction_right_q = (dir == dir_left) ? 0 : 1;
  
            return true;
        }
    else if (bridge_direction_left_q == 1 && dir == dir_left)        
        {
            bridge_direction_left_q++;
  
            return true;
        }
    else if (bridge_direction_right_q == 1 && dir == dir_right)
        {
            bridge_direction_right_q++;
  
            return true;
        }
    else
        {
            return false;
        }
}
```

Функция car_permition учитывает случаи: когда на мосту нет машин, когда на мосту есть машина и ее направление совпадает с направлением текущей машины, и когда на мосту 2 машины или направление машин на мосту не совпало с направлением текущей машины.

```c title=control
void
sems_control (enum car_priority prio, enum car_direction dir)
{
    if (prio == car_emergency)
        {
            if (dir == dir_left)
                {
                    left_emergency_q++;
                    sema_down(&left_emergency_c);
                }
            else if (dir == dir_right)
                {
                    right_emergency_q++;
                    sema_down(&right_emergency_c);
                }
        }
  
    else if (prio == car_normal)
        {
            if (dir == dir_left)
                {
                    left_usual_q++;
                    sema_down(&left_usual_c);
                }
            else if (dir == dir_right)
                {
                    right_usual_q++;
                    sema_down(&right_usual_c);
                }
        }
}
```

Функция sems_control ставит текущую машину в соответствующую очередь за счет команды семафора down, учитывая ее направление и тип.

```c title=exit
void exit_bridge(enum car_priority prio UNUSED, enum car_direction dir UNUSED)
{
  
    enum car_direction prev_direction = (bridge_direction_left_q > 0) ? dir_left : dir_right;
    bridge_direction_left_q = (prev_direction == dir_left) ? bridge_direction_left_q - 1 : bridge_direction_left_q;
    bridge_direction_right_q = (prev_direction == dir_right) ? bridge_direction_right_q - 1 : bridge_direction_right_q;
  
    if (bridge_direction_left_q + bridge_direction_right_q == 0) car_pass(prev_direction);
  
}
```

Устанавливается направление машин и изменяется количество машин на мосту, после чего в случае, когда мост становится пуст, удержание машин в "очередях"
регулируется за счет "car_pass".

```c title=pass
void
car_pass (enum car_priority dir)
{
    if (dir == dir_left)
        {
            if (right_emergency_q > 0)
                {
                    emergency_pass(dir_right);
                    usual_pass(dir_right);
                }
            else if (left_emergency_q > 0)
                {
                    emergency_pass(dir_left);
                    usual_pass(dir_left);
                }
            else if (right_usual_q > 0)
                {
                    usual_pass(dir_right);
                }
            else if (left_usual_q > 0)
                {
                    usual_pass(dir_left);
                }
        }
    if (dir == dir_right)
        {
            if (left_emergency_q > 0)
                {
                    emergency_pass(dir_left);
                    usual_pass(dir_left);
                }
            else if (right_emergency_q > 0)
                {
                    emergency_pass(dir_right);
                    usual_pass(dir_right);
                }
            else if (left_usual_q > 0)
                {
                    usual_pass(dir_left);
                }
            else if (right_usual_q > 0)
                {
                    usual_pass(dir_right);
                }
        }
}
```

Функция car_pass, учитывая максимальное возможное количество машин одновременно находящихся на мосту, запускает на мост машины в порядке приоритета:
сначала проверяются машины "скорой помощи" с противоположной стороны согласно направлению машины, последней проехавшей мост, после машины "скорой помощи" с текущей стороны, после "обычные" машины с противоположной стороны, и в конце "обычные" машины с текущей стороны, что позволяет пропускать машины скорой помощи вне очереди и избежать "голодания процессов".

```c title=emergency_pass
void
emergency_pass (enum car_priority dir)
{
    if (dir == dir_left)
        {
            while (left_emergency_q > 0 && bridge_direction_right_q + bridge_direction_left_q < 2)
                {
                    bridge_direction_left_q++;
                    left_emergency_q--;
                    sema_up(&left_emergency_c);
                }
        }
    else if (dir == dir_right)
        {
            while (right_emergency_q > 0 && bridge_direction_right_q + bridge_direction_left_q < 2)
                {
                    bridge_direction_right_q++;
                    right_emergency_q--;
                    sema_up(&right_emergency_c);
                }
        }
}
```

Функция emergency_pass выпускает из соответствующей "очереди" машину, изменяя при этом соответствующие характеристики, пока машин в этой очереди не останется, или пока на мосту не окажется 2 машины.
Аналогично работает usual_pass.

##### 3. Диаграмма взаимодействия процессов на «узком мосте» в зависимости от их количества и состояний примитивов синхронизации. Показать, как осуществляется ограничение доступа к разделяемому ресурсу.

![[diagram 3.png]]

##### 4. Полученный в результате выполнения программы вывод

pintos --qemu -- -q run "narrow-bridge 0 0 0 0" 
![[{5B53DB1C-02E0-473B-8E37-9AD84E46C121}.png]]
```
(narrow-bridge) begin
(narrow-bridge) end
```
pintos --qemu -- -q run "narrow-bridge 0 0 0 1" 
![[{2B8C9ED1-406E-415D-AD18-9B2A98A52F7F}.png]]
```
(narrow-bridge) begin
(narrow-bridge) Vehicle:    1, prio: emer, direct: l <- r, ticks=  26
(narrow-bridge) end
```
pintos --qemu -- -q run "narrow-bridge 0 4 0 0" 
![[{67A011B8-4027-489A-BF6F-38F24F875FF2}.png]]
```
(narrow-bridge) begin
(narrow-bridge) Vehicle:    1, prio: norm, direct: l <- r, ticks=  25
(narrow-bridge) Vehicle:    2, prio: norm, direct: l <- r, ticks=  25
(narrow-bridge) Vehicle:    3, prio: norm, direct: l <- r, ticks=  35
(narrow-bridge) Vehicle:    4, prio: norm, direct: l <- r, ticks=  35
(narrow-bridge) end
```
pintos --qemu -- -q run "narrow-bridge 0 0 4 0" 
![[{906F7486-7E32-4E6A-A3AC-250843454C93}.png]]
```
(narrow-bridge) begin
(narrow-bridge) Vehicle:    1, prio: emer, direct: l -> r, ticks=  26
(narrow-bridge) Vehicle:    2, prio: emer, direct: l -> r, ticks=  26
(narrow-bridge) Vehicle:    3, prio: emer, direct: l -> r, ticks=  38
(narrow-bridge) Vehicle:    4, prio: emer, direct: l -> r, ticks=  38
(narrow-bridge) end
```
pintos --qemu -- -q run "narrow-bridge 3 3 3 3" 
![[{F423E73E-2001-4F80-8782-8F229A5363AE}.png]]
```
(narrow-bridge) begin
(narrow-bridge) Vehicle:    1, prio: norm, direct: l -> r, ticks=  26
(narrow-bridge) Vehicle:    2, prio: norm, direct: l -> r, ticks=  26
(narrow-bridge) Vehicle:   10, prio: emer, direct: l <- r, ticks=  36
(narrow-bridge) Vehicle:   11, prio: emer, direct: l <- r, ticks=  36
(narrow-bridge) Vehicle:    7, prio: emer, direct: l -> r, ticks=  46
(narrow-bridge) Vehicle:    8, prio: emer, direct: l -> r, ticks=  46
(narrow-bridge) Vehicle:   12, prio: emer, direct: l <- r, ticks=  56
(narrow-bridge) Vehicle:    4, prio: norm, direct: l <- r, ticks=  56
(narrow-bridge) Vehicle:    9, prio: emer, direct: l -> r, ticks=  66
(narrow-bridge) Vehicle:    3, prio: norm, direct: l -> r, ticks=  66
(narrow-bridge) Vehicle:    5, prio: norm, direct: l <- r, ticks=  76
(narrow-bridge) Vehicle:    6, prio: norm, direct: l <- r, ticks=  76
(narrow-bridge) end
```
pintos --qemu -- -q run "narrow-bridge 4 3 4 3" 
![[{B382A4B3-6CBA-4221-A49A-02090558693E}.png]]
```
(narrow-bridge) begin
(narrow-bridge) Vehicle:    1, prio: norm, direct: l -> r, ticks=  27
(narrow-bridge) Vehicle:    2, prio: norm, direct: l -> r, ticks=  27
(narrow-bridge) Vehicle:   12, prio: emer, direct: l <- r, ticks=  37
(narrow-bridge) Vehicle:   13, prio: emer, direct: l <- r, ticks=  37
(narrow-bridge) Vehicle:    8, prio: emer, direct: l -> r, ticks=  47
(narrow-bridge) Vehicle:    9, prio: emer, direct: l -> r, ticks=  47
(narrow-bridge) Vehicle:   14, prio: emer, direct: l <- r, ticks=  57
(narrow-bridge) Vehicle:    5, prio: norm, direct: l <- r, ticks=  57
(narrow-bridge) Vehicle:   10, prio: emer, direct: l -> r, ticks=  67
(narrow-bridge) Vehicle:   11, prio: emer, direct: l -> r, ticks=  67
(narrow-bridge) Vehicle:    6, prio: norm, direct: l <- r, ticks=  77
(narrow-bridge) Vehicle:    7, prio: norm, direct: l <- r, ticks=  77
(narrow-bridge) Vehicle:    3, prio: norm, direct: l -> r, ticks=  87
(narrow-bridge) Vehicle:    4, prio: norm, direct: l -> r, ticks=  87
(narrow-bridge) end
```
pintos --qemu -- -q run "narrow-bridge 7 23 17 1" 
![[{FEB71766-8665-4747-9C59-5C5175C5CC84}.png]]
```
(narrow-bridge) begin
(narrow-bridge) Vehicle:    1, prio: norm, direct: l -> r, ticks=  27
(narrow-bridge) Vehicle:    2, prio: norm, direct: l -> r, ticks=  28
(narrow-bridge) Vehicle:   48, prio: emer, direct: l <- r, ticks=  38
(narrow-bridge) Vehicle:    8, prio: norm, direct: l <- r, ticks=  38
(narrow-bridge) Vehicle:   31, prio: emer, direct: l -> r, ticks=  48
(narrow-bridge) Vehicle:   32, prio: emer, direct: l -> r, ticks=  48
(narrow-bridge) Vehicle:   33, prio: emer, direct: l -> r, ticks=  58
(narrow-bridge) Vehicle:   34, prio: emer, direct: l -> r, ticks=  58
(narrow-bridge) Vehicle:   35, prio: emer, direct: l -> r, ticks=  68
(narrow-bridge) Vehicle:   36, prio: emer, direct: l -> r, ticks=  68
(narrow-bridge) Vehicle:   37, prio: emer, direct: l -> r, ticks=  78
(narrow-bridge) Vehicle:   38, prio: emer, direct: l -> r, ticks=  78
(narrow-bridge) Vehicle:   39, prio: emer, direct: l -> r, ticks=  88
(narrow-bridge) Vehicle:   40, prio: emer, direct: l -> r, ticks=  88
(narrow-bridge) Vehicle:   41, prio: emer, direct: l -> r, ticks=  98
(narrow-bridge) Vehicle:   42, prio: emer, direct: l -> r, ticks=  98
(narrow-bridge) Vehicle:   43, prio: emer, direct: l -> r, ticks= 108
(narrow-bridge) Vehicle:   44, prio: emer, direct: l -> r, ticks= 108
(narrow-bridge) Vehicle:   45, prio: emer, direct: l -> r, ticks= 118
(narrow-bridge) Vehicle:   46, prio: emer, direct: l -> r, ticks= 118
(narrow-bridge) Vehicle:   47, prio: emer, direct: l -> r, ticks= 128
(narrow-bridge) Vehicle:    3, prio: norm, direct: l -> r, ticks= 128
(narrow-bridge) Vehicle:    9, prio: norm, direct: l <- r, ticks= 138
(narrow-bridge) Vehicle:   10, prio: norm, direct: l <- r, ticks= 138
(narrow-bridge) Vehicle:    4, prio: norm, direct: l -> r, ticks= 148
(narrow-bridge) Vehicle:    5, prio: norm, direct: l -> r, ticks= 148
(narrow-bridge) Vehicle:   11, prio: norm, direct: l <- r, ticks= 158
(narrow-bridge) Vehicle:   12, prio: norm, direct: l <- r, ticks= 158
(narrow-bridge) Vehicle:    6, prio: norm, direct: l -> r, ticks= 168
(narrow-bridge) Vehicle:    7, prio: norm, direct: l -> r, ticks= 168
(narrow-bridge) Vehicle:   13, prio: norm, direct: l <- r, ticks= 179
(narrow-bridge) Vehicle:   14, prio: norm, direct: l <- r, ticks= 179
(narrow-bridge) Vehicle:   15, prio: norm, direct: l <- r, ticks= 189
(narrow-bridge) Vehicle:   16, prio: norm, direct: l <- r, ticks= 189
(narrow-bridge) Vehicle:   17, prio: norm, direct: l <- r, ticks= 199
(narrow-bridge) Vehicle:   18, prio: norm, direct: l <- r, ticks= 199
(narrow-bridge) Vehicle:   19, prio: norm, direct: l <- r, ticks= 209
(narrow-bridge) Vehicle:   20, prio: norm, direct: l <- r, ticks= 209
(narrow-bridge) Vehicle:   21, prio: norm, direct: l <- r, ticks= 220
(narrow-bridge) Vehicle:   22, prio: norm, direct: l <- r, ticks= 220
(narrow-bridge) Vehicle:   23, prio: norm, direct: l <- r, ticks= 230
(narrow-bridge) Vehicle:   24, prio: norm, direct: l <- r, ticks= 231
(narrow-bridge) Vehicle:   25, prio: norm, direct: l <- r, ticks= 241
(narrow-bridge) Vehicle:   26, prio: norm, direct: l <- r, ticks= 241
(narrow-bridge) Vehicle:   27, prio: norm, direct: l <- r, ticks= 252
(narrow-bridge) Vehicle:   28, prio: norm, direct: l <- r, ticks= 252
(narrow-bridge) Vehicle:   29, prio: norm, direct: l <- r, ticks= 263
(narrow-bridge) Vehicle:   30, prio: norm, direct: l <- r, ticks= 263
(narrow-bridge) end
```
pintos --qemu -- -q run "narrow-bridge 40 30 0 0" 
![[{556B727E-7D98-47ED-A86C-1493300D09F8}.png]]
```
(narrow-bridge) begin
(narrow-bridge) Vehicle:    1, prio: norm, direct: l -> r, ticks=  25
(narrow-bridge) Vehicle:    2, prio: norm, direct: l -> r, ticks=  25
(narrow-bridge) Vehicle:   41, prio: norm, direct: l <- r, ticks=  36
(narrow-bridge) Vehicle:   42, prio: norm, direct: l <- r, ticks=  36
(narrow-bridge) Vehicle:    3, prio: norm, direct: l -> r, ticks=  46
(narrow-bridge) Vehicle:    4, prio: norm, direct: l -> r, ticks=  46
(narrow-bridge) Vehicle:   43, prio: norm, direct: l <- r, ticks=  56
(narrow-bridge) Vehicle:   44, prio: norm, direct: l <- r, ticks=  56
(narrow-bridge) Vehicle:    5, prio: norm, direct: l -> r, ticks=  66
(narrow-bridge) Vehicle:    6, prio: norm, direct: l -> r, ticks=  66
(narrow-bridge) Vehicle:   45, prio: norm, direct: l <- r, ticks=  76
(narrow-bridge) Vehicle:   46, prio: norm, direct: l <- r, ticks=  76
(narrow-bridge) Vehicle:    7, prio: norm, direct: l -> r, ticks=  86
(narrow-bridge) Vehicle:    8, prio: norm, direct: l -> r, ticks=  86
(narrow-bridge) Vehicle:   47, prio: norm, direct: l <- r, ticks=  96
(narrow-bridge) Vehicle:   48, prio: norm, direct: l <- r, ticks=  96
(narrow-bridge) Vehicle:    9, prio: norm, direct: l -> r, ticks= 106
(narrow-bridge) Vehicle:   10, prio: norm, direct: l -> r, ticks= 106
(narrow-bridge) Vehicle:   49, prio: norm, direct: l <- r, ticks= 116
(narrow-bridge) Vehicle:   50, prio: norm, direct: l <- r, ticks= 116
(narrow-bridge) Vehicle:   11, prio: norm, direct: l -> r, ticks= 126
(narrow-bridge) Vehicle:   12, prio: norm, direct: l -> r, ticks= 126
(narrow-bridge) Vehicle:   51, prio: norm, direct: l <- r, ticks= 136
(narrow-bridge) Vehicle:   52, prio: norm, direct: l <- r, ticks= 136
(narrow-bridge) Vehicle:   13, prio: norm, direct: l -> r, ticks= 146
(narrow-bridge) Vehicle:   14, prio: norm, direct: l -> r, ticks= 146
(narrow-bridge) Vehicle:   53, prio: norm, direct: l <- r, ticks= 156
(narrow-bridge) Vehicle:   54, prio: norm, direct: l <- r, ticks= 156
(narrow-bridge) Vehicle:   15, prio: norm, direct: l -> r, ticks= 166
(narrow-bridge) Vehicle:   16, prio: norm, direct: l -> r, ticks= 166
(narrow-bridge) Vehicle:   55, prio: norm, direct: l <- r, ticks= 176
(narrow-bridge) Vehicle:   56, prio: norm, direct: l <- r, ticks= 176
(narrow-bridge) Vehicle:   17, prio: norm, direct: l -> r, ticks= 186
(narrow-bridge) Vehicle:   18, prio: norm, direct: l -> r, ticks= 186
(narrow-bridge) Vehicle:   57, prio: norm, direct: l <- r, ticks= 196
(narrow-bridge) Vehicle:   58, prio: norm, direct: l <- r, ticks= 196
(narrow-bridge) Vehicle:   19, prio: norm, direct: l -> r, ticks= 206
(narrow-bridge) Vehicle:   20, prio: norm, direct: l -> r, ticks= 206
(narrow-bridge) Vehicle:   59, prio: norm, direct: l <- r, ticks= 216
(narrow-bridge) Vehicle:   60, prio: norm, direct: l <- r, ticks= 216
(narrow-bridge) Vehicle:   21, prio: norm, direct: l -> r, ticks= 226
(narrow-bridge) Vehicle:   22, prio: norm, direct: l -> r, ticks= 226
(narrow-bridge) Vehicle:   61, prio: norm, direct: l <- r, ticks= 236
(narrow-bridge) Vehicle:   62, prio: norm, direct: l <- r, ticks= 236
(narrow-bridge) Vehicle:   23, prio: norm, direct: l -> r, ticks= 246
(narrow-bridge) Vehicle:   24, prio: norm, direct: l -> r, ticks= 246
(narrow-bridge) Vehicle:   63, prio: norm, direct: l <- r, ticks= 256
(narrow-bridge) Vehicle:   64, prio: norm, direct: l <- r, ticks= 256
(narrow-bridge) Vehicle:   25, prio: norm, direct: l -> r, ticks= 266
(narrow-bridge) Vehicle:   26, prio: norm, direct: l -> r, ticks= 266
(narrow-bridge) Vehicle:   65, prio: norm, direct: l <- r, ticks= 276
(narrow-bridge) Vehicle:   66, prio: norm, direct: l <- r, ticks= 276
(narrow-bridge) Vehicle:   27, prio: norm, direct: l -> r, ticks= 286
(narrow-bridge) Vehicle:   28, prio: norm, direct: l -> r, ticks= 286
(narrow-bridge) Vehicle:   67, prio: norm, direct: l <- r, ticks= 296
(narrow-bridge) Vehicle:   68, prio: norm, direct: l <- r, ticks= 296
(narrow-bridge) Vehicle:   29, prio: norm, direct: l -> r, ticks= 306
(narrow-bridge) Vehicle:   30, prio: norm, direct: l -> r, ticks= 306
(narrow-bridge) Vehicle:   69, prio: norm, direct: l <- r, ticks= 316
(narrow-bridge) Vehicle:   70, prio: norm, direct: l <- r, ticks= 316
(narrow-bridge) Vehicle:   31, prio: norm, direct: l -> r, ticks= 326
(narrow-bridge) Vehicle:   32, prio: norm, direct: l -> r, ticks= 326
(narrow-bridge) Vehicle:   33, prio: norm, direct: l -> r, ticks= 336
(narrow-bridge) Vehicle:   34, prio: norm, direct: l -> r, ticks= 336
(narrow-bridge) Vehicle:   35, prio: norm, direct: l -> r, ticks= 346
(narrow-bridge) Vehicle:   36, prio: norm, direct: l -> r, ticks= 346
(narrow-bridge) Vehicle:   37, prio: norm, direct: l -> r, ticks= 356
(narrow-bridge) Vehicle:   38, prio: norm, direct: l -> r, ticks= 356
(narrow-bridge) Vehicle:   39, prio: norm, direct: l -> r, ticks= 366
(narrow-bridge) Vehicle:   40, prio: norm, direct: l -> r, ticks= 366
(narrow-bridge) end
```
pintos --qemu -- -q run "narrow-bridge 30 40 0 0" 
![[{1D2B5C9D-4FDC-4A21-91DC-5F72CADD4D1B}.png]]
```
(narrow-bridge) begin
(narrow-bridge) Vehicle:    1, prio: norm, direct: l -> r, ticks=  25
(narrow-bridge) Vehicle:    2, prio: norm, direct: l -> r, ticks=  25
(narrow-bridge) Vehicle:   31, prio: norm, direct: l <- r, ticks=  36
(narrow-bridge) Vehicle:   32, prio: norm, direct: l <- r, ticks=  36
(narrow-bridge) Vehicle:    3, prio: norm, direct: l -> r, ticks=  46
(narrow-bridge) Vehicle:    4, prio: norm, direct: l -> r, ticks=  46
(narrow-bridge) Vehicle:   33, prio: norm, direct: l <- r, ticks=  56
(narrow-bridge) Vehicle:   34, prio: norm, direct: l <- r, ticks=  56
(narrow-bridge) Vehicle:    5, prio: norm, direct: l -> r, ticks=  66
(narrow-bridge) Vehicle:    6, prio: norm, direct: l -> r, ticks=  66
(narrow-bridge) Vehicle:   35, prio: norm, direct: l <- r, ticks=  76
(narrow-bridge) Vehicle:   36, prio: norm, direct: l <- r, ticks=  76
(narrow-bridge) Vehicle:    7, prio: norm, direct: l -> r, ticks=  86
(narrow-bridge) Vehicle:    8, prio: norm, direct: l -> r, ticks=  86
(narrow-bridge) Vehicle:   37, prio: norm, direct: l <- r, ticks=  96
(narrow-bridge) Vehicle:   38, prio: norm, direct: l <- r, ticks=  96
(narrow-bridge) Vehicle:    9, prio: norm, direct: l -> r, ticks= 106
(narrow-bridge) Vehicle:   10, prio: norm, direct: l -> r, ticks= 106
(narrow-bridge) Vehicle:   39, prio: norm, direct: l <- r, ticks= 116
(narrow-bridge) Vehicle:   40, prio: norm, direct: l <- r, ticks= 116
(narrow-bridge) Vehicle:   11, prio: norm, direct: l -> r, ticks= 126
(narrow-bridge) Vehicle:   12, prio: norm, direct: l -> r, ticks= 126
(narrow-bridge) Vehicle:   41, prio: norm, direct: l <- r, ticks= 137
(narrow-bridge) Vehicle:   42, prio: norm, direct: l <- r, ticks= 137
(narrow-bridge) Vehicle:   13, prio: norm, direct: l -> r, ticks= 147
(narrow-bridge) Vehicle:   14, prio: norm, direct: l -> r, ticks= 147
(narrow-bridge) Vehicle:   43, prio: norm, direct: l <- r, ticks= 157
(narrow-bridge) Vehicle:   44, prio: norm, direct: l <- r, ticks= 157
(narrow-bridge) Vehicle:   15, prio: norm, direct: l -> r, ticks= 167
(narrow-bridge) Vehicle:   16, prio: norm, direct: l -> r, ticks= 167
(narrow-bridge) Vehicle:   45, prio: norm, direct: l <- r, ticks= 177
(narrow-bridge) Vehicle:   46, prio: norm, direct: l <- r, ticks= 177
(narrow-bridge) Vehicle:   17, prio: norm, direct: l -> r, ticks= 187
(narrow-bridge) Vehicle:   18, prio: norm, direct: l -> r, ticks= 187
(narrow-bridge) Vehicle:   47, prio: norm, direct: l <- r, ticks= 197
(narrow-bridge) Vehicle:   48, prio: norm, direct: l <- r, ticks= 197
(narrow-bridge) Vehicle:   19, prio: norm, direct: l -> r, ticks= 207
(narrow-bridge) Vehicle:   20, prio: norm, direct: l -> r, ticks= 207
(narrow-bridge) Vehicle:   49, prio: norm, direct: l <- r, ticks= 217
(narrow-bridge) Vehicle:   50, prio: norm, direct: l <- r, ticks= 217
(narrow-bridge) Vehicle:   21, prio: norm, direct: l -> r, ticks= 227
(narrow-bridge) Vehicle:   22, prio: norm, direct: l -> r, ticks= 227
(narrow-bridge) Vehicle:   51, prio: norm, direct: l <- r, ticks= 238
(narrow-bridge) Vehicle:   52, prio: norm, direct: l <- r, ticks= 238
(narrow-bridge) Vehicle:   23, prio: norm, direct: l -> r, ticks= 248
(narrow-bridge) Vehicle:   24, prio: norm, direct: l -> r, ticks= 248
(narrow-bridge) Vehicle:   53, prio: norm, direct: l <- r, ticks= 258
(narrow-bridge) Vehicle:   54, prio: norm, direct: l <- r, ticks= 258
(narrow-bridge) Vehicle:   25, prio: norm, direct: l -> r, ticks= 268
(narrow-bridge) Vehicle:   26, prio: norm, direct: l -> r, ticks= 268
(narrow-bridge) Vehicle:   55, prio: norm, direct: l <- r, ticks= 278
(narrow-bridge) Vehicle:   56, prio: norm, direct: l <- r, ticks= 278
(narrow-bridge) Vehicle:   27, prio: norm, direct: l -> r, ticks= 288
(narrow-bridge) Vehicle:   28, prio: norm, direct: l -> r, ticks= 288
(narrow-bridge) Vehicle:   57, prio: norm, direct: l <- r, ticks= 298
(narrow-bridge) Vehicle:   58, prio: norm, direct: l <- r, ticks= 298
(narrow-bridge) Vehicle:   29, prio: norm, direct: l -> r, ticks= 308
(narrow-bridge) Vehicle:   30, prio: norm, direct: l -> r, ticks= 308
(narrow-bridge) Vehicle:   59, prio: norm, direct: l <- r, ticks= 318
(narrow-bridge) Vehicle:   60, prio: norm, direct: l <- r, ticks= 318
(narrow-bridge) Vehicle:   61, prio: norm, direct: l <- r, ticks= 328
(narrow-bridge) Vehicle:   62, prio: norm, direct: l <- r, ticks= 328
(narrow-bridge) Vehicle:   63, prio: norm, direct: l <- r, ticks= 338
(narrow-bridge) Vehicle:   64, prio: norm, direct: l <- r, ticks= 338
(narrow-bridge) Vehicle:   65, prio: norm, direct: l <- r, ticks= 348
(narrow-bridge) Vehicle:   66, prio: norm, direct: l <- r, ticks= 348
(narrow-bridge) Vehicle:   67, prio: norm, direct: l <- r, ticks= 358
(narrow-bridge) Vehicle:   68, prio: norm, direct: l <- r, ticks= 358
(narrow-bridge) Vehicle:   69, prio: norm, direct: l <- r, ticks= 368
(narrow-bridge) Vehicle:   70, prio: norm, direct: l <- r, ticks= 368
(narrow-bridge) end
```
pintos --qemu -- -q run "narrow-bridge 23 23 1 11" 
![[{0247C8CA-6844-4FD1-B44C-7982E4AAEC54}.png]]
```(narrow-bridge) begin
(narrow-bridge) Vehicle:    1, prio: norm, direct: l -> r, ticks=  25
(narrow-bridge) Vehicle:    2, prio: norm, direct: l -> r, ticks=  25
(narrow-bridge) Vehicle:   48, prio: emer, direct: l <- r, ticks=  35
(narrow-bridge) Vehicle:   49, prio: emer, direct: l <- r, ticks=  35
(narrow-bridge) Vehicle:   47, prio: emer, direct: l -> r, ticks=  45
(narrow-bridge) Vehicle:    3, prio: norm, direct: l -> r, ticks=  45
(narrow-bridge) Vehicle:   50, prio: emer, direct: l <- r, ticks=  55
(narrow-bridge) Vehicle:   51, prio: emer, direct: l <- r, ticks=  55
(narrow-bridge) Vehicle:   52, prio: emer, direct: l <- r, ticks=  65
(narrow-bridge) Vehicle:   53, prio: emer, direct: l <- r, ticks=  65
(narrow-bridge) Vehicle:   54, prio: emer, direct: l <- r, ticks=  75
(narrow-bridge) Vehicle:   55, prio: emer, direct: l <- r, ticks=  75
(narrow-bridge) Vehicle:   56, prio: emer, direct: l <- r, ticks=  85
(narrow-bridge) Vehicle:   57, prio: emer, direct: l <- r, ticks=  85
(narrow-bridge) Vehicle:   58, prio: emer, direct: l <- r, ticks=  95
(narrow-bridge) Vehicle:   24, prio: norm, direct: l <- r, ticks=  95
(narrow-bridge) Vehicle:    4, prio: norm, direct: l -> r, ticks= 105
(narrow-bridge) Vehicle:    5, prio: norm, direct: l -> r, ticks= 105
(narrow-bridge) Vehicle:   25, prio: norm, direct: l <- r, ticks= 115
(narrow-bridge) Vehicle:   26, prio: norm, direct: l <- r, ticks= 115
(narrow-bridge) Vehicle:    6, prio: norm, direct: l -> r, ticks= 125
(narrow-bridge) Vehicle:    7, prio: norm, direct: l -> r, ticks= 125
(narrow-bridge) Vehicle:   27, prio: norm, direct: l <- r, ticks= 135
(narrow-bridge) Vehicle:   28, prio: norm, direct: l <- r, ticks= 135
(narrow-bridge) Vehicle:    8, prio: norm, direct: l -> r, ticks= 145
(narrow-bridge) Vehicle:    9, prio: norm, direct: l -> r, ticks= 145
(narrow-bridge) Vehicle:   29, prio: norm, direct: l <- r, ticks= 155
(narrow-bridge) Vehicle:   30, prio: norm, direct: l <- r, ticks= 155
(narrow-bridge) Vehicle:   10, prio: norm, direct: l -> r, ticks= 165
(narrow-bridge) Vehicle:   11, prio: norm, direct: l -> r, ticks= 165
(narrow-bridge) Vehicle:   31, prio: norm, direct: l <- r, ticks= 175
(narrow-bridge) Vehicle:   32, prio: norm, direct: l <- r, ticks= 175
(narrow-bridge) Vehicle:   12, prio: norm, direct: l -> r, ticks= 185
(narrow-bridge) Vehicle:   13, prio: norm, direct: l -> r, ticks= 185
(narrow-bridge) Vehicle:   33, prio: norm, direct: l <- r, ticks= 195
(narrow-bridge) Vehicle:   34, prio: norm, direct: l <- r, ticks= 195
(narrow-bridge) Vehicle:   14, prio: norm, direct: l -> r, ticks= 205
(narrow-bridge) Vehicle:   15, prio: norm, direct: l -> r, ticks= 205
(narrow-bridge) Vehicle:   35, prio: norm, direct: l <- r, ticks= 215
(narrow-bridge) Vehicle:   36, prio: norm, direct: l <- r, ticks= 215
(narrow-bridge) Vehicle:   16, prio: norm, direct: l -> r, ticks= 225
(narrow-bridge) Vehicle:   17, prio: norm, direct: l -> r, ticks= 225
(narrow-bridge) Vehicle:   37, prio: norm, direct: l <- r, ticks= 235
(narrow-bridge) Vehicle:   38, prio: norm, direct: l <- r, ticks= 235
(narrow-bridge) Vehicle:   18, prio: norm, direct: l -> r, ticks= 245
(narrow-bridge) Vehicle:   19, prio: norm, direct: l -> r, ticks= 245
(narrow-bridge) Vehicle:   39, prio: norm, direct: l <- r, ticks= 255
(narrow-bridge) Vehicle:   40, prio: norm, direct: l <- r, ticks= 255
(narrow-bridge) Vehicle:   20, prio: norm, direct: l -> r, ticks= 265
(narrow-bridge) Vehicle:   21, prio: norm, direct: l -> r, ticks= 265
(narrow-bridge) Vehicle:   41, prio: norm, direct: l <- r, ticks= 276
(narrow-bridge) Vehicle:   42, prio: norm, direct: l <- r, ticks= 276
(narrow-bridge) Vehicle:   22, prio: norm, direct: l -> r, ticks= 286
(narrow-bridge) Vehicle:   23, prio: norm, direct: l -> r, ticks= 286
(narrow-bridge) Vehicle:   43, prio: norm, direct: l <- r, ticks= 296
(narrow-bridge) Vehicle:   44, prio: norm, direct: l <- r, ticks= 296
(narrow-bridge) Vehicle:   45, prio: norm, direct: l <- r, ticks= 306
(narrow-bridge) Vehicle:   46, prio: norm, direct: l <- r, ticks= 306
(narrow-bridge) end
```
pintos --qemu -- -q run "narrow-bridge 22 22 10 10" 
![[{B615915B-9662-41F3-9111-E453E8CD413E}.png]]
```
(narrow-bridge) begin
(narrow-bridge) Vehicle:    1, prio: norm, direct: l -> r, ticks=  25
(narrow-bridge) Vehicle:    2, prio: norm, direct: l -> r, ticks=  25
(narrow-bridge) Vehicle:   55, prio: emer, direct: l <- r, ticks=  36
(narrow-bridge) Vehicle:   56, prio: emer, direct: l <- r, ticks=  36
(narrow-bridge) Vehicle:   45, prio: emer, direct: l -> r, ticks=  46
(narrow-bridge) Vehicle:   46, prio: emer, direct: l -> r, ticks=  46
(narrow-bridge) Vehicle:   57, prio: emer, direct: l <- r, ticks=  56
(narrow-bridge) Vehicle:   58, prio: emer, direct: l <- r, ticks=  56
(narrow-bridge) Vehicle:   47, prio: emer, direct: l -> r, ticks=  66
(narrow-bridge) Vehicle:   48, prio: emer, direct: l -> r, ticks=  66
(narrow-bridge) Vehicle:   59, prio: emer, direct: l <- r, ticks=  76
(narrow-bridge) Vehicle:   60, prio: emer, direct: l <- r, ticks=  76
(narrow-bridge) Vehicle:   49, prio: emer, direct: l -> r, ticks=  86
(narrow-bridge) Vehicle:   50, prio: emer, direct: l -> r, ticks=  86
(narrow-bridge) Vehicle:   61, prio: emer, direct: l <- r, ticks=  96
(narrow-bridge) Vehicle:   62, prio: emer, direct: l <- r, ticks=  96
(narrow-bridge) Vehicle:   51, prio: emer, direct: l -> r, ticks= 106
(narrow-bridge) Vehicle:   52, prio: emer, direct: l -> r, ticks= 106
(narrow-bridge) Vehicle:   63, prio: emer, direct: l <- r, ticks= 116
(narrow-bridge) Vehicle:   64, prio: emer, direct: l <- r, ticks= 116
(narrow-bridge) Vehicle:   53, prio: emer, direct: l -> r, ticks= 126
(narrow-bridge) Vehicle:   54, prio: emer, direct: l -> r, ticks= 126
(narrow-bridge) Vehicle:   23, prio: norm, direct: l <- r, ticks= 136
(narrow-bridge) Vehicle:   24, prio: norm, direct: l <- r, ticks= 136
(narrow-bridge) Vehicle:    3, prio: norm, direct: l -> r, ticks= 146
(narrow-bridge) Vehicle:    4, prio: norm, direct: l -> r, ticks= 146
(narrow-bridge) Vehicle:   25, prio: norm, direct: l <- r, ticks= 156
(narrow-bridge) Vehicle:   26, prio: norm, direct: l <- r, ticks= 156
(narrow-bridge) Vehicle:    5, prio: norm, direct: l -> r, ticks= 166
(narrow-bridge) Vehicle:    6, prio: norm, direct: l -> r, ticks= 166
(narrow-bridge) Vehicle:   27, prio: norm, direct: l <- r, ticks= 176
(narrow-bridge) Vehicle:   28, prio: norm, direct: l <- r, ticks= 176
(narrow-bridge) Vehicle:    7, prio: norm, direct: l -> r, ticks= 186
(narrow-bridge) Vehicle:    8, prio: norm, direct: l -> r, ticks= 186
(narrow-bridge) Vehicle:   29, prio: norm, direct: l <- r, ticks= 196
(narrow-bridge) Vehicle:   30, prio: norm, direct: l <- r, ticks= 196
(narrow-bridge) Vehicle:    9, prio: norm, direct: l -> r, ticks= 206
(narrow-bridge) Vehicle:   10, prio: norm, direct: l -> r, ticks= 206
(narrow-bridge) Vehicle:   31, prio: norm, direct: l <- r, ticks= 216
(narrow-bridge) Vehicle:   32, prio: norm, direct: l <- r, ticks= 216
(narrow-bridge) Vehicle:   11, prio: norm, direct: l -> r, ticks= 226
(narrow-bridge) Vehicle:   12, prio: norm, direct: l -> r, ticks= 226
(narrow-bridge) Vehicle:   33, prio: norm, direct: l <- r, ticks= 236
(narrow-bridge) Vehicle:   34, prio: norm, direct: l <- r, ticks= 236
(narrow-bridge) Vehicle:   13, prio: norm, direct: l -> r, ticks= 246
(narrow-bridge) Vehicle:   14, prio: norm, direct: l -> r, ticks= 246
(narrow-bridge) Vehicle:   35, prio: norm, direct: l <- r, ticks= 256
(narrow-bridge) Vehicle:   36, prio: norm, direct: l <- r, ticks= 256
(narrow-bridge) Vehicle:   15, prio: norm, direct: l -> r, ticks= 267
(narrow-bridge) Vehicle:   16, prio: norm, direct: l -> r, ticks= 267
(narrow-bridge) Vehicle:   37, prio: norm, direct: l <- r, ticks= 278
(narrow-bridge) Vehicle:   38, prio: norm, direct: l <- r, ticks= 278
(narrow-bridge) Vehicle:   17, prio: norm, direct: l -> r, ticks= 288
(narrow-bridge) Vehicle:   18, prio: norm, direct: l -> r, ticks= 288
(narrow-bridge) Vehicle:   39, prio: norm, direct: l <- r, ticks= 298
(narrow-bridge) Vehicle:   40, prio: norm, direct: l <- r, ticks= 298
(narrow-bridge) Vehicle:   19, prio: norm, direct: l -> r, ticks= 308
(narrow-bridge) Vehicle:   20, prio: norm, direct: l -> r, ticks= 308
(narrow-bridge) Vehicle:   41, prio: norm, direct: l <- r, ticks= 319
(narrow-bridge) Vehicle:   42, prio: norm, direct: l <- r, ticks= 319
(narrow-bridge) Vehicle:   21, prio: norm, direct: l -> r, ticks= 329
(narrow-bridge) Vehicle:   22, prio: norm, direct: l -> r, ticks= 329
(narrow-bridge) Vehicle:   43, prio: norm, direct: l <- r, ticks= 339
(narrow-bridge) Vehicle:   44, prio: norm, direct: l <- r, ticks= 339
(narrow-bridge) end
```
pintos --qemu -- -q run "narrow-bridge 0 0 11 12" 
![[{FF9FF565-40DC-4F99-99B1-29461701CB42}.png]]
```
(narrow-bridge) begin
(narrow-bridge) Vehicle:    1, prio: emer, direct: l -> r, ticks=  28
(narrow-bridge) Vehicle:    2, prio: emer, direct: l -> r, ticks=  28
(narrow-bridge) Vehicle:   12, prio: emer, direct: l <- r, ticks=  39
(narrow-bridge) Vehicle:   13, prio: emer, direct: l <- r, ticks=  39
(narrow-bridge) Vehicle:    3, prio: emer, direct: l -> r, ticks=  49
(narrow-bridge) Vehicle:    4, prio: emer, direct: l -> r, ticks=  49
(narrow-bridge) Vehicle:   14, prio: emer, direct: l <- r, ticks=  59
(narrow-bridge) Vehicle:   15, prio: emer, direct: l <- r, ticks=  59
(narrow-bridge) Vehicle:    5, prio: emer, direct: l -> r, ticks=  69
(narrow-bridge) Vehicle:    6, prio: emer, direct: l -> r, ticks=  69
(narrow-bridge) Vehicle:   16, prio: emer, direct: l <- r, ticks=  79
(narrow-bridge) Vehicle:   17, prio: emer, direct: l <- r, ticks=  79
(narrow-bridge) Vehicle:    7, prio: emer, direct: l -> r, ticks=  89
(narrow-bridge) Vehicle:    8, prio: emer, direct: l -> r, ticks=  89
(narrow-bridge) Vehicle:   18, prio: emer, direct: l <- r, ticks=  99
(narrow-bridge) Vehicle:   19, prio: emer, direct: l <- r, ticks=  99
(narrow-bridge) Vehicle:    9, prio: emer, direct: l -> r, ticks= 109
(narrow-bridge) Vehicle:   10, prio: emer, direct: l -> r, ticks= 109
(narrow-bridge) Vehicle:   20, prio: emer, direct: l <- r, ticks= 120
(narrow-bridge) Vehicle:   21, prio: emer, direct: l <- r, ticks= 120
(narrow-bridge) Vehicle:   11, prio: emer, direct: l -> r, ticks= 130
(narrow-bridge) Vehicle:   22, prio: emer, direct: l <- r, ticks= 140
(narrow-bridge) Vehicle:   23, prio: emer, direct: l <- r, ticks= 140
(narrow-bridge) end
```
pintos --qemu -- -q run "narrow-bridge 0 10 0 10" 
![[{FD8FFAF7-60BB-47E3-817B-B00A7E254FBF}.png]]
```
(narrow-bridge) begin
(narrow-bridge) Vehicle:    1, prio: norm, direct: l <- r, ticks=  28
(narrow-bridge) Vehicle:    2, prio: norm, direct: l <- r, ticks=  28
(narrow-bridge) Vehicle:   11, prio: emer, direct: l <- r, ticks=  38
(narrow-bridge) Vehicle:   12, prio: emer, direct: l <- r, ticks=  38
(narrow-bridge) Vehicle:   13, prio: emer, direct: l <- r, ticks=  48
(narrow-bridge) Vehicle:   14, prio: emer, direct: l <- r, ticks=  48
(narrow-bridge) Vehicle:   15, prio: emer, direct: l <- r, ticks=  58
(narrow-bridge) Vehicle:   16, prio: emer, direct: l <- r, ticks=  58
(narrow-bridge) Vehicle:   17, prio: emer, direct: l <- r, ticks=  68
(narrow-bridge) Vehicle:   18, prio: emer, direct: l <- r, ticks=  68
(narrow-bridge) Vehicle:   19, prio: emer, direct: l <- r, ticks=  78
(narrow-bridge) Vehicle:   20, prio: emer, direct: l <- r, ticks=  78
(narrow-bridge) Vehicle:    3, prio: norm, direct: l <- r, ticks=  88
(narrow-bridge) Vehicle:    4, prio: norm, direct: l <- r, ticks=  88
(narrow-bridge) Vehicle:    5, prio: norm, direct: l <- r, ticks=  98
(narrow-bridge) Vehicle:    6, prio: norm, direct: l <- r, ticks=  98
(narrow-bridge) Vehicle:    7, prio: norm, direct: l <- r, ticks= 108
(narrow-bridge) Vehicle:    8, prio: norm, direct: l <- r, ticks= 108
(narrow-bridge) Vehicle:    9, prio: norm, direct: l <- r, ticks= 118
(narrow-bridge) Vehicle:   10, prio: norm, direct: l <- r, ticks= 118
(narrow-bridge) end
```
pintos --qemu -- -q run "narrow-bridge 0 10 10 0"
![[{D2A17D7B-012E-49C1-840F-0524F2ED8A6A}.png]]
```
(narrow-bridge) begin
(narrow-bridge) Vehicle:    1, prio: norm, direct: l <- r, ticks=  27
(narrow-bridge) Vehicle:    2, prio: norm, direct: l <- r, ticks=  28
(narrow-bridge) Vehicle:   11, prio: emer, direct: l -> r, ticks=  38
(narrow-bridge) Vehicle:   12, prio: emer, direct: l -> r, ticks=  38
(narrow-bridge) Vehicle:   13, prio: emer, direct: l -> r, ticks=  48
(narrow-bridge) Vehicle:   14, prio: emer, direct: l -> r, ticks=  48
(narrow-bridge) Vehicle:   15, prio: emer, direct: l -> r, ticks=  58
(narrow-bridge) Vehicle:   16, prio: emer, direct: l -> r, ticks=  58
(narrow-bridge) Vehicle:   17, prio: emer, direct: l -> r, ticks=  68
(narrow-bridge) Vehicle:   18, prio: emer, direct: l -> r, ticks=  68
(narrow-bridge) Vehicle:   19, prio: emer, direct: l -> r, ticks=  78
(narrow-bridge) Vehicle:   20, prio: emer, direct: l -> r, ticks=  78
(narrow-bridge) Vehicle:    3, prio: norm, direct: l <- r, ticks=  88
(narrow-bridge) Vehicle:    4, prio: norm, direct: l <- r, ticks=  88
(narrow-bridge) Vehicle:    5, prio: norm, direct: l <- r, ticks=  98
(narrow-bridge) Vehicle:    6, prio: norm, direct: l <- r, ticks=  98
(narrow-bridge) Vehicle:    7, prio: norm, direct: l <- r, ticks= 108
(narrow-bridge) Vehicle:    8, prio: norm, direct: l <- r, ticks= 108
(narrow-bridge) Vehicle:    9, prio: norm, direct: l <- r, ticks= 118
(narrow-bridge) Vehicle:   10, prio: norm, direct: l <- r, ticks= 118
(narrow-bridge) end
```

##### 5. Результаты выполнения дополнительного индивидуального задания: описание алгоритма и его реализации, диаграмма состояния и взаимодействия процессов, результаты выполнения программ и анализ полученных результатов.

Был разработан алгоритм, основанный на работе двух семафоров: board - отвечающего за посадку/высадку (т.е. ожидание со стороны вагончика) и ride - отвечающего за поездку (т.е. ожидание со стороны пассажиров).

Логика алгоритма в упрощенном виде:
![[Pasted image 20250218033728.png]]

Инициализируются семафоры и количество необходимых для запуска вагончика пассажиров.

```c title=init
static void init(unsigned int carriage_size UNUSED)
{
  sema_init(&board, 0);
  sema_init(&ride, 0);
  
  needed_passengers = carriage_size;
}
```

Создается процесс вагончика, который выполняет команду down семафора board n раз, соответствующее количеству ожидаемых пассажиров, тем самым реализуя ожидание посадки, после чего "заполнения" выполняет timer_sleep и выполняет функцию "up" для семафора ride, тем самым разблокирует пассажиров для "выхода" из вагончика, после чего за счет функции down семафора board ожидает выхода n пассажиров.

```c title=cerriage
static void carriage(void* arg UNUSED)
{
  msg("carriage created.");
  
  while (1)
    {
      for (int i = 0; i < needed_passengers; i++)         
        {
          sema_down(&board);
        }
  
      msg("carriage filled.");
      msg("carriage started.");
  
      timer_sleep(10);                                    
  
      msg("carriage finished.");
      msg("passengers leaving.");
  
      for (int i = 0; i < needed_passengers; i++)         
        {
          sema_up(&ride);
        }
  
      for (int i = 0; i < needed_passengers; i++)         
        {
          sema_down(&board);
        }
  
      msg("carriage empty.");
    }
}
```

Создается процесс пассажиров, которые "заполняют" вагончик, постепенно его разблокируют функцией up семафора board, после чего используется функция down семафора ride, "блокируя" пассажиров на время поездки, после окончания поездки "разблокируют" вагончик, ожидающий высадки функцией up семафора board.

pintos --qemu -- -q run "rolcoast 10 3 100"
![[{E7200098-4C92-4719-B03D-F2998745C3AB}.png]]
```
(rolcoast) begin
(rolcoast) carriage created.
(rolcoast) passenger 1 created.
(rolcoast) passenger 1 boarded and wait.
(rolcoast) passenger 2 created.
(rolcoast) passenger 2 boarded and wait.
(rolcoast) passenger 3 created.
(rolcoast) passenger 3 boarded and wait.
(rolcoast) carriage filled.
(rolcoast) carriage started.
(rolcoast) carriage finished.
(rolcoast) passengers leaving.
(rolcoast) passenger 1 left.
(rolcoast) passenger 2 left.
(rolcoast) passenger 3 left.
(rolcoast) carriage empty.
(rolcoast) passenger 4 created.
(rolcoast) passenger 4 boarded and wait.
(rolcoast) passenger 5 created.
(rolcoast) passenger 5 boarded and wait.
(rolcoast) passenger 6 created.
(rolcoast) passenger 6 boarded and wait.
(rolcoast) carriage filled.
(rolcoast) carriage started.
(rolcoast) carriage finished.
(rolcoast) passengers leaving.
(rolcoast) passenger 4 left.
(rolcoast) passenger 5 left.
(rolcoast) passenger 6 left.
(rolcoast) carriage empty.
(rolcoast) passenger 7 created.
(rolcoast) passenger 7 boarded and wait.
(rolcoast) passenger 8 created.
(rolcoast) passenger 8 boarded and wait.
(rolcoast) passenger 9 created.
(rolcoast) passenger 9 boarded and wait.
(rolcoast) carriage filled.
(rolcoast) carriage started.
(rolcoast) carriage finished.
(rolcoast) passengers leaving.
(rolcoast) passenger 7 left.
(rolcoast) passenger 8 left.
(rolcoast) passenger 9 left.
(rolcoast) carriage empty.
(rolcoast) passenger 10 created.
(rolcoast) passenger 10 boarded and wait.
(rolcoast) PASS
(rolcoast) end
```

По результатам видно, что вагончик "уезжает" только при накоплении нужного кол-ва пассажиров, а пассажиры "садятся" и "выходят" только в неактивном состоянии вагончика, что говорит о корректности работы алгоритма и использования в нем примитив синхронизации.